#pragma once

#include "../SAM/SAM.h"

class CSpaghettiRigidBody {
private:

	SAM::Vector<3, float>					m_position;
	SAM::Vector<3, float>					m_newPosition;
	SAM::Vector<3, float>					m_velocity;	

public:
											//! Class constructor
											CSpaghettiRigidBody();

											//! Class destructor
											~CSpaghettiRigidBody();

											//! Initialize the rigid body
	void									Initialize();

											//! Update the rigid body
	void									Update();

};