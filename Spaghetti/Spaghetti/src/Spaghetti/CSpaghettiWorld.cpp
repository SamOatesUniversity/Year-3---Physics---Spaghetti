#include "CSpaghettiWorld.h"

/*
*	\brief Class constructor
*/
CSpaghettiWorld::CSpaghettiWorld()
{
	m_gravity = Ogre::Vector3(0.0f, -9.81f, 0.0f);
	m_flags.allflags = 0;
	m_collision.clear();
}

/*
*	\brief Class destructor
*/
CSpaghettiWorld::~CSpaghettiWorld()
{
	Release();
}

/*
*	\brief Class destructor
*/
void CSpaghettiWorld::Release()
{
	std::vector<CSpaghettiRigidBody*>::const_iterator begin = m_rigidBodies.begin();
	std::vector<CSpaghettiRigidBody*>::const_iterator end = m_rigidBodies.end();
	for (std::vector<CSpaghettiRigidBody*>::const_iterator bodyIt = begin; bodyIt != end; bodyIt++)
	{
		CSpaghettiRigidBody *const body = (*bodyIt);
		delete body;
	}
	m_rigidBodies.clear();

	m_gravity = Ogre::Vector3(0.0f, -9.81f, 0.0f);
	m_flags.allflags = 0;
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
Ogre::Vector3 CSpaghettiWorld::GetGravity() const
{
	return m_gravity;
}

/*
*	\brief Update all simulation objects in the world
*/
void CSpaghettiWorld::Update(
		const float deltaTime						//!< Delta time (The amount of time past since the last update)
	)
{
	m_collision.clear();

	if (m_flags.isPaused)
		return;

	RigidBodyList::const_iterator begin = m_rigidBodies.begin();
	RigidBodyList::const_iterator end = m_rigidBodies.end();
	
	// check for collisions and apply impulses
	RigidBodyList::const_iterator bodyIt = begin;
	for (bodyIt; bodyIt != end; bodyIt++)
	{
		CSpaghettiRigidBody *const body = (*bodyIt);
		if (body->IsStatic())
			continue;


		for (RigidBodyList::const_iterator otherBodyIt = begin; otherBodyIt != end; otherBodyIt++)
		{
			CSpaghettiRigidBody *const otherBody = (*otherBodyIt);
			if (body == otherBody)
				continue;

			body->HandleCollision(this, otherBody, deltaTime, m_collision);
		}
	}

	// update the velocities
	bodyIt = begin;
	for (bodyIt; bodyIt != end; bodyIt++)
	{
		CSpaghettiRigidBody *const body = (*bodyIt);
		body->UpdateVelocity(this, deltaTime);
	}
}
