#pragma once

#include "../../../SAM/SAM/SAM.h"
#include "CSpaghettiRigidBody.h"
#include "CCollision.h"

#include <vector>

class CSpaghettiWorld {

friend class CSpaghettiRigidBody;

private:

	SAM::TVector<float, 3>					m_gravity;										//!< A constant force applied to all objects within this world

	std::vector<CSpaghettiRigidBody*>		m_rigidBodies;									//!< A list of rigid bodies contained within this world

	std::vector<CCollision>					m_collisions;									//!< 

public:
											//! Class constructor
											CSpaghettiWorld();

											//! Class destructor
											~CSpaghettiWorld();

											//! Add a rigid body to the world
	void									AddRigidBody(
												CSpaghettiRigidBody *rigidBody				//!< The rigid body to add to the scene
											);

											//! Gets the worlds gravity vector
	SAM::TVector<float, 3>					GetGravity() const;

											//! Update all simulation objects in the world
	void									Update(
												const float deltaTime				//!< Delta time (The amount of time past since the last update)
											);

											//! 
	void									AddCollision(
												CSpaghettiRigidBody *bodyOne,
												CSpaghettiRigidBody *bodyTwo,
												SAM::TVector3 &point,
												SAM::TVector3 &normal,
												const float penetration
											);

											//! 
	void									ApplyImpulses(
												float deltaTime
											);

											//! 
	void									AddCollisionImpulse(
												CSpaghettiRigidBody *bodyOne, 
												CSpaghettiRigidBody *bodyTwo, 
												SAM::TVector3& hitPoint, 
												SAM::TVector3& normal, 
												float deltaTime,
												SAM::TVector3 &velBodyOne,
												SAM::TVector3 &velBodyTwo,
												SAM::TVector3 &angularVelBodyOne,
												SAM::TVector3 &angularVelBodyTwo
											);
};