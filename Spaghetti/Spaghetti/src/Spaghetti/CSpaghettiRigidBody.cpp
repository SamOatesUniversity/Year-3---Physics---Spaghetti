#include "CSpaghettiRigidBody.h"

/*
*	\brief Class constructor
*/
CSpaghettiRigidBody::CSpaghettiRigidBody(
		void* renderObject										//!< A pointer to an object this rigid body will represent
	)
{
	m_renderObject = renderObject;

	m_position.Set(0.0f, 0.0f, 0.0f);
	m_lastPosition.Set(0.0f, 0.0f, 0.0f);

	m_velocity.Set(0.0f, 0.0f, 0.0f);
	m_torque.Set(0.0f, 0.0f, 0.0f);
	
	m_mass = 1.0f;
	m_flags.alllags = 0;
	m_flags.isEnabled = true;

	m_centerOfMass.Set(0, 0, 0);
	
	m_rotation.Identity();
	m_angularVelocity.Set(0, 0, 0);
	m_angularMomentum.Set(0, 0, 0);
}

/*
*	\brief Class destructor
*/
CSpaghettiRigidBody::~CSpaghettiRigidBody()
{
	m_renderObject = nullptr;
}

/*
*	\brief Update all simulation objects in the world
*/
void CSpaghettiRigidBody::SetPosition( 
		const float x,											//!< The x coordinate
		const float y,											//!< The y coordinate
		const float z											//!< The z coordinate
	)
{
	m_position.Set(x, y, z);
	m_boundingBox.Transform(m_position);
}

/*
*	\brief	Set a rigid bodies bounding box
*/
void CSpaghettiRigidBody::SetBoundingBox( 
	CSpaghettiBoundingBox &boundingBox								//!< The bounding box to set this bodies bounding box too
	)
{
	m_boundingBox = boundingBox;

	CalculateInertiaBodyTensor();
	UpdateInertiaTensor();
	UpdateAngularVelocity();
}

/*
*	\brief	Calculate the inertia based upon the bounding box
*/
void CSpaghettiRigidBody::CalculateInertiaBodyTensor()
{
	static const float oneOverTwelve = 1.0f / 12.0f;
	const float heightSquared = m_boundingBox.Height() * m_boundingBox.Height();
	const float depthSquared = m_boundingBox.Depth() * m_boundingBox.Depth();
	const float widthSquared = m_boundingBox.Width() * m_boundingBox.Width();

	m_inertiaTensor[0][0] = (oneOverTwelve * m_mass) * (heightSquared + depthSquared); m_inertiaTensor[0][1] = 0; m_inertiaTensor[0][2] = 0;
	m_inertiaTensor[1][0] = 0; m_inertiaTensor[1][1] = (oneOverTwelve * m_mass) * (widthSquared + depthSquared);  m_inertiaTensor[1][2] = 0;
	m_inertiaTensor[2][0] = 0; m_inertiaTensor[2][1] = 0; m_inertiaTensor[2][2] = (oneOverTwelve * m_mass) * (widthSquared + heightSquared);

	m_inertiaTensorBodyInverse = m_inertiaTensor.Inverse();
}

/*
*	\brief	
*/
void CSpaghettiRigidBody::UpdateInertiaTensor()
{
	m_inertiaTensorInverse = m_rotation * (m_inertiaTensorBodyInverse * m_rotation.Transpose());
}

/*
*	\brief	
*/
void CSpaghettiRigidBody::UpdateAngularVelocity()
{
	m_angularVelocity = m_inertiaTensorInverse * m_angularMomentum;
}

/*
*	\brief Update all simulation objects in the world
*/
void CSpaghettiRigidBody::Update( 
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

	m_boundingBox.Transform(m_position);
}

/*
*	\brief	Handle collision against another rigid body
*/
void CSpaghettiRigidBody::HandleCollision(
		CSpaghettiRigidBody *otherRigidBody								//!< The other rigid body to compare against
	)
{
	if (!m_flags.isEnabled)
		return;

	if (m_boundingBox.Intersects(otherRigidBody->GetBoundingBox()))
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