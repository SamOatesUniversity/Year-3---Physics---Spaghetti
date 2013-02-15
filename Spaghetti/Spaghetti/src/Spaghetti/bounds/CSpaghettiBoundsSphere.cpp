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
		// Sphere on box
		const bool result = other->Intersects(this, collision);
		if (!result)
			return false;

		return true;
	}
	else if (other->GetType() == BoundsType::Sphere)
	{
		// Sphere on sphere
		CSpaghettiBoundsSphere *const otherSphere = static_cast<CSpaghettiBoundsSphere*>(other);
		const float distance = (m_position - otherSphere->GetPosition()).length();
		const float radius = GetRadius() + otherSphere->GetRadius();

		if (distance <= radius)
		{
			Ogre::Vector3 collisionNormal = otherSphere->GetPosition() - GetPosition();
			collisionNormal.normalise();

			Ogre::Vector3 collisionPoint =collisionNormal * GetRadius();

			CCollision newCollision;
			newCollision.bodyOne = GetBody();
			newCollision.bodyTwo = otherSphere->GetBody();
			newCollision.collisionNormal = collisionNormal;
			newCollision.collisionPoint = GetPosition() + collisionPoint;
			newCollision.penetration = radius - distance;

			collision.push_back(newCollision);

			return true;
		}

		return false;
	}

	return false;
}
