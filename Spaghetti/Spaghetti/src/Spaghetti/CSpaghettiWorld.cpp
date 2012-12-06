#include "CSpaghettiWorld.h"

/*
*	\brief Class constructor
*/
CSpaghettiWorld::CSpaghettiWorld()
{
	m_gravity.Set(0.0f, -0.00000981f, 0.0f);
}

/*
*	\brief Class destructor
*/
CSpaghettiWorld::~CSpaghettiWorld()
{
	std::vector<CSpaghettiRigidBody*>::const_iterator begin = m_rigidBodies.begin();
	std::vector<CSpaghettiRigidBody*>::const_iterator end = m_rigidBodies.end();
	for (std::vector<CSpaghettiRigidBody*>::const_iterator bodyIt = begin; bodyIt != end; bodyIt++)
	{
		CSpaghettiRigidBody *const body = (*bodyIt);
		delete body;
	}
	m_rigidBodies.clear();
}

/*
*	\brief Add a rigid body to the world
*/
void CSpaghettiWorld::AddRigidBody( 
	CSpaghettiRigidBody *rigidBody							//!< The rigid body to add to the scene
	)
{
	m_rigidBodies.push_back(rigidBody);
}

/*
*	\brief Get the worlds gravity vector
*/
SAM::TVector<float, 3> CSpaghettiWorld::GetGravity() const
{
	return m_gravity;
}

/*
*	\brief Update all simulation objects in the world
*/
void CSpaghettiWorld::Update(
		const unsigned long deltaTime						//!< Delta time (The amount of time past since the last update)
	)
{
	// Update the rigid bodies
	std::vector<CSpaghettiRigidBody*>::const_iterator begin = m_rigidBodies.begin();
	std::vector<CSpaghettiRigidBody*>::const_iterator end = m_rigidBodies.end();
	for (std::vector<CSpaghettiRigidBody*>::const_iterator bodyIt = begin; bodyIt != end; bodyIt++)
	{
		CSpaghettiRigidBody *const body = (*bodyIt);
		body->Update(this, deltaTime);
	}

	// Check collisions
	for (std::vector<CSpaghettiRigidBody*>::const_iterator bodyIt = begin; bodyIt != end; bodyIt++)
	{
		CSpaghettiRigidBody *const body = (*bodyIt);
		for (std::vector<CSpaghettiRigidBody*>::const_iterator otherBodyIt = bodyIt; otherBodyIt != end; otherBodyIt++)
		{
			CSpaghettiRigidBody *const otherBody = (*otherBodyIt);
			if (body == otherBody)
				continue;

			body->HandleCollision(otherBody);
		}		
	}
}
