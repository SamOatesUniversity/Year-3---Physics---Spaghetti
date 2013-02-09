#include "CSpaghettiRigidBodyBox.h"

/*
*	\brief Class constructor
*/
CSpaghettiRigidBodyBox::CSpaghettiRigidBodyBox(
		void *renderObject									//!< A pointer to an object this rigid body will represent
	) : CSpaghettiRigidBody(renderObject)
{

}

/*
*	\brief Class destructor
*/
CSpaghettiRigidBodyBox::~CSpaghettiRigidBodyBox()
{

}

/*
*	\brief	Calculate the inertia based upon the bounding box
*/
void CSpaghettiRigidBodyBox::CalculateInertiaBodyTensor()
{
	static const float oneOverTwelve = 1.0f / 12.0f;
	const float heightSquared = m_bounds->Height() * m_bounds->Height();
	const float depthSquared = m_bounds->Depth() * m_bounds->Depth();
	const float widthSquared = m_bounds->Width() * m_bounds->Width();

	m_inertiaTensor[0][0] = (oneOverTwelve * m_mass) * (heightSquared + depthSquared); m_inertiaTensor[0][1] = 0; m_inertiaTensor[0][2] = 0;
	m_inertiaTensor[1][0] = 0; m_inertiaTensor[1][1] = (oneOverTwelve * m_mass) * (widthSquared + depthSquared);  m_inertiaTensor[1][2] = 0;
	m_inertiaTensor[2][0] = 0; m_inertiaTensor[2][1] = 0; m_inertiaTensor[2][2] = (oneOverTwelve * m_mass) * (widthSquared + heightSquared);

	m_inertiaTensorBodyInverse = m_inertiaTensor.Inverse();
}

/*
*	\brief Update all simulation objects in the world
*/
void CSpaghettiRigidBodyBox::UpdateVelocity( 
		const CSpaghettiWorld *world,
		const float deltaTime									//!< Delta time (The amount of time past since the last update)
	)
{
	// Nothing to update if he rigid body is static or disabled
	if (m_flags.isStatic || !m_flags.isEnabled)
		return;

	// add gravity
	AddForceAtPoint(world->GetGravity(), m_position);

	// update velocity
	m_velocity = m_velocity + (m_force * deltaTime);

	// update the position of the body
	m_lastPosition = m_position;
	m_position = m_position + m_velocity;

	// update angular momentum
	m_angularMomentum = m_angularMomentum + (m_torque * deltaTime);

	UpdateInertiaTensor();
	UpdateAngularVelocity();

	// construct the skew matrix
	Ogre::Matrix3 skewMatrix;
	skewMatrix[0][0] = 0.0f;
	skewMatrix[0][1] = -m_angularVelocity.z;
	skewMatrix[0][2] = m_angularVelocity.y;

	skewMatrix[1][0] = m_angularVelocity.z;
	skewMatrix[1][1] = 0.0f;
	skewMatrix[1][2] = -m_angularVelocity.x;

	skewMatrix[2][0] = -m_angularVelocity.y;
	skewMatrix[2][1] = m_angularVelocity.x;
	skewMatrix[2][2] = 0.0f;

	// update rotation matrix
	m_rotation = m_rotation + (skewMatrix * m_rotation);

	// update and normalize the quaternion
	m_quaternion.FromRotationMatrix(m_rotation);
	m_quaternion.normalise();

	// transform the bounding box data
	m_bounds->Transform(m_position, m_quaternion);

	// Zero out force and torque
	m_force = Ogre::Vector3::ZERO;
	m_torque = Ogre::Vector3::ZERO;
}

/*
*	\brief	Handle collision against another rigid body
*/
void CSpaghettiRigidBodyBox::HandleCollision(
		CSpaghettiWorld	*world,											//!< The world we are moving in
		CSpaghettiRigidBody *otherRigidBody								//!< The other rigid body to compare against
	)
{
	std::vector<CCollision> collisions;
	if (!m_bounds->Intersects(otherRigidBody->GetBounds(), collisions))
		return;

	SetPosition(GetLastPosition());
	otherRigidBody->SetPosition(otherRigidBody->GetLastPosition());

	static const float e = 1.2f;
	Ogre::Vector3 relativeVelocity = GetVelocity() - otherRigidBody->GetVelocity();

	// linear
	{
		const float inverseMassBodyOne = 1.0f / GetMass();
		const float inverseMassBodyTwo = 1.0f / otherRigidBody->GetMass();
		const float sumOfInverseMass = inverseMassBodyOne + inverseMassBodyTwo;

		const unsigned int noofCollision = collisions.size();
		for (unsigned int collisionIndex = 0; collisionIndex < noofCollision; ++collisionIndex)
		{
			Ogre::Vector3 collisionNormal = collisions[collisionIndex].collisionNormal;
			const float jLinear = (-(1.0f + e) * relativeVelocity.dotProduct(collisionNormal)) / sumOfInverseMass;

			Ogre::Vector3 bodyOneImpulse = (jLinear * collisionNormal) * inverseMassBodyOne;
			Ogre::Vector3 bodyTwoImpulse = (-jLinear * collisionNormal) * inverseMassBodyTwo;

			SetVelocity(GetVelocity() + bodyOneImpulse);
			otherRigidBody->SetVelocity(otherRigidBody->GetVelocity() + bodyTwoImpulse);
		}
	}		

	// angular
	{

	}
}
