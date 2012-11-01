#pragma once

#include "CSpaghettiRigidBody.h"

class CSpaghettiWorld {
private:

	float									m_gravity[3];							//!< A constant force applied to all objects within this world

	std::vector<CSpaghettiRigidBody*>		m_rigidBodies;							//!< A list of rigid bodies contained within this world

public:
											//! Class constructor
											CSpaghettiWorld();

											//! Class destructor
											~CSpaghettiWorld();

};