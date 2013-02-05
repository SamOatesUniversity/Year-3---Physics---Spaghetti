#include "CSpaghettiWorld.h"

/*
*	\brief Class constructor
*/
CSpaghettiWorld::CSpaghettiWorld()
{
	m_gravity.Set(0.0f, -9.81f, 0.0f);
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
	// collision detection
	m_collisions.clear();

	std::vector<CSpaghettiRigidBody*>::const_iterator begin = m_rigidBodies.begin();
	std::vector<CSpaghettiRigidBody*>::const_iterator end = m_rigidBodies.end();

	for (std::vector<CSpaghettiRigidBody*>::const_iterator bodyIt = begin; bodyIt != end; bodyIt++)
	{
		for (std::vector<CSpaghettiRigidBody*>::const_iterator otherBodyIt = bodyIt; otherBodyIt != end; otherBodyIt++)
		{
			if (otherBodyIt == bodyIt)
				continue;

			 const bool collision = (*bodyIt)->CheckCollision(*otherBodyIt);
			 if (collision)
			 {
				 std::vector<SAM::TVector3> hitPoints = (*bodyIt)->GetBounds()->GetHitPoints();
				 const int noofCollisionPoints = hitPoints.size();

				 for (int collisionPointIndex = 0; collisionPointIndex < noofCollisionPoints; ++collisionPointIndex)
					AddCollision(*bodyIt, *otherBodyIt, hitPoints[collisionPointIndex], (*bodyIt)->GetBounds()->GetHitNormal(), (*bodyIt)->GetBounds()->GetPenetration());
			 }
		}
	}

	// Update forces
	for (std::vector<CSpaghettiRigidBody*>::const_iterator bodyIt = begin; bodyIt != end; bodyIt++)
	{
		(*bodyIt)->UpdateVelocity(this, deltaTime);
	}

	ApplyImpulses(deltaTime);

	// Update positions
	for (std::vector<CSpaghettiRigidBody*>::const_iterator bodyIt = begin; bodyIt != end; bodyIt++)
	{
		(*bodyIt)->UpdatePosition(deltaTime);
	}


}

void CSpaghettiWorld::AddCollision( 
		CSpaghettiRigidBody *bodyOne, 
		CSpaghettiRigidBody *bodyTwo, 
		SAM::TVector3 &point, 
		SAM::TVector3 &normal, 
		const float penetration 
	)
{
	CCollision *cp = nullptr;

	for (unsigned int collisionIndex = 0; collisionIndex < m_collisions.size(); ++collisionIndex)
	{
		if (m_collisions[collisionIndex].m_bodyOne == bodyOne && m_collisions[collisionIndex].m_bodyTwo == bodyTwo)
		{
			cp = &m_collisions[collisionIndex];
			break;
		}
	}

	if (cp == nullptr)
	{
		CCollision newCollision;
		newCollision.m_bodyOne = bodyOne;
		newCollision.m_bodyTwo = bodyTwo;
		newCollision.m_noofPoints = 1;
		
		CCollision::CollisionPoints newCollisionPoints;
		newCollisionPoints.normal = normal;
		newCollisionPoints.point = point;
		newCollisionPoints.penetration = penetration;
		newCollisionPoints.positionOne = bodyOne->GetPosition();
		newCollisionPoints.positionTwo = bodyTwo->GetPosition();
		
		newCollision.m_points.push_back(newCollisionPoints);
		m_collisions.push_back(newCollision);				
		return;
	}

	CCollision::CollisionPoints newCollisionPoints;
	newCollisionPoints.normal = normal;
	newCollisionPoints.point = point;
	newCollisionPoints.penetration = penetration;
	newCollisionPoints.positionOne = bodyOne->GetPosition();
	newCollisionPoints.positionTwo = bodyTwo->GetPosition();	

	cp->m_points.push_back(newCollisionPoints);
	cp->m_noofPoints++;
}

void CSpaghettiWorld::ApplyImpulses( 
		float deltaTime 
	)
{
	const unsigned int noofColisions = m_collisions.size();

	for (unsigned int collisionIndex = 0; collisionIndex < noofColisions; ++collisionIndex)
	{
		CCollision *cp = &m_collisions[collisionIndex];

		CSpaghettiRigidBody *bodyOne = cp->m_bodyOne;
		CSpaghettiRigidBody *bodyTwo = cp->m_bodyTwo;
		int numPoints = cp->m_noofPoints;

		SAM::TVector3 bodyOneVel, bodyTwoVel;

		for (int collisionPointIndex = 0; collisionPointIndex < numPoints; ++collisionPointIndex)
		{
			SAM::TVector3 hitPoint = cp->m_points[collisionPointIndex].point;
			SAM::TVector3 normal = cp->m_points[collisionPointIndex].normal;
			float penDepth = cp->m_points[collisionPointIndex].penetration;

			AddCollisionImpulse(bodyOne, bodyTwo, hitPoint, normal, deltaTime, bodyOneVel, bodyTwoVel);
		}

		bodyOneVel = bodyOneVel / static_cast<float>(numPoints);
		bodyTwoVel = bodyTwoVel / static_cast<float>(numPoints);

		static const float minSquaredVelocitySquared = 10.0f;
		if (bodyOneVel.LengthSquared() < minSquaredVelocitySquared) bodyOneVel.Set(0, 0, 0);
		if (bodyTwoVel.LengthSquared() < minSquaredVelocitySquared) bodyTwoVel.Set(0, 0, 0);

		bodyOne->SetPosition(bodyOne->GetLastPosition());
		bodyOne->SetVelocity(bodyOneVel);
		
		bodyTwo->SetPosition(bodyTwo->GetLastPosition());
		bodyTwo->SetVelocity(bodyTwoVel);
	}
}

void CSpaghettiWorld::AddCollisionImpulse( 
		CSpaghettiRigidBody *bodyOne, 
		CSpaghettiRigidBody *bodyTwo, 
		SAM::TVector3& hitPoint, 
		SAM::TVector3& normal, 
		float deltaTime, 
		SAM::TVector3 &velBodyOne,
		SAM::TVector3 &velBodyTwo
	)
{
	
	static const float e = -0.5f;

	SAM::TVector3 relativeVelocity = bodyOne->GetVelocity() - bodyTwo->GetVelocity();

	const float inverseMassBodyOne = 1.0f / bodyOne->GetMass();
	const float inverseMassBodyTwo = 1.0f / bodyTwo->GetMass();
	const float sumOfInverseMass = inverseMassBodyOne + inverseMassBodyTwo;

	const float jLinear = (-((1.0f + e) * relativeVelocity.Dot(normal))) / sumOfInverseMass;
	velBodyOne = velBodyOne + ((normal * jLinear) * inverseMassBodyOne);
	velBodyTwo = velBodyTwo + ((normal * -jLinear) * inverseMassBodyTwo);
}
