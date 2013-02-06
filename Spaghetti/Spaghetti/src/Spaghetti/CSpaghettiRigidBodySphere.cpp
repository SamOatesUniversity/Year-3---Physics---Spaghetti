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

	m_velocity = m_velocity + (world->GetGravity() * deltaTime);
	m_angularMomentum = m_angularMomentum;

	UpdateInertiaTensor();
	UpdateAngularVelocity();
}

/*
*	\brief	Update the rigid bodies position
*/
void CSpaghettiRigidBodySphere::UpdatePosition(
		CSpaghettiWorld	*world,										//!< The world we are moving in
		const float deltaTime										//!< Delta time (The amount of time past since the last update)
	)
{
	m_lastPosition = m_position;
	m_position = m_position + (m_velocity * deltaTime);

	// update skew matrix
	SAM::TMatrix<float, 3, 3> skewMatrix;
	skewMatrix[0][0] = 0.0f;
	skewMatrix[0][1] = -m_angularVelocity.Z();
	skewMatrix[0][2] = m_angularVelocity.Y();

	skewMatrix[1][0] = m_angularVelocity.Z();
	skewMatrix[1][1] = 0.0f;
	skewMatrix[1][2] = -m_angularVelocity.X();

	skewMatrix[2][0] = -m_angularVelocity.Y();
	skewMatrix[2][1] = m_angularVelocity.X();
	skewMatrix[2][2] = 0.0f;

	// update rotation matrix
	m_rotation = m_rotation + ((skewMatrix * m_rotation) * deltaTime);

	// update and normalize the quaternion
	m_quaternion.FromMatrix3x3(m_rotation);
	m_quaternion.Normalize();

	m_bounds->Transform(m_position, m_quaternion);
}

/*
*	\brief	Handle collision against another rigid body
*/
void CSpaghettiRigidBodySphere::HandleCollision(
	CSpaghettiWorld	*world,											//!< The world we are moving in
	CSpaghettiRigidBody *otherRigidBody								//!< The other rigid body to compare against
	)
{

}
