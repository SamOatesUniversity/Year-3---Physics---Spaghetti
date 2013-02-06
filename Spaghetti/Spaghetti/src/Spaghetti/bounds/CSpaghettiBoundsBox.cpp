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
*	\brief	Does this bounding box intersect with another
*/
const bool CSpaghettiBoundsBox::Intersects( 
		CSpaghettiBounds *other								//! The bounding box to test against
	)
{
	if (other->GetType() == BoundsType::Box)
	{
		CSpaghettiBoundsBox *const otherBox = static_cast<CSpaghettiBoundsBox*>(other);

		if (otherBox->GetPosition().X() + otherBox->GetMax().X() < GetPosition().X() - GetMax().X()) return false;
		if (otherBox->GetPosition().X() + otherBox->GetMin().X() > GetPosition().X() + GetMax().X()) return false;
		if (otherBox->GetPosition().Y() + otherBox->GetMax().Y() < GetPosition().Y() - GetMax().Y()) return false;
		if (otherBox->GetPosition().Y() + otherBox->GetMin().Y() > GetPosition().Y() + GetMax().Y()) return false;
		if (otherBox->GetPosition().Z() + otherBox->GetMax().Z() < GetPosition().Z() - GetMax().Z()) return false;
		if (otherBox->GetPosition().Z() + otherBox->GetMin().Z() > GetPosition().Z() + GetMax().Z()) return false;

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
