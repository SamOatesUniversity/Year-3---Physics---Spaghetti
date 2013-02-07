#include "CSpaghettiRigidBody.h"

/*
*	\brief Class constructor
*/
CSpaghettiRigidBody::CSpaghettiRigidBody(
		void* renderObject										//!< A pointer to an object this rigid body will represent
	) :
	m_bounds(nullptr)
{
	m_renderObject = renderObject;

	m_position.Set(0.0f, 0.0f, 0.0f);
	m_lastPosition.Set(0.0f, 0.0f, 0.0f);

	m_velocity.Set(0.0f, 0.0f, 0.0f);
	m_torque.Set(0.0f, 0.0f, 0.0f);
	
	m_mass = 1.0f;
	m_flags.allflags = 0;
	m_flags.isEnabled = true;

	m_angularVelocity.Set(0, 0, 0);
	m_angularMomentum.Set(0.0f, 0.0f, 0.0f);
}

/*
*	\brief Class destructor
*/
CSpaghettiRigidBody::~CSpaghettiRigidBody()
{
	m_renderObject = nullptr;
}

/*
*	\brief Update all simulation objects in the world
*/
void CSpaghettiRigidBody::SetPosition( 
		const float x,											//!< The x coordinate
		const float y,											//!< The y coordinate
		const float z											//!< The z coordinate
	)
{
	m_position.Set(x, y, z);
	if (m_bounds != nullptr) m_bounds->Transform(m_position, m_rotation);
}

/*
*	\brief Set the velocity of the rigid body
*/
void CSpaghettiRigidBody::SetVelocity(
		SAM::TVector<float, 3> velocity
	)
{
	if (m_flags.isStatic)
		return;

	m_velocity = velocity;

	if (m_velocity.LengthSquared() < 1.0f) m_velocity.Zero();
}

/*
*	\brief	
*/
void CSpaghettiRigidBody::UpdateInertiaTensor()
{
	m_inertiaTensorInverse = m_rotation * (m_inertiaTensorBodyInverse * m_rotation.Transpose());
}

/*
*	\brief	
*/
void CSpaghettiRigidBody::UpdateAngularVelocity()
{
	m_angularVelocity = m_inertiaTensorInverse * m_angularMomentum;
}

/*
*	\brief	Set a rigid bodies bounding box
*/
void CSpaghettiRigidBody::SetBounds( 
	CSpaghettiBounds *boundingBox								//!< The bounding box to set this bodies bounding box too
	)
{
	m_bounds = boundingBox;
	if (m_bounds != nullptr) 
	{
		m_bounds->SetBody(this);
		m_bounds->Transform(m_position, m_rotation);
	}

	CalculateInertiaBodyTensor();
	UpdateInertiaTensor();
	UpdateAngularVelocity();
}

void CSpaghettiRigidBody::AddForceAtPoint( 
		SAM::TVector3 &force, 
		SAM::TVector3 &point 
	)
{
	m_force = m_force + force;
	AddTorqueAtPoint(force, point);	
}

void CSpaghettiRigidBody::AddTorqueAtPoint(
		SAM::TVector3 &force,
		SAM::TVector3 &point
	)
{
	// Convert to coordinates relative to center of mass.
	SAM::TVector3 pt = point;
	pt = pt - m_position;

	m_torque = m_torque + (pt.Cross(force));
}