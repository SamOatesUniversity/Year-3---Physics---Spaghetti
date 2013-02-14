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
		// Box on sphere

		// sphere on box

		CSpaghettiBoundsBox *const otherBox = static_cast<CSpaghettiBoundsBox*>(other);

		// quick aabb test, for early out
		if (otherBox->GetPosition().x + otherBox->GetMax().x < GetPosition().x - GetRadius()) return false;
		if (otherBox->GetPosition().x + otherBox->GetMin().x > GetPosition().x + GetRadius()) return false;
		if (otherBox->GetPosition().y + otherBox->GetMax().y < GetPosition().y - GetRadius()) return false;
		if (otherBox->GetPosition().y + otherBox->GetMin().y > GetPosition().y + GetRadius()) return false;
		if (otherBox->GetPosition().z + otherBox->GetMax().z < GetPosition().z - GetRadius()) return false;
		if (otherBox->GetPosition().z + otherBox->GetMin().z > GetPosition().z + GetRadius()) return false;

		// potentially a collision, check against object aligned box

		// transform the spheres center of mass into the boxes space
		Ogre::Vector3 sphereCenter = GetPosition();
		Ogre::Vector3 sphereInBoxSpace = sphereCenter;
		sphereInBoxSpace.x -= otherBox->GetTransform()[0][3];
		sphereInBoxSpace.y -= otherBox->GetTransform()[1][3];
		sphereInBoxSpace.z -= otherBox->GetTransform()[2][3];

		sphereInBoxSpace = Ogre::Vector3(
			sphereInBoxSpace.x * otherBox->GetTransform()[0][0] +
			sphereInBoxSpace.y * otherBox->GetTransform()[1][0] +
			sphereInBoxSpace.z * otherBox->GetTransform()[2][0],

			sphereInBoxSpace.x * otherBox->GetTransform()[0][1] +
			sphereInBoxSpace.y * otherBox->GetTransform()[1][1] +
			sphereInBoxSpace.z * otherBox->GetTransform()[2][1],

			sphereInBoxSpace.x * otherBox->GetTransform()[0][2] +
			sphereInBoxSpace.y * otherBox->GetTransform()[1][2] +
			sphereInBoxSpace.z * otherBox->GetTransform()[2][2]			
		);

		float dist = 0.0f;
		Ogre::Vector3 closestPoint = Ogre::Vector3::ZERO;
		Ogre::Vector3 halfBox = otherBox->GetBoxSize() * 0.5f;

		// Clamp each coordinate to the box.
		dist = sphereInBoxSpace.x;
		if (dist > halfBox.x) dist = halfBox.x;
		if (dist < -halfBox.x) dist = -halfBox.x;
		closestPoint.x = dist;

		dist = sphereInBoxSpace.y;
		if (dist > halfBox.y) dist = halfBox.y;
		if (dist < -halfBox.y) dist = -halfBox.y;
		closestPoint.y = dist;

		dist = sphereInBoxSpace.z;
		if (dist > halfBox.z) dist = halfBox.z;
		if (dist < -halfBox.z) dist = -halfBox.z;
		closestPoint.z = dist;

		// Check we're in contact
		dist = (closestPoint - sphereInBoxSpace).squaredLength();
		if (dist > GetRadius() * GetRadius())
			return false;

		// Calculate the collision points
		Ogre::Vector3 closestPtWorld = otherBox->GetTransform() * closestPoint;
		Ogre::Vector3 collisionNormal = sphereCenter - closestPtWorld;
		collisionNormal.normalise();

		// add the collision to our collision list
		CCollision newCollision;
		newCollision.bodyOne = otherBox->GetBody();
		newCollision.bodyTwo = GetBody();
		newCollision.collisionNormal = collisionNormal;
		newCollision.collisionPoint = closestPtWorld;

		collision.push_back(newCollision);

		return true;
	}
	else if (other->GetType() == BoundsType::Sphere)
	{
		// Sphere on sphere

		CSpaghettiBoundsSphere *const otherSphere = static_cast<CSpaghettiBoundsSphere*>(other);
		const float distance = (otherSphere->GetPosition() - m_position).length();
		const float radius = GetRadius() + otherSphere->GetRadius();

		if (distance < radius)
		{
			Ogre::Vector3 collisionNormal = otherSphere->GetPosition() - GetPosition();
			collisionNormal.normalise();

			Ogre::Vector3 collisionPoint =collisionNormal * GetRadius();

			CCollision newCollision;
			newCollision.bodyOne = GetBody();
			newCollision.bodyTwo = otherSphere->GetBody();
			newCollision.collisionNormal = collisionNormal;
			newCollision.collisionPoint = GetPosition() + collisionPoint;

			collision.push_back(newCollision);

			return true;
		}

		return false;
	}

	return false;
}
