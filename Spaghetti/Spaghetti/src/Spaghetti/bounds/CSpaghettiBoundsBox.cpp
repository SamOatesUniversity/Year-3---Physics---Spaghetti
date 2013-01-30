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

		if (m_position.X() + m_max.X() < otherBox->GetTransform().X() + otherBox->GetMin().X()) return false;
		if (m_position.X() + m_min.X() > otherBox->GetTransform().X() + otherBox->GetMax().X()) return false;
		if (m_position.Y() + m_max.Y() < otherBox->GetTransform().Y() + otherBox->GetMin().Y()) return false;
		if (m_position.Y() + m_min.Y() > otherBox->GetTransform().Y() + otherBox->GetMax().Y()) return false;
		if (m_position.Z() + m_max.Z() < otherBox->GetTransform().Z() + otherBox->GetMin().Z()) return false;
		if (m_position.Z() + m_min.Z() > otherBox->GetTransform().Z() + otherBox->GetMax().Z()) return false;

		return true;
	}
	else if (other->GetType() == BoundsType::Sphere)
	{
		CSpaghettiBoundsSphere *const otherSphere = static_cast<CSpaghettiBoundsSphere*>(other);

		if (m_position.X() + m_max.X() < otherSphere->GetTransform().X() - otherSphere->GetRadius()) return false;
		if (m_position.X() + m_min.X() > otherSphere->GetTransform().X() + otherSphere->GetRadius()) return false;
		if (m_position.Y() + m_max.Y() < otherSphere->GetTransform().Y() - otherSphere->GetRadius()) return false;
		if (m_position.Y() + m_min.Y() > otherSphere->GetTransform().Y() + otherSphere->GetRadius()) return false;
		if (m_position.Z() + m_max.Z() < otherSphere->GetTransform().Z() - otherSphere->GetRadius()) return false;
		if (m_position.Z() + m_min.Z() > otherSphere->GetTransform().Z() + otherSphere->GetRadius()) return false;

		return true;
	}
	
	return false;
}