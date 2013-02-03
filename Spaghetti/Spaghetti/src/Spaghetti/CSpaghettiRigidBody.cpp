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
	
	m_mass = 5.0f;
	m_flags.alllags = 0;
	m_flags.isEnabled = true;

	m_centerOfMass.Set(0, 0, 0);
	
	m_rotation.Identity();
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
	const float vel = velocity.Length();
	if (vel < 0.0002f && vel > -0.0002f)
	{
		m_velocity.Set(0.0f, 0.0f, 0.0f);
	}
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
	if (m_bounds != nullptr) m_bounds->Transform(m_position, m_rotation);

	CalculateInertiaBodyTensor();
	UpdateInertiaTensor();
	UpdateAngularVelocity();
}

//! 
void CSpaghettiRigidBody::AddForce(
		SAM::TVector3 position,
		SAM::TVector3 force
	)
{
	m_force = m_force + force;
	m_torque = m_torque + (m_position - position).Cross(force);
}

//! 
void CSpaghettiRigidBody::UpdateMatrix()
{
	SAM::TVector3 axis[3];
	axis[0].Set(1,0,0);
	axis[1].Set(0,1,0);
	axis[2].Set(0,0,1);

	SAM::TMatrix<float, 4, 4> matR = m_rotation.ToMatrix4x4();

	axis[0] = matR.Transform(axis[0]);
	axis[1] = matR.Transform(axis[1]);
	axis[2] = matR.Transform(axis[2]);

	SAM::TMatrix<float, 4, 4> matT;
	matT.Translate(m_position.X(), m_position.Y(), m_position.Z());

	m_matWorld = matR * matT;

	UpdateInertiaTensor();

	m_bounds->Transform(m_position, m_rotation);
}

//! 
void CSpaghettiRigidBody::UpdatePosition(
		const float deltaTime						//!< Delta time (The amount of time past since the last update)
	)
{
	m_lastPosition = m_position;
	m_position = m_position + m_velocity;

	m_force.Set(0.0f, 0.0f, 0.0f);
	m_torque.Set(0.0f, 0.0f, 0.0f);

	UpdateMatrix();
}