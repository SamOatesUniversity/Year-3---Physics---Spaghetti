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

	m_position = Ogre::Vector3::ZERO;
	m_lastPosition = Ogre::Vector3::ZERO;

	m_velocity = Ogre::Vector3::ZERO;
	m_force = Ogre::Vector3::ZERO;
	m_torque = Ogre::Vector3::ZERO;
	
	m_mass = 1.0f;
	m_flags.allflags = 0;
	m_flags.isEnabled = true;

	m_angularVelocity = Ogre::Vector3::ZERO;
	m_angularMomentum = Ogre::Vector3::ZERO;

	m_rotation = Ogre::Matrix3::IDENTITY;
}

/*
*	\brief Class destructor
*/
CSpaghettiRigidBody::~CSpaghettiRigidBody()
{
	m_renderObject = nullptr;

	if (m_bounds != nullptr)
	{
		delete m_bounds;
		m_bounds = nullptr;
	}
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
	m_position = Ogre::Vector3(x, y, z);
	if (m_bounds != nullptr)
		m_bounds->Transform(m_position, m_rotation);
}

/*
*	\brief Set the velocity of the rigid body
*/
void CSpaghettiRigidBody::SetVelocity(
		Ogre::Vector3 velocity
	)
{
	if (m_flags.isStatic)
		return;

	m_velocity = velocity;

	if (m_velocity.squaredLength() < 1.0f) 
		m_velocity = Ogre::Vector3::ZERO;
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
		Ogre::Vector3 &force, 
		Ogre::Vector3 &point 
	)
{
	m_force = m_force + force;
	AddTorqueAtPoint(force, point);	
}

void CSpaghettiRigidBody::AddTorqueAtPoint(
		Ogre::Vector3 &force,
		Ogre::Vector3 &point
	)
{
	// Convert to coordinates relative to center of mass.
	Ogre::Vector3 pt = point;
	pt = m_position - pt;

	m_angularVelocity = m_angularVelocity + (pt.crossProduct(force) * m_inertiaTensorInverse);
}

void CSpaghettiRigidBody::SetMass( const float mass )
{
	m_mass = mass;
}

void CSpaghettiRigidBody::SetRotation( Ogre::Vector3 rotation )
{
	Ogre::Radian x(rotation.x * 0.01745);
	Ogre::Radian y(rotation.y * 0.01745);
	Ogre::Radian z(rotation.z * 0.01745);

	m_rotation.FromEulerAnglesXYZ(x, y, z);
	m_quaternion.FromRotationMatrix(m_rotation);

	if (m_bounds != nullptr)
		m_bounds->Transform(m_position, m_rotation);
}

void CSpaghettiRigidBody::HandleCollision( 
		CSpaghettiWorld *world,					/*!< The world we are moving in */ 
		CSpaghettiRigidBody *otherRigidBody,	/*!< The other rigid body to compare against */ 
		const float deltaTime
	)
{
	std::vector<CCollision> collisions;
	if (!m_bounds->Intersects(otherRigidBody->GetBounds(), collisions))
		return;

	SetPosition(GetLastPosition());
	otherRigidBody->SetPosition(otherRigidBody->GetLastPosition());

	// no collisions stored, but there was a bound overlap
	if (collisions.empty())
		return;

	static const float restitution = 0.5f;
	float sumOfMass = 0.0f;
	Ogre::Vector3 relativeVeclocity = Ogre::Vector3::ZERO;

	const float inverseMassBodyOne = 1.0f / GetMass();
	const float inverseMassBodyTwo = 1.0f / otherRigidBody->GetMass();

	if (otherRigidBody->IsStatic())
	{
		sumOfMass = inverseMassBodyOne;
		relativeVeclocity = GetVelocity();
	}
	else
	{
		sumOfMass = inverseMassBodyOne + inverseMassBodyTwo;
		relativeVeclocity = GetVelocity() - otherRigidBody->GetVelocity();
	}

	unsigned int noofCollision = collisions.size();
	for (unsigned int collisionIndex = 0; collisionIndex < noofCollision; ++collisionIndex)
	{
		Ogre::Vector3 collisionNormal = collisions[collisionIndex].collisionNormal;
		Ogre::Vector3 collisionPoint = collisions[collisionIndex].collisionPoint;

		float rvDn = relativeVeclocity.dotProduct(collisionNormal);
		Ogre::Vector3 impulseLinear = (collisionNormal * -(1 + restitution) * rvDn) / sumOfMass;

		//Ogre::Vector3 angularCollision = GetVelocity() * inverseMassBodyOne;

		SetVelocity(GetVelocity() + (impulseLinear * inverseMassBodyOne));
		//SetPosition(GetPosition() + ((GetVelocity() + impulseLinear.crossProduct(collisionNormal) / deltaTime)));

		//AddTorqueAtPoint(angularCollision, collisionPoint);

		if (!otherRigidBody->IsStatic())
		{
			otherRigidBody->SetVelocity(otherRigidBody->GetVelocity() - (impulseLinear * inverseMassBodyTwo));
			//otherRigidBody->SetPosition(otherRigidBody->GetPosition() - ((otherRigidBody->GetVelocity() + impulseLinear.crossProduct(collisionNormal)) * deltaTime));

			//otherRigidBody->AddTorqueAtPoint(-angularCollision, collisionPoint);
		}
	}

}
