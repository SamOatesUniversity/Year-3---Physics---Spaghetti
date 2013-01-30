#pragma once

#include "CSpaghettiWorld.h"
#include "CSpaghettiRigidBodyBox.h"
#include "CSpaghettiRigidBodySphere.h"
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
										void *renderObject,							//!< A pointer to an object the new rigid body will represent
										CSpaghettiWorld *world,						//!< The world the rigid body should be long too
										RigidBodyType::Enum bodyType				//!< The type of rigidbody to create
									);

};