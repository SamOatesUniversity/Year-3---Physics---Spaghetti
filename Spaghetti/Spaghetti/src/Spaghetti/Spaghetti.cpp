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

