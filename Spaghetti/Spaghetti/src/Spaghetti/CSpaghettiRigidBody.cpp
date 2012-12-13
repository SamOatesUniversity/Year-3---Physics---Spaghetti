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

	m_flags.alllags = 0;
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
*	\brief Update all simulation objects in the world
*/
void CSpaghettiRigidBody::Update( 
		const CSpaghettiWorld *world,
		const unsigned long deltaTime							//!< Delta time (The amount of time past since the last update)
	)
{
	// Nothing to update if he rigidbody is static
	if (m_flags.isStatic)
		return;

	m_lastPosition = m_position;

	SAM::TVector<float, 3> velocity = m_velocity + world->GetGravity() * (static_cast<float>(deltaTime) * 0.001f);
	SAM::TVector<float, 3> position = m_position + (m_velocity * (static_cast<float>(deltaTime) * 0.001f));

	m_velocity = velocity;
	m_position = position;

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
}

/*
*	\brief	Handle collision against another rigid body
*/
void CSpaghettiRigidBody::HandleCollision(
		CSpaghettiRigidBody *otherRigidBody								//!< The other rigid body to compare against
	)
{
	if (m_boundingBox.Intersects(otherRigidBody->GetBoundingBox()))
	{
		// TODO: need to get the normal of the collision
		// TODO: create materials, that can be applied to rigid bodies spcifying, friction, bounce and mass

		static const float FRICTION = 0.5f;

		SAM::TVector<float, 3> direction = m_position - otherRigidBody->GetPosition();
		direction = direction.Unit();

		SAM::TVector<float, 3> thisVeloctiy;
		thisVeloctiy.Set(
			direction.X() == 0 ? m_velocity.X() : m_velocity.X() * -(direction.X() * FRICTION), 
			direction.Y() == 0 ? m_velocity.Y() : m_velocity.Y() * -(direction.Y() * FRICTION), 
			direction.Z() == 0 ? m_velocity.Z() : m_velocity.Z() * -(direction.Z() * FRICTION)
		);
		SetVelocity(thisVeloctiy);
		m_position = m_lastPosition;

		SAM::TVector<float, 3> otherVeloctiy;
		otherVeloctiy.Set(
			direction.X() == 0 ? otherRigidBody->GetVelocity().X() : otherRigidBody->GetVelocity().X() * -(direction.X() * FRICTION), 
			direction.Y() == 0 ? otherRigidBody->GetVelocity().Y() : otherRigidBody->GetVelocity().Y() * -(direction.Y() * FRICTION), 
			direction.Z() == 0 ? otherRigidBody->GetVelocity().Z() : otherRigidBody->GetVelocity().Z() * -(direction.Z() * FRICTION)
		);
		otherRigidBody->SetVelocity(otherVeloctiy);
		otherRigidBody->SetPosition(otherRigidBody->GetLastPosition());
	}
}