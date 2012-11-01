#pragma once

#include "CSpaghettiWorld.h"
#include <vector>

class CSpaghetti {
private:

	std::vector<CSpaghettiWorld*>	m_worlds;							//!< A list of allcreated worlds within our physics simulation

public:
									//! Class constructor
									CSpaghetti();

									//! Class destructor
									~CSpaghetti();

									//! Create a simulation world
	CSpaghettiWorld					*CreateWorld();

};