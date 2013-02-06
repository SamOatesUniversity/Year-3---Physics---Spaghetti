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

		if (m_corner[cornerIndex].SumOf() < m_min.SumOf())
			m_min = m_corner[cornerIndex];

		if (m_corner[cornerIndex].SumOf() > m_max.SumOf())
			m_max = m_corner[cornerIndex];
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
