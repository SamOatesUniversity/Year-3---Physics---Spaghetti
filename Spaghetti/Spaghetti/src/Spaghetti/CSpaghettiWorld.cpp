#include "CSpaghettiWorld.h"

/*
*	\brief Class constructor
*/
CSpaghettiWorld::CSpaghettiWorld()
{
	m_gravity.Set(0.0f, -9.81f, 0.0f);
	m_flags.allflags = 0;
}

/*
*	\brief Class destructor
*/
CSpaghettiWorld::~CSpaghettiWorld()
{
	std::vector<CSpaghettiRigidBody*>::const_iterator begin = m_rigidBodies.begin();
	std::vector<CSpaghettiRigidBody*>::const_iterator end = m_rigidBodies.end();
	for (std::vector<CSpaghettiRigidBody*>::const_iterator bodyIt = begin; bodyIt != end; bodyIt++)
	{
		CSpaghettiRigidBody *const body = (*bodyIt);
		delete body;
	}
	m_rigidBodies.clear();
}

/*
*	\brief Add a rigid body to the world
*/
void CSpaghettiWorld::AddRigidBody( 
	CSpaghettiRigidBody *rigidBody							//!< The rigid body to add to the scene
	)
{
	m_rigidBodies.push_back(rigidBody);
}

/*
*	\brief Get the worlds gravity vector
*/
SAM::TVector<float, 3> CSpaghettiWorld::GetGravity() const
{
	return m_gravity;
}

/*
*	\brief Update all simulation objects in the world
*/
void CSpaghettiWorld::Update(
		const float deltaTime						//!< Delta time (The amount of time past since the last update)
	)
{
	if (m_flags.isPaused)
		return;

	RigidBodyList::const_iterator begin = m_rigidBodies.begin();
	RigidBodyList::const_iterator end = m_rigidBodies.end();

	// Update the positions
	RigidBodyList::const_iterator bodyIt = begin;
	for (bodyIt; bodyIt != end; bodyIt++)
	{
		CSpaghettiRigidBody *const body = (*bodyIt);
		if (body->IsStatic())
			continue;

		body->UpdatePosition(this, deltaTime);
	}

	// check for collisions and apply impulses
	bodyIt = begin;
	for (bodyIt; bodyIt != end; bodyIt++)
	{
		CSpaghettiRigidBody *const body = (*bodyIt);
		for (RigidBodyList::const_iterator otherBodyIt = bodyIt; otherBodyIt != end; otherBodyIt++)
		{
			CSpaghettiRigidBody *const otherBody = (*otherBodyIt);
			if (body == otherBody)
				continue;

			body->HandleCollision(this, otherBody);
		}
	}

	// update the velocities
	bodyIt = begin;
	for (bodyIt; bodyIt != end; bodyIt++)
	{
		CSpaghettiRigidBody *const body = (*bodyIt);
		if (body->IsStatic())
			continue;

		body->UpdateVelocity(this, deltaTime);
	}

}

//void CSpaghettiWorld::AddCollisionImpulse( 
//		CSpaghettiRigidBody *bodyOne, 
//		CSpaghettiRigidBody *bodyTwo, 
//		SAM::TVector3& hitPoint, 
//		SAM::TVector3& normal, 
//		float deltaTime, 
//		SAM::TVector3 &velBodyOne,
//		SAM::TVector3 &velBodyTwo,
//		SAM::TVector3 &angularVelBodyOne,
//		SAM::TVector3 &angularVelBodyTwo
//	)
//{
//	
//	static const float e = -0.5f;
//
//	SAM::TVector3 relativeVelocity = bodyOne->GetVelocity() - bodyTwo->GetVelocity();
//
//	// linear
//	{
//		const float inverseMassBodyOne = 1.0f / bodyOne->GetMass();
//		const float inverseMassBodyTwo = 1.0f / bodyTwo->GetMass();
//		const float sumOfInverseMass = inverseMassBodyOne + inverseMassBodyTwo;
//
//		const float jLinear = (-((1.0f + e) * relativeVelocity.Dot(normal))) / sumOfInverseMass;
//		velBodyOne = velBodyOne + ((normal * jLinear) * inverseMassBodyOne);
//		velBodyTwo = velBodyTwo + ((normal * -jLinear) * inverseMassBodyTwo);
//	}
//
//	// angular
//	{
//		SAM::TVector3 r1 = hitPoint - bodyOne->GetPosition();
//		SAM::TVector3 r2 = hitPoint - bodyTwo->GetPosition();
//
//		SAM::TVector3 ac = (bodyOne->GetInverseInertia() * r1.Cross(normal)).Cross(r1);
//		SAM::TVector3 bc = (bodyTwo->GetInverseInertia() * r2.Cross(normal)).Cross(r2);
//		const float nDc = normal.Dot(ac) + normal.Dot(bc);
//		const float jAngular = -(((1.0f + e) * relativeVelocity.Dot(normal)) * nDc);
//
//		angularVelBodyOne = angularVelBodyOne + (bodyOne->GetInverseInertia() * r1.Cross(normal * jAngular));
//		angularVelBodyTwo = angularVelBodyTwo + (bodyTwo->GetInverseInertia() * r2.Cross(normal * -jAngular));
//	}
//
//}
