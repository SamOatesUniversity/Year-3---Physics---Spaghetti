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
};