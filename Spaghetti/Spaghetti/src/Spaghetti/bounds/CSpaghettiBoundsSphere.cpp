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
		CSpaghettiBounds *other,							//!< The bounding box to test against
		std::vector<CCollision> &collision
	)
{
	if (other->GetType() == BoundsType::Box)
	{
		CSpaghettiBoundsBox *const otherBox = static_cast<CSpaghettiBoundsBox*>(other);

		if (otherBox->GetPosition().x + otherBox->GetMax().x < m_position.x - GetRadius()) return false;
		if (otherBox->GetPosition().x + otherBox->GetMin().x > m_position.x + GetRadius()) return false;
		if (otherBox->GetPosition().y + otherBox->GetMax().y < m_position.y - GetRadius()) return false;
		if (otherBox->GetPosition().y + otherBox->GetMin().y > m_position.y + GetRadius()) return false;
		if (otherBox->GetPosition().z + otherBox->GetMax().z < m_position.z - GetRadius()) return false;
		if (otherBox->GetPosition().z + otherBox->GetMin().z > m_position.z + GetRadius()) return false;

		CCollision newCollision;
		newCollision.bodyOne = GetBody();
		newCollision.bodyTwo = otherBox->GetBody();
		newCollision.collisionNormal = Ogre::Vector3(0, -1, 0);
		newCollision.collisionPoint = m_position - Ogre::Vector3(0, Height() * 0.5f, 0);

		collision.push_back(newCollision);

		return true;
	}
	else if (other->GetType() == BoundsType::Sphere)
	{
		CSpaghettiBoundsSphere *const otherSphere = static_cast<CSpaghettiBoundsSphere*>(other);
		const float distance = (otherSphere->GetPosition() - m_position).length();
		const float radius = GetRadius() + otherSphere->GetRadius();

		if (distance < radius)
		{
			CCollision newCollision;
			newCollision.bodyOne = GetBody();
			newCollision.bodyTwo = otherSphere->GetBody();
			newCollision.collisionNormal = Ogre::Vector3(0, -1, 0);
			newCollision.collisionPoint = m_position - Ogre::Vector3(0, Height() * 0.5f, 0);

			collision.push_back(newCollision);

			return true;
		}

		return false;
	}

	return false;
}
