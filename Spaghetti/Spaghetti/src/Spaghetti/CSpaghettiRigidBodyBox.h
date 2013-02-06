#pragma once

#include "CSpaghettiRigidBody.h"
#include "bounds\CSpaghettiBoundsBox.h"

class CSpaghettiRigidBodyBox : public CSpaghettiRigidBody {

friend class CSpaghettiWorld;

private:
											//! Calculate the inertia tensor based upon the bounding box
	virtual void							CalculateInertiaBodyTensor();

public:

											//! Class constructor
											CSpaghettiRigidBodyBox(
												void *renderObject									//!< A pointer to an object this rigid body will represent
											);

											//! Class destructor
											~CSpaghettiRigidBodyBox();

											//! Update the rigid body
	virtual void							UpdateVelocity(
												const CSpaghettiWorld *world,
												const float deltaTime								//!< Delta time (The amount of time past since the last update)
											);

											//! Update the rigid bodies position
	virtual void							UpdatePosition(
												CSpaghettiWorld	*world,								//!< The world we are moving in
												const float deltaTime								//!< Delta time (The amount of time past since the last update)
											);

											//! Handle collision against another rigid body
	virtual void							HandleCollision(
												CSpaghettiWorld	*world,								//!< The world we are moving in
												CSpaghettiRigidBody *otherRigidBody					//!< The other rigid body to compare against
											);

	SAM::TVector3							CalculateCollisionPoint(
												CSpaghettiRigidBody *otherRigidBody					//!< The other rigid body to compare against
											);

};