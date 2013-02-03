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
		const float deltaTime							//!< Delta time (The amount of time past since the last update)
	)
{
	// Nothing to update if he rigid body is static or disabled
	if (m_flags.isStatic || !m_flags.isEnabled)
		return;

	if (m_velocity.Length() < 0.001f) m_velocity.Set(0.0f, 0.0f, 0.0f);

	// Add Gravity
	SAM::TVector3 gravity = world->GetGravity();
	gravity.SetY(gravity.Y() * m_mass);
	AddForce(m_position, gravity);
	
	// Update Linear
	m_velocity = m_velocity + ((m_force / m_mass) * deltaTime);

	UpdateMatrix();
}

/*
*	\brief	Handle collision against another rigid body
*/
bool CSpaghettiRigidBodyBox::CheckCollision(
		CSpaghettiRigidBody *otherRigidBody								//!< The other rigid body to compare against
	)
{
	if (!m_flags.isEnabled)
		return false;

	if (m_bounds->Intersects(otherRigidBody->GetBounds()))
		return true;

	return false;
}