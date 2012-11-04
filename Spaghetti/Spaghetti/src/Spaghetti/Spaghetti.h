#pragma once

#include "CSpaghettiWorld.h"
#include <vector>

class CSpaghetti {
private:

	std::vector<CSpaghettiWorld*>	m_worlds;										//!< A list of allcreated worlds within our physics simulation

public:
									//! Class constructor
									CSpaghetti();

									//! Class destructor
									~CSpaghetti();

									//! Create a simulation world
	CSpaghettiWorld					*CreateWorld();

									//! Creates a rigid body
	CSpaghettiRigidBody				*CreateRigidBody(
										std::string id,								//!< A string id representing the graphical instance of this rigid body
										CSpaghettiWorld *world						//!< The world the rigid body should be long too
									);

};