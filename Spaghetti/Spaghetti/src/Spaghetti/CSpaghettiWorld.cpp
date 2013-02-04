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

		for (int collisionPointIndex = 0; collisionPointIndex < numPoints; ++collisionPointIndex)
		{
			SAM::TVector3 hitPoint = cp->m_points[collisionPointIndex].point;
			SAM::TVector3 normal = cp->m_points[collisionPointIndex].normal;
			float penDepth = cp->m_points[collisionPointIndex].penetration;

			AddCollisionImpulse(bodyOne, bodyTwo, hitPoint, normal, deltaTime, penDepth);
		}
	}
}

void CSpaghettiWorld::AddCollisionImpulse( 
		CSpaghettiRigidBody *bodyOne, 
		CSpaghettiRigidBody *bodyTwo, 
		SAM::TVector3& hitPoint, 
		SAM::TVector3& normal, 
		float deltaTime, 
		float penetration 
	)
{
	
	float invMass0 = bodyOne->IsStatic() ? 0.0f : (1.0f / bodyOne->GetMass());
	float invMass1 = bodyTwo->IsStatic() ? 0.0f : (1.0f / bodyTwo->GetMass());

	SAM::TVector3 r0 = hitPoint - bodyOne->GetPosition();
	SAM::TVector3 r1 = hitPoint - bodyTwo->GetPosition();

	SAM::TVector3 v0 = bodyOne->GetVelocity() + bodyOne->GetAngularVelocity().Cross(r0);
	SAM::TVector3 v1 = bodyTwo->GetVelocity() + bodyTwo->GetAngularVelocity().Cross(r1);

	// Relative Velocity
	SAM::TVector3 dv = v0 - v1;

	// normal collision
	{
		// Compute Normal Impulse
		float vn = dv.Dot(normal);

		// Works out the bias to prevent Prevents sinking!
		const float allowedPenetration = 0.0f;
		const float biasFactor = 0.1f;

		float inv_dt = 1.0f / deltaTime;
		float bias = biasFactor * inv_dt * SAM::Max(0.0f, penetration - allowedPenetration);
		
		SAM::TVector3 ac = (bodyOne->GetInverseInertia() * r0.Cross(normal)).Cross(r0);
		SAM::TVector3 bc = (bodyTwo->GetInverseInertia() * r1.Cross(normal)).Cross(r1);
		float kNormal = invMass0 + invMass1 + normal.Dot(ac + bc);

		float massNormal = 1.0f / kNormal;
		float dPn = massNormal * ((-vn) + bias);
		dPn = SAM::Max(dPn, 0.0f);
		
		// Apply normal contact impulse
		SAM::TVector3 P = normal * dPn;

		bodyOne->SetVelocity(bodyOne->GetVelocity() + (P * invMass0)); // c0.m_linVelocity += invMass0 * P;
		//bodyOne->SetAngularVelocity(bodyOne->GetAngularVelocity() + (bodyOne->GetInverseInertia() * r0.Cross(P))); // c0.m_angVelocity += Cross(r0, P) * c0.m_invInertia;
		
		bodyTwo->SetVelocity(bodyTwo->GetVelocity() - (P * invMass1)); // c1.m_linVelocity -= invMass1 * P;
		//bodyTwo->SetAngularVelocity(bodyTwo->GetAngularVelocity() - (bodyTwo->GetInverseInertia() * r1.Cross(P))); // c1.m_angVelocity -= Cross(r1, P) * c1.m_invInertia;
	}

	// Tangent collision
	{
		SAM::TVector3 tangent = dv - (normal * dv.Dot(normal));
		tangent = tangent.Normalize();

		SAM::TVector3 ac = (bodyOne->GetInverseInertia() * r0.Cross(tangent)).Cross(r0);
		SAM::TVector3 bc = (bodyTwo->GetInverseInertia() * r1.Cross(tangent)).Cross(r1);
		float kTangent = invMass0 + invMass1 + tangent.Dot(ac + bc);

		float massTangent = 1.0f / kTangent;
		float vt = dv.Dot(tangent);
		float dPt = massTangent * (-vt);

		static const float FRICTION = 1.0f;
		float maxPt = FRICTION * dPt;
		dPt = SAM::Clamp(dPt, -maxPt, maxPt);

		// Apply contact impulse
		SAM::TVector3 P = tangent * dPt;
		
		bodyOne->SetVelocity(bodyOne->GetVelocity() + (P * invMass0)); //c0.m_linVelocity += invMass0 * P;
		//c0.m_angVelocity += Cross(r0, P) * c0.m_invInertia;

		bodyTwo->SetVelocity(bodyTwo->GetVelocity() - (P * invMass1)); //c1.m_linVelocity -= invMass1 * P;
		//c1.m_angVelocity -= Cross(r1, P) * c1.m_invInertia;
	}

}
