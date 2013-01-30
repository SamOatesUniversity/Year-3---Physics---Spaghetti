#include "CSpaghettiBoundsSphere.h"

/*
*	\brief	Class constructor		
*/
CSpaghettiBoundsSphere::CSpaghettiBoundsSphere()
{
	m_type = BoundsType::Sphere;
}

/*
*	\brief	Class destructor
*/
CSpaghettiBoundsSphere::~CSpaghettiBoundsSphere()
{

}

/*
*	\brief	Does this bounding box intersect with another
*/
const bool CSpaghettiBoundsSphere::Intersects( 
		CSpaghettiBounds *other								//! The bounding box to test against
	)
{
	if (other->GetType() == BoundsType::Box)
	{
		CSpaghettiBoundsBox *const otherBox = static_cast<CSpaghettiBoundsBox*>(other);

		if (otherBox->GetTransform().X() + otherBox->GetMax().X() < m_position.X() - GetRadius()) return false;
		if (otherBox->GetTransform().X() + otherBox->GetMin().X() > m_position.X() + GetRadius()) return false;
		if (otherBox->GetTransform().Y() + otherBox->GetMax().Y() < m_position.Y() - GetRadius()) return false;
		if (otherBox->GetTransform().Y() + otherBox->GetMin().Y() > m_position.Y() + GetRadius()) return false;
		if (otherBox->GetTransform().Z() + otherBox->GetMax().Z() < m_position.Z() - GetRadius()) return false;
		if (otherBox->GetTransform().Z() + otherBox->GetMin().Z() > m_position.Z() + GetRadius()) return false;

		return true;
	}
	else if (other->GetType() == BoundsType::Sphere)
	{
		CSpaghettiBoundsSphere *const otherSphere = static_cast<CSpaghettiBoundsSphere*>(other);

		if (m_position.X() + GetRadius() < otherSphere->GetTransform().X() - otherSphere->GetRadius()) return false;
		if (m_position.X() - GetRadius() > otherSphere->GetTransform().X() + otherSphere->GetRadius()) return false;
		if (m_position.Y() + GetRadius() < otherSphere->GetTransform().Y() - otherSphere->GetRadius()) return false;
		if (m_position.Y() - GetRadius() > otherSphere->GetTransform().Y() + otherSphere->GetRadius()) return false;
		if (m_position.Z() + GetRadius() < otherSphere->GetTransform().Z() - otherSphere->GetRadius()) return false;
		if (m_position.Z() - GetRadius() > otherSphere->GetTransform().Z() + otherSphere->GetRadius()) return false;

		return true;
	}

	return false;
}