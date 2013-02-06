#pragma once

#include "../../../SAM/SAM/SAM.h"
#include "CSpaghettiRigidBody.h"
#include "CCollision.h"

#include <vector>

typedef union {
	struct {
		bool isPaused				:	1;
	};
	int allflags;
} WorldFlags;

typedef	std::vector<CSpaghettiRigidBody*>	RigidBodyList;

class CSpaghettiWorld {

friend class CSpaghettiRigidBody;

private:

	SAM::TVector<float, 3>					m_gravity;										//!< A constant force applied to all objects within this world
	RigidBodyList							m_rigidBodies;									//!< A list of rigid bodies contained within this world
	WorldFlags								m_flags;										//!< World related flags

public:
											//! Class constructor
											CSpaghettiWorld();

											//! Class destructor
											~CSpaghettiWorld();

											//! Add a rigid body to the world
	void									AddRigidBody(
												CSpaghettiRigidBody *rigidBody						//!< The rigid body to add to the scene
											);

											//! Gets the worlds gravity vector
	SAM::TVector<float, 3>					GetGravity() const;

											//! Set gravity of the world
	void									SetGravity(
												SAM::TVector<float, 3> gravity						//!< The gravity to set the worlds gravity too
											)
											{
												m_gravity = gravity;
											}

											//! Get whether the world is paused or not
	const bool								IsPaused() const
											{
												return m_flags.isPaused;
											}

											//! Set whether the world is paused or not
	void									SetPaused(
												const bool pause									//!< True to pause the world, false otherwise
											)
											{
												m_flags.isPaused = pause;
											}

											//! Update all simulation objects in the world
	void									Update(
												const float deltaTime								//!< Delta time (The amount of time past since the last update)
											);

};