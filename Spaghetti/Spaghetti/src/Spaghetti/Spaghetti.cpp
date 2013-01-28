#include "Spaghetti.h"

/*
*	\brief Class constructor
*/
CSpaghetti::CSpaghetti()
{

}

/*
*	\brief Class destructor
*/
CSpaghetti::~CSpaghetti()
{
	std::vector<CSpaghettiWorld*>::const_iterator begin = m_worlds.begin();
	std::vector<CSpaghettiWorld*>::const_iterator end = m_worlds.end();
	for (std::vector<CSpaghettiWorld*>::const_iterator worldIt = begin; worldIt != end; worldIt++)
	{
		CSpaghettiWorld *const world = (*worldIt);
		delete world;
	}
	m_worlds.clear();
}

/*
*	\brief Create a simulation world and add it to our list
*/
CSpaghettiWorld *CSpaghetti::CreateWorld()
{
	CSpaghettiWorld *newWorld = new CSpaghettiWorld();
	m_worlds.push_back(newWorld);

	return newWorld;
}

/*
*	\brief Creates a rigid body
*/
CSpaghettiRigidBody *CSpaghetti::CreateRigidBody(
		void *renderObject,									//!< A pointer to an object the new igid body will represent
		CSpaghettiWorld *world								//!< The world the rigid body should be long too
	)
{
	CSpaghettiRigidBody *const newRigidBody = new CSpaghettiRigidBodyBox(renderObject);
	if (world != nullptr) {
		world->AddRigidBody(newRigidBody);
	}

	return newRigidBody;
}

