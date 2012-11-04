#pragma once

#include <string>
#include "../../../SAM/SAM/SAM.h"
#include "CSpaghettiWorld.h"

class CSpaghettiRigidBody {

friend class CSpaghettiWorld;

private:

	std::string								m_id;													//!< The id of the rigid body

	SAM::TVector<float, 3>					m_position;												//!< 
	SAM::TVector<float, 3>					m_newPosition;											//!< 
	SAM::TVector<float, 3>					m_velocity;												//!< 

public:
											//! Class constructor
											CSpaghettiRigidBody(
												const std::string	id								//!< The id of the rigid body	
											);

											//! Class destructor
											~CSpaghettiRigidBody();

											//! Initialize the rigid body
	void									Initialize();

											//! Set the absolute position of the rigid body
	void									SetPosition(
												const float x,										//!< The x coordinate 
												const float y,										//!< The y coordinate 
												const float z										//!< The z coordinate 
											);

											//! Get the rigid body position
	SAM::TVector<float, 3>					&GetPosition() 
											{
												return m_position;
											}

											//! Update the rigid body
	void									Update(
												const CSpaghettiWorld *world,
												const unsigned long deltaTime						//!< Delta time (The amount of time past since the last update)
											);

};