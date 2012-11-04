#include "CSpaghettiRigidBody.h"

/*
*	\brief Class constructor
*/
CSpaghettiRigidBody::CSpaghettiRigidBody(
		const std::string	id									//!< The id of the rigid body	
	)
{
	m_id = id;

	m_position.Set(0.0f, 0.0f, 0.0f);
	m_newPosition.Set(0.0f, 0.0f, 0.0f);
	m_velocity.Set(0.0f, 0.0f, 0.0f);
}

/*
*	\brief Class destructor
*/
CSpaghettiRigidBody::~CSpaghettiRigidBody()
{

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
	m_newPosition.Set(x, y, z);
}

/*
*	\brief Update all simulation objects in the world
*/
void CSpaghettiRigidBody::Update( 
		const CSpaghettiWorld *world,
		const unsigned long deltaTime							//!< Delta time (The amount of time past since the last update)
	)
{
	SAM::TVector<float, 3> velocity = m_velocity + world->GetGravity() * (static_cast<float>(deltaTime) * 0.001f);
	SAM::TVector<float, 3> position = m_position + (m_velocity * (static_cast<float>(deltaTime) * 0.001f));

	if (position.Y() < 2.2f) 
	{
		m_velocity.Set(0.0f, 0.0f, 0.0f);
		return;
	}

	//if (m_velocity.Length() > 0.05f || (abs(m_position.Y())) > 0.05)
	//{   
	//	float drag = 0.98f;
	//	velocity = m_velocity * drag + world->GetGravity() * (static_cast<float>(deltaTime) * 0.001f);
	//	position = m_position + m_velocity * (static_cast<float>(deltaTime) * 0.001f);
	//	if (position.Y() < 2.0f)
	//	{
	//		velocity.SetY(velocity.Y() * -1.0f);
	//		position.SetY(2.0f);
	//	}
	//}

	m_velocity = velocity;
	m_position = position;
	m_newPosition = position;  
}