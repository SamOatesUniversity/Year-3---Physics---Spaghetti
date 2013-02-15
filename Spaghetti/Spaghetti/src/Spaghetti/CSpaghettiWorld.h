#pragma once

#include "Ogre/Ogre.h"
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

	Ogre::Vector3							m_gravity;										//!< A constant force applied to all objects within this world
	RigidBodyList							m_rigidBodies;									//!< A list of rigid bodies contained within this world
	WorldFlags								m_flags;										//!< World related flags

	std::vector<CCollision>					m_collision;

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
	Ogre::Vector3							GetGravity() const;

											//! Set gravity of the world
	void									SetGravity(
												Ogre::Vector3 gravity								//!< The gravity to set the worlds gravity too
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
	
											//! Release all bodies in the world and reset values
	void									Release();

											//!
	std::vector<CCollision>					GetLastUpdateCollisions()
											{
												return m_collision;
											}
};