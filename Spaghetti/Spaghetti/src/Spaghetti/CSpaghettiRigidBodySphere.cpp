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
void CSpaghettiRigidBodySphere::Update( 
	const CSpaghettiWorld *world,
	const unsigned long deltaTime							//!< Delta time (The amount of time past since the last update)
	)
{
	// Nothing to update if he rigid body is static or disabled
	if (m_flags.isStatic || !m_flags.isEnabled)
		return;

	static const float SPEEDSCALER = 0.0025f;

	m_lastPosition = m_position;

	m_velocity = m_velocity + world->GetGravity() * (static_cast<float>(deltaTime) * SPEEDSCALER);
	m_position = m_position + (m_velocity * (static_cast<float>(deltaTime) * SPEEDSCALER));
	m_angularMomentum = m_angularMomentum;

	UpdateInertiaTensor();
	UpdateAngularVelocity();

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
	m_rotation = m_rotation + ((skewMatrix * m_rotation) * (static_cast<float>(deltaTime) * SPEEDSCALER));

	// update and normalize the quaternion
	m_quaternion.FromMatrix3x3(m_rotation);
	m_quaternion.Normalize();

	m_bounds->Transform(m_position);
}

/*
*	\brief	Handle collision against another rigid body
*/
void CSpaghettiRigidBodySphere::HandleCollision(
	CSpaghettiRigidBody *otherRigidBody								//!< The other rigid body to compare against
	)
{
	if (!m_flags.isEnabled)
		return;

	if (m_bounds->Intersects(otherRigidBody->GetBounds()))
	{
		SAM::TVector<float, 3> direction = m_position - otherRigidBody->GetPosition();
		direction = direction.Unit();
		direction = direction * 0.5f;

		SAM::TVector<float, 3> thisVeloctiy;
		thisVeloctiy.Set(
			m_velocity.X() * direction.X(), 
			m_velocity.Y() * direction.Y(),
			m_velocity.Z() * direction.Z()
			);
		SetVelocity(thisVeloctiy);
		m_position = m_lastPosition;

		SAM::TVector<float, 3> otherVeloctiy;
		otherVeloctiy.Set(
			otherRigidBody->GetVelocity().X() * direction.X(), 
			otherRigidBody->GetVelocity().Y() * direction.Y(),
			otherRigidBody->GetVelocity().Z() * direction.Z()
			);
		otherRigidBody->SetVelocity(otherVeloctiy);
		otherRigidBody->SetPosition(otherRigidBody->GetLastPosition());
	}
}