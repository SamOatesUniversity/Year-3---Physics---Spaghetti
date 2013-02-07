#include "CSpaghettiBoundsBox.h"

/*
*	\brief	Class constructor		
*/
CSpaghettiBoundsBox::CSpaghettiBoundsBox()
{
	m_type = BoundsType::Box;
}

/*
*	\brief	Class destructor
*/
CSpaghettiBoundsBox::~CSpaghettiBoundsBox()
{

}

/*
*	\brief	Set the corners of the bounding box, the min and max will also be calculated in here
*/
void CSpaghettiBoundsBox::SetCorners( 
		SAM::TVector<float, 3> *corners					//!< A pointer to the positions of the corners
	)
{
	for (int cornerIndex = 0; cornerIndex < NOOF_BOUNDINGBOX_CORNERS; ++cornerIndex)
	{
		m_corner[cornerIndex] = corners[cornerIndex];
		m_aaCorner[cornerIndex] = corners[cornerIndex];

		if (m_corner[cornerIndex].X() < m_min.X()) m_min.SetX(m_corner[cornerIndex].X());
		if (m_corner[cornerIndex].Y() < m_min.Y()) m_min.SetY(m_corner[cornerIndex].Y());
		if (m_corner[cornerIndex].Z() < m_min.Z()) m_min.SetZ(m_corner[cornerIndex].Z());

		if (m_corner[cornerIndex].X() > m_max.X()) m_max.SetX(m_corner[cornerIndex].X());
		if (m_corner[cornerIndex].Y() > m_max.Y()) m_max.SetY(m_corner[cornerIndex].Y());
		if (m_corner[cornerIndex].Z() > m_max.Z()) m_max.SetZ(m_corner[cornerIndex].Z());
	}
}

/*
*	\brief	
*/
float TransformToAxis(
		CSpaghettiBoundsBox *box,
		const SAM::TVector3 &axis
	)
{
	return (box->Width() * 0.5f) * fabsf(axis * box->GetAxis(0)) +
		(box->Height() * 0.5f) * fabsf(axis * box->GetAxis(1)) +
		(box->Depth() * 0.5f) * fabsf(axis * box->GetAxis(2));
}

/*
*	\brief 
*/
const bool CheckForAxisOverlap(
		CSpaghettiBoundsBox *one,
		CSpaghettiBoundsBox *two,
		SAM::TVector3 &axis,
		SAM::TVector3 &toCenter,
		float &penetration,
		unsigned int &best,
		unsigned int id
	)
{
	if (axis.LengthSquared() < 0.0001f) 
return true;
	axis.Normalize();

	// Project the half-size of one onto axis
	const float oneProject = TransformToAxis(one, axis);
	const float twoProject = TransformToAxis(two, axis);

	// Project this onto the axis
	const float distance = fabsf(toCenter * axis);
	const float p = oneProject + twoProject - distance;

	if (p < 0)
		return false;

	if (p < penetration)
	{
		penetration = p;
		best = id;
	}

	return true;
}

/*
*	\brief	Does this bounding box intersect with another
*/
const bool CSpaghettiBoundsBox::Intersects( 
		CSpaghettiBounds *other,								//!< The bounding box to test against
		std::vector<CCollision> &collision	
	)
{
	collision.clear();

	if (other->GetType() == BoundsType::Box)
	{
		CSpaghettiBoundsBox *const otherBox = static_cast<CSpaghettiBoundsBox*>(other);

		SAM::TVector3 toCenter = other->GetPosition() - GetPosition();
		float penetration = 1000000.0f;
		unsigned int best = (unsigned int)-1;

		if (!CheckForAxisOverlap(this, otherBox, GetAxis(0), toCenter, penetration, best, 0)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(1), toCenter, penetration, best, 1)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(2), toCenter, penetration, best, 2)) return false;

		if (!CheckForAxisOverlap(this, otherBox, otherBox->GetAxis(0), toCenter, penetration, best, 3)) return false;
		if (!CheckForAxisOverlap(this, otherBox, otherBox->GetAxis(1), toCenter, penetration, best, 4)) return false;
		if (!CheckForAxisOverlap(this, otherBox, otherBox->GetAxis(2), toCenter, penetration, best, 5)) return false;

		if (!CheckForAxisOverlap(this, otherBox, GetAxis(0).Cross(otherBox->GetAxis(0)), toCenter, penetration, best, 6)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(0).Cross(otherBox->GetAxis(1)), toCenter, penetration, best, 7)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(0).Cross(otherBox->GetAxis(2)), toCenter, penetration, best, 8)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(1).Cross(otherBox->GetAxis(0)), toCenter, penetration, best, 9)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(1).Cross(otherBox->GetAxis(1)), toCenter, penetration, best, 10)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(1).Cross(otherBox->GetAxis(2)), toCenter, penetration, best, 11)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(2).Cross(otherBox->GetAxis(0)), toCenter, penetration, best, 12)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(2).Cross(otherBox->GetAxis(1)), toCenter, penetration, best, 13)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(2).Cross(otherBox->GetAxis(2)), toCenter, penetration, best, 14)) return false;

		if (best < 3)
		{
			SAM::TVector3 contactNormal = GetAxis(best);
			if (GetAxis(best) * toCenter > 0)
			{
				contactNormal = contactNormal * -1.0f;
			}

			static float mults[8][3] = {{1,1,1},{-1,1,1},{1,-1,1},{-1,-1,1},{1,1,-1},{-1,1,-1},{1,-1,-1},{-1,-1,-1}};
			for (unsigned i = 0; i < 8; i++) {

				// Calculate the position of each vertex
				SAM::TVector3 vertexPos;
				vertexPos.Set(mults[i][0], mults[i][1], mults[i][2]);

				SAM::TVector3 halfSize = (GetMax() - GetMin()) * 0.5f;

				vertexPos = vertexPos.Mul(halfSize);
				vertexPos = m_xform.Transform(vertexPos);

				// Calculate the distance from the plane
				float vertexDistance = vertexPos * contactNormal;

				// Compare this to the plane's distance
				//if (vertexDistance <= 1.0f)
				{
					// Create the contact data.
					CCollision newCollision;
					newCollision.bodyOne = GetBody();
					newCollision.bodyTwo = other->GetBody();
					newCollision.collisionNormal = contactNormal;
					newCollision.collisionPoint = vertexPos;
					collision.push_back(newCollision);
				}
			}
		}
		else if (best < 6)
		{
			best -= 3;

			SAM::TVector3 contactNormal = other->GetAxis(best);
			if (other->GetAxis(best) * (toCenter * -1.0f) > 0)
			{
				contactNormal = contactNormal * -1.0f;
			}

			static float mults[8][3] = {{1,1,1},{-1,1,1},{1,-1,1},{-1,-1,1},{1,1,-1},{-1,1,-1},{1,-1,-1},{-1,-1,-1}};
			for (unsigned i = 0; i < 8; i++) {

				// Calculate the position of each vertex
				SAM::TVector3 vertexPos;
				vertexPos.Set(mults[i][0], mults[i][1], mults[i][2]);

				SAM::TVector3 halfSize = (otherBox->GetMax() - otherBox->GetMin()) * 0.5f;

				vertexPos = vertexPos.Mul(halfSize);
				vertexPos = m_xform.Transform(vertexPos);

				// Calculate the distance from the plane
				float vertexDistance = vertexPos * contactNormal;

				// Compare this to the plane's distance
				//if (vertexDistance <= 1.0f)
				{
					// Create the contact data.
					CCollision newCollision;
					newCollision.bodyOne = other->GetBody();
					newCollision.bodyTwo = GetBody();
					newCollision.collisionNormal = contactNormal;
					newCollision.collisionPoint = vertexPos;
					collision.push_back(newCollision);
				}
			}
		}
		else
		{
			int i = 0;
			i = 1;
		}

		return true;
	}
	else if (other->GetType() == BoundsType::Sphere)
	{
		CSpaghettiBoundsSphere *const otherSphere = static_cast<CSpaghettiBoundsSphere*>(other);

		if (m_position.X() + m_max.X() < otherSphere->GetPosition().X() - otherSphere->GetRadius()) return false;
		if (m_position.X() + m_min.X() > otherSphere->GetPosition().X() + otherSphere->GetRadius()) return false;
		if (m_position.Y() + m_max.Y() < otherSphere->GetPosition().Y() - otherSphere->GetRadius()) return false;
		if (m_position.Y() + m_min.Y() > otherSphere->GetPosition().Y() + otherSphere->GetRadius()) return false;
		if (m_position.Z() + m_max.Z() < otherSphere->GetPosition().Z() - otherSphere->GetRadius()) return false;
		if (m_position.Z() + m_min.Z() > otherSphere->GetPosition().Z() + otherSphere->GetRadius()) return false;

		return true;
	}
	
	return false;
}

void CSpaghettiBoundsBox::Transform( 
		SAM::TVector<float, 3> position,						//!< The position of the bounds in world space 
		SAM::TMatrix<float, 3, 3> rotation						//!< The rotation of the bounds in local space 
	)
{
	m_position = position;

	m_axis[0].Set(1, 0, 0);
	m_axis[1].Set(0, 1, 0);	
	m_axis[2].Set(0, 0, 1);
	m_axis[0] = rotation * m_axis[0];
	m_axis[1] = rotation * m_axis[1];
	m_axis[2] = rotation * m_axis[2];

	SAM::TMatrix<float, 4, 4> r = rotation.ToMatrix4x4();
	SAM::TMatrix<float, 4, 4> t;
	t.Translate(m_position.X(), m_position.Y(), m_position.Z());
	m_xform = r * t;
	
	m_min.Zero(); m_max.Zero();
	for (int cornerIndex = 0; cornerIndex < NOOF_BOUNDINGBOX_CORNERS; ++cornerIndex)
	{
		m_corner[cornerIndex] = rotation * m_aaCorner[cornerIndex];

		if (m_corner[cornerIndex].X() < m_min.X()) m_min.SetX(m_corner[cornerIndex].X());
		if (m_corner[cornerIndex].Y() < m_min.Y()) m_min.SetY(m_corner[cornerIndex].Y());
		if (m_corner[cornerIndex].Z() < m_min.Z()) m_min.SetZ(m_corner[cornerIndex].Z());

		if (m_corner[cornerIndex].X() > m_max.X()) m_max.SetX(m_corner[cornerIndex].X());
		if (m_corner[cornerIndex].Y() > m_max.Y()) m_max.SetY(m_corner[cornerIndex].Y());
		if (m_corner[cornerIndex].Z() > m_max.Z()) m_max.SetZ(m_corner[cornerIndex].Z());
	}
}
