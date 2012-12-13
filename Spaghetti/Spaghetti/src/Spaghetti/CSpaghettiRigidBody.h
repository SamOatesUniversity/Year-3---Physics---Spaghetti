#pragma once

#include <string>
#include "../../../SAM/SAM/SAM.h"
#include "CSpaghettiWorld.h"
#include "CSpaghettiBoundingBox.h"

typedef union {
	struct {
		bool isStatic				:	1;
	};
	int alllags;
} RigidBodyFlags;

class CSpaghettiRigidBody {

friend class CSpaghettiWorld;

private:

	void									*m_renderObject;										//!< A pointer to an object this rigid body will represent

	SAM::TVector<float, 3>					m_position;												//!< 
	SAM::TVector<float, 3>					m_velocity;												//!< 
	SAM::TVector<float, 3>					m_lastPosition;											//!< 

	CSpaghettiBoundingBox					m_boundingBox;											//!< The rigid bodies bounding box

	RigidBodyFlags							m_flags;												//!< 

public:

											//! 
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

											//! Set the absolute position of the rigid body
	void									SetPosition(
												SAM::TVector<float, 3> position 
											)
											{
												m_position = m_lastPosition;
											}

											//! Get the rigid body position
	SAM::TVector<float, 3>					&GetPosition() 
											{
												return m_position;
											}

											//! Get the last position of the body
	SAM::TVector<float, 3>					&GetLastPosition()
											{
												return m_lastPosition;
											}

											//! Update the rigid body
	void									Update(
												const CSpaghettiWorld *world,
												const unsigned long deltaTime						//!< Delta time (The amount of time past since the last update)
											);

											//! Set the rigid bodies bounding box
	void									SetBoundingBox(
												CSpaghettiBoundingBox &boundingBox					//!< The bounding box to set this bodies bounding box too
											);

											//! Get the bounding box
	CSpaghettiBoundingBox					GetBoundingBox() const
											{
												return m_boundingBox;
											}

											//! Handle collision against another rigid body
	void									HandleCollision(
												CSpaghettiRigidBody *otherRigidBody					//!< The other rigid body to compare against
											);

											//! Set the velocity of the rigid body
	void									SetVelocity(
												SAM::TVector<float, 3> velocity
											)
											{
												m_velocity = velocity;
												const float vel = velocity.Length();
												if (vel < 0.002f && vel > -0.002f)
												{
													m_velocity.Set(0.0f, 0.0f, 0.0f);
												}
											}

											//! Get the current velocity of the rigid body
	SAM::TVector<float, 3>					GetVelocity() const
											{
												return m_velocity;
											}

											// Set the static flag of the rigid body
	void									SetIsStatic(
												const bool isStatic
											)
											{
												m_flags.isStatic = isStatic;
											}
};