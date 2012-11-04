#include "CSpaghettiWorld.h"

/*
*	\brief Class constructor
*/
CSpaghettiWorld::CSpaghettiWorld()
{
	m_gravity[0] = 0.0f;
	m_gravity[1] = -9.81f;
	m_gravity[2] = 0.0f;
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
*	\brief Update all simulation objects in the world
*/
void CSpaghettiWorld::Update(
		const unsigned long deltaTime						//!< Delta time (The amount of time past since the last update)
	)
{
	std::vector<CSpaghettiRigidBody*>::const_iterator begin = m_rigidBodies.begin();
	std::vector<CSpaghettiRigidBody*>::const_iterator end = m_rigidBodies.end();
	for (std::vector<CSpaghettiRigidBody*>::const_iterator bodyIt = begin; bodyIt != end; bodyIt++)
	{
		CSpaghettiRigidBody *const body = (*bodyIt);
		body->Update(deltaTime);
	}
}