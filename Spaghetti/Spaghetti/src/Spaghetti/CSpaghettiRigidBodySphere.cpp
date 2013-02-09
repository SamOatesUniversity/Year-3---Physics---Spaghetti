#include "CSpaghettiRigidBodySphere.h"

/*
*	\brief Class constructor
*/
CSpaghettiRigidBodySphere::CSpaghettiRigidBodySphere(
	void *renderObject									//!< A pointer to an object this rigid body will represent
	) : CSpaghettiRigidBody(renderObject)
{

}

/*
*	\brief Class destructor
*/
CSpaghettiRigidBodySphere::~CSpaghettiRigidBodySphere()
{

}

/*
*	\brief	Calculate the inertia based upon the bounding sphere
*/
void CSpaghettiRigidBodySphere::CalculateInertiaBodyTensor()
{
	static const float twoOverFive = 2.0f / 5.0f;
	const float radius = m_bounds->Height() * 0.5f;
	const float radiusSquared = radius * radius;

	m_inertiaTensor[0][0] = twoOverFive * (m_mass * radiusSquared); m_inertiaTensor[0][1] = 0; m_inertiaTensor[0][2] = 0;
	m_inertiaTensor[1][0] = 0; m_inertiaTensor[1][1] = twoOverFive * (m_mass * radiusSquared);  m_inertiaTensor[1][2] = 0;
	m_inertiaTensor[2][0] = 0; m_inertiaTensor[2][1] = 0; m_inertiaTensor[2][2] = twoOverFive * (m_mass * radiusSquared);

	m_inertiaTensorBodyInverse = m_inertiaTensor.Inverse();
}

/*
*	\brief Update all simulation objects in the world
*/
void CSpaghettiRigidBodySphere::UpdateVelocity( 
	const CSpaghettiWorld *world,
	const float deltaTime							//!< Delta time (The amount of time past since the last update)
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
	m_angularMomentum *= 0.99f;

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
