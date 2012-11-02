#pragma once

#include "../../../SAM/SAM/SAM.h"

class CSpaghettiRigidBody {
private:

	SAM::TVector<float, 3>					m_position;
	SAM::TVector<float, 3>					m_newPosition;
	SAM::TVector<float, 3>					m_velocity;	

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