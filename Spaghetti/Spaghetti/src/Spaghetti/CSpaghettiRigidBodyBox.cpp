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

	m_velocity = m_velocity + world->GetGravity();
	m_angularMomentum = m_angularMomentum;

	UpdateInertiaTensor();
	UpdateAngularVelocity();
}

/*
*	\brief	Update the rigid bodies position
*/
void CSpaghettiRigidBodyBox::UpdatePosition(
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
void CSpaghettiRigidBodyBox::HandleCollision(
		CSpaghettiWorld	*world,											//!< The world we are moving in
		CSpaghettiRigidBody *otherRigidBody								//!< The other rigid body to compare against
	)
{
	if (!m_bounds->Intersects(otherRigidBody->GetBounds()))
		return;

	SetPosition(GetLastPosition());
	otherRigidBody->SetPosition(otherRigidBody->GetLastPosition());

	static const float e = -0.5f;
	SAM::TVector3 relativeVelocity = GetVelocity() - otherRigidBody->GetVelocity();
	
	// linear
	{
		SAM::TVector3 normal;
		normal.SetY(1.0f);

		const float inverseMassBodyOne = 1.0f / GetMass();
		const float inverseMassBodyTwo = 1.0f / otherRigidBody->GetMass();
		const float sumOfInverseMass = inverseMassBodyOne + inverseMassBodyTwo;
	
		const float jLinear = (-((1.0f + e) * relativeVelocity.Dot(normal))) / sumOfInverseMass;
		SAM::TVector3 newVelocity = ((normal * jLinear) * inverseMassBodyOne);
		SAM::TVector3 otherNewVelocity = ((normal * -jLinear) * inverseMassBodyTwo);

		SetVelocity(newVelocity);
		otherRigidBody->SetVelocity(otherNewVelocity);
	}	
}