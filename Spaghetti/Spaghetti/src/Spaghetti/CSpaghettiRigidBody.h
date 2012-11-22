#pragma once

#include <string>
#include "../../../SAM/SAM/SAM.h"
#include "CSpaghettiWorld.h"

class CSpaghettiRigidBody {

friend class CSpaghettiWorld;

private:

	void									*m_renderObject;										//!< A pointer to an object this rigid body will represent

	SAM::TVector<float, 3>					m_position;												//!< 
	SAM::TVector<float, 3>					m_newPosition;											//!< 
	SAM::TVector<float, 3>					m_velocity;												//!< 

public:

	void									*GetRenderObject() const
											{
												return m_renderObject;
											}

public:
											//! Class constructor
											CSpaghettiRigidBody(
												void *renderObject									//!< A pointer to an object this rigid body will represent
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