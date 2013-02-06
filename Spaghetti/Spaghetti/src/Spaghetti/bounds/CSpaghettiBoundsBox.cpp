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
	return
		(box->Width() * 0.5f) * fabsf(axis * box->GetAxis(0)) +
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
		SAM::TVector3 &toCenter
	)
{
	// Project the half-size of one onto axis
	const float oneProject = TransformToAxis(one, axis);
	const float twoProject = TransformToAxis(two, axis);

	// Project this onto the axis
	const float distance = fabsf(toCenter * axis);

	// Check for overlap
	return (distance <= oneProject + twoProject);
}

/*
*	\brief	Does this bounding box intersect with another
*/
const bool CSpaghettiBoundsBox::Intersects( 
		CSpaghettiBounds *other								//! The bounding box to test against
	)
{
	if (other->GetType() == BoundsType::Box)
	{
		CSpaghettiBoundsBox *const otherBox = static_cast<CSpaghettiBoundsBox*>(other);

		SAM::TVector3 toCenter = other->GetPosition() - GetPosition();

		if (!CheckForAxisOverlap(this, otherBox, GetAxis(0), toCenter)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(1), toCenter)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(2), toCenter)) return false;

		if (!CheckForAxisOverlap(this, otherBox, otherBox->GetAxis(0), toCenter)) return false;
		if (!CheckForAxisOverlap(this, otherBox, otherBox->GetAxis(1), toCenter)) return false;
		if (!CheckForAxisOverlap(this, otherBox, otherBox->GetAxis(2), toCenter)) return false;

		if (!CheckForAxisOverlap(this, otherBox, GetAxis(0).Cross(otherBox->GetAxis(0)), toCenter)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(0).Cross(otherBox->GetAxis(1)), toCenter)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(0).Cross(otherBox->GetAxis(2)), toCenter)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(1).Cross(otherBox->GetAxis(0)), toCenter)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(1).Cross(otherBox->GetAxis(1)), toCenter)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(1).Cross(otherBox->GetAxis(2)), toCenter)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(2).Cross(otherBox->GetAxis(0)), toCenter)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(2).Cross(otherBox->GetAxis(1)), toCenter)) return false;
		if (!CheckForAxisOverlap(this, otherBox, GetAxis(2).Cross(otherBox->GetAxis(2)), toCenter)) return false;

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
		SAM::TVector<float, 3> &position,						//!< The position of the bounds in world space 
		SAM::TQuaternion &rotation								//!< The rotation of the bounds in local space 
	)
{
	m_position = position;
	
	m_min.Zero();
	m_max.Zero();

	SAM::TMatrix<float, 3, 3> rotationMatrix = rotation.ToMatrix3x3();
	for (int cornerIndex = 0; cornerIndex < NOOF_BOUNDINGBOX_CORNERS; ++cornerIndex)
	{
		m_corner[cornerIndex] = rotationMatrix * m_aaCorner[cornerIndex];

		if (m_corner[cornerIndex].X() < m_min.X()) m_min.SetX(m_corner[cornerIndex].X());
		if (m_corner[cornerIndex].Y() < m_min.Y()) m_min.SetY(m_corner[cornerIndex].Y());
		if (m_corner[cornerIndex].Z() < m_min.Z()) m_min.SetZ(m_corner[cornerIndex].Z());

		if (m_corner[cornerIndex].X() > m_max.X()) m_max.SetX(m_corner[cornerIndex].X());
		if (m_corner[cornerIndex].Y() > m_max.Y()) m_max.SetY(m_corner[cornerIndex].Y());
		if (m_corner[cornerIndex].Z() > m_max.Z()) m_max.SetZ(m_corner[cornerIndex].Z());
	}
}
