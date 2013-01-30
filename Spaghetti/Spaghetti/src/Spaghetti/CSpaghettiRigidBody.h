#pragma once

#include <string>
#include "../../../SAM/SAM/SAM.h"
#include "CSpaghettiWorld.h"
#include "CSpaghettiBounds.h"

struct RigidBodyType {
	enum Enum {
		Box,
		Sphere,
		Noof
	};
};

typedef union {
	struct {
		bool isStatic				:	1;
		bool isEnabled				:	1;
	};
	int alllags;
} RigidBodyFlags;

class CSpaghettiRigidBody {

friend class CSpaghettiWorld;

protected:

	void									*m_renderObject;										//!< A pointer to an object this rigid body will represent
	CSpaghettiBounds						*m_bounds;												//!< The rigid bodies bounding box
		
	RigidBodyFlags							m_flags;												//!<

	float									m_mass;													//!<
	SAM::TMatrix<float, 3, 3>				m_inertiaTensor;										//!< 
	SAM::TMatrix<float, 3, 3>				m_inertiaTensorBodyInverse;								//!< 
	SAM::TMatrix<float, 3, 3>				m_inertiaTensorInverse;									//!< 

	SAM::TVector<float, 3>					m_position;												//!< 
	SAM::TVector<float, 3>					m_lastPosition;											//!< 

	SAM::TMatrix<float, 3, 3>				m_rotation;												//!< 
	SAM::TQuaternion						m_quaternion;											//!< 

	SAM::TVector<float, 3>					m_centerOfMass;											//!< 

	SAM::TVector<float, 3>					m_velocity;												//!< 
	SAM::TVector<float, 3>					m_angularMomentum;										//!< 

	SAM::TVector<float, 3>					m_angularVelocity;										//!< The direction gives the direction of the axis about which the body is spinning. The magnitude tells how fast the body is spinning

	SAM::TVector<float, 3>					m_torque;												//!< 
	SAM::TVector<float, 3>					m_force;												//!< 

protected:
											//! Calculate the inertia tensor based upon the bounding box
	virtual void							CalculateInertiaBodyTensor() = 0;

											//! 
	void									UpdateInertiaTensor();

											//! 
	void									UpdateAngularVelocity();

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

											//! Get the orientation of the rigid body
	SAM::TQuaternion						&GetOrientation()
											{
												return m_quaternion;
											}

											//! Set the velocity of the rigid body
	void									SetVelocity(
												SAM::TVector<float, 3> velocity
											);

											//! Get the current velocity of the rigid body
	SAM::TVector<float, 3>					GetVelocity() const
											{
												return m_velocity;
											}
	
											//! Set the rigid bodies bounding box
	void									SetBounds(
												CSpaghettiBounds *boundingBox					//!< The bounding box to set this bodies bounding box too
											);

											//! Get the bounding box
	CSpaghettiBounds						*GetBounds() const
											{
												return m_bounds;
											}

											// Set the static flag of the rigid body
	void									SetIsStatic(
												const bool isStatic
											)
											{
												m_flags.isStatic = isStatic;
											}

											//! Update the rigid body
	virtual void							Update(
												const CSpaghettiWorld *world,
												const unsigned long deltaTime						//!< Delta time (The amount of time past since the last update)
											) = 0;

											//! Handle collision against another rigid body
	virtual void							HandleCollision(
												CSpaghettiRigidBody *otherRigidBody					//!< The other rigid body to compare against
											) = 0;
};