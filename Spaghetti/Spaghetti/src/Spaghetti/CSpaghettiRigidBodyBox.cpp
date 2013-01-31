#include "CSpaghettiRigidBodyBox.h"

/*
*	\brief Class constructor
*/
CSpaghettiRigidBodyBox::CSpaghettiRigidBodyBox(
		void *renderObject									//!< A pointer to an object this rigid body will represent
	) : CSpaghettiRigidBody(renderObject)
{

}

/*
*	\brief Class destructor
*/
CSpaghettiRigidBodyBox::~CSpaghettiRigidBodyBox()
{

}

/*
*	\brief	Calculate the inertia based upon the bounding box
*/
void CSpaghettiRigidBodyBox::CalculateInertiaBodyTensor()
{
	static const float oneOverTwelve = 1.0f / 12.0f;
	const float heightSquared = m_bounds->Height() * m_bounds->Height();
	const float depthSquared = m_bounds->Depth() * m_bounds->Depth();
	const float widthSquared = m_bounds->Width() * m_bounds->Width();

	m_inertiaTensor[0][0] = (oneOverTwelve * m_mass) * (heightSquared + depthSquared); m_inertiaTensor[0][1] = 0; m_inertiaTensor[0][2] = 0;
	m_inertiaTensor[1][0] = 0; m_inertiaTensor[1][1] = (oneOverTwelve * m_mass) * (widthSquared + depthSquared);  m_inertiaTensor[1][2] = 0;
	m_inertiaTensor[2][0] = 0; m_inertiaTensor[2][1] = 0; m_inertiaTensor[2][2] = (oneOverTwelve * m_mass) * (widthSquared + heightSquared);

	m_inertiaTensorBodyInverse = m_inertiaTensor.Inverse();
}

/*
*	\brief Update all simulation objects in the world
*/
void CSpaghettiRigidBodyBox::Update( 
		const CSpaghettiWorld *world,
		const unsigned long deltaTime							//!< Delta time (The amount of time past since the last update)
	)
{
	// Nothing to update if he rigid body is static or disabled
	if (m_flags.isStatic || !m_flags.isEnabled)
		return;

	m_lastPosition = m_position;

	m_velocity = m_velocity + world->GetGravity() * (static_cast<float>(deltaTime) * SPEEDSCALER);
	m_position = m_position + (m_velocity * (static_cast<float>(deltaTime) * SPEEDSCALER));
	m_angularMomentum = m_angularMomentum;

	UpdateInertiaTensor();
	UpdateAngularVelocity();

	// update skew matrix
	SAM::TMatrix<float, 3, 3> skewMatrix;
	skewMatrix[0][0] = 0.0f;
	skewMatrix[0][1] = -m_angularVelocity.Z();
	skewMatrix[0][2] = m_angularVelocity.Y();

	skewMatrix[1][0] = m_angularVelocity.Z();
	skewMatrix[1][1] = 0.0f;
	skewMatrix[1][2] = -m_angularVelocity.X();

	skewMatrix[2][0] = -m_angularVelocity.Y();
	skewMatrix[2][1] = m_angularVelocity.X();
	skewMatrix[2][2] = 0.0f;

	// update rotation matrix
	m_rotation = m_rotation + ((skewMatrix * m_rotation) * (static_cast<float>(deltaTime) * SPEEDSCALER));

	// update and normalize the quaternion
	m_quaternion.FromMatrix3x3(m_rotation);
	m_quaternion.Normalize();

	m_bounds->Transform(m_position, m_rotation);
}

void AddCollisionImpulse(
		CSpaghettiRigidBody *c0, 
		CSpaghettiRigidBody *c1, 
		SAM::TVector<float, 3> &hitPoint, 
		SAM::TVector<float, 3> &normal, 
		float dt,
		float penetration
	)
{

	// Some simple check code.
	if (dt <= 0.0) return;

	float invMass0 = 1.0f / c0->GetMass();
	float invMass1 = 1.0f / c1->GetMass();

	invMass0 = c0->IsStatic() ? 0.0f : invMass0;
	invMass1 = c1->IsStatic() ? 0.0f : invMass1;

	// Both objects are non movable
	if ( (invMass0+invMass1) == 0.0f ) return;
	
	SAM::TVector<float, 3> r0 = hitPoint - c0->GetPosition();
	SAM::TVector<float, 3> r1 = hitPoint - c1->GetPosition();

	SAM::TVector<float, 3> v0 = c0->GetVelocity() + c0->GetAngularVelocity().Cross(r0);
	SAM::TVector<float, 3> v1 = c1->GetVelocity() + c1->GetAngularVelocity().Cross(r1);

	// Relative Velocity
	SAM::TVector<float, 3> dv = v0 - v1;

	// NORMAL Impulse Code

	// Compute Normal Impulse
	float vn = dv.Dot(normal);

	// Works out the bias to prevent Prevents sinking!
	const float allowedPenetration = 0.1f;
	const float biasFactor = 0.1f; // 0.1 to 0.3
	float biasFactorValue = true ? biasFactor : 0.0f;

	float inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;
	float bias = biasFactorValue * inv_dt * max(0.0f, penetration - allowedPenetration);

	SAM::TVector<float, 3> ac = c0->GetInverseInertia() * r0.Cross(normal);
	SAM::TVector<float, 3> bc = c1->GetInverseInertia() * r1.Cross(normal);
	SAM::TVector<float, 3> crossNormal = ac.Cross(r0) + bc.Cross(r1);

	float kNormal = invMass0 + invMass1 + normal.Dot(crossNormal);

	float massNormal = 1.0f / kNormal;

	float dPn = massNormal * (-vn + bias);

	dPn = max(dPn, 0.0f);

	// Apply normal contact impulse
	SAM::TVector<float, 3> P = normal * dPn;

	c0->SetVelocity(c0->GetVelocity() + (P * invMass0));
	c0->SetAngularVelocity(c0->GetAngularVelocity() + (c0->GetInverseInertia() * r0.Cross(P)));

	c1->SetVelocity(c1->GetVelocity() + (P * invMass1));
	c1->SetAngularVelocity(c1->GetAngularVelocity() + (c1->GetInverseInertia() * r1.Cross(P)));
	// NORMAL
	
	// TANGENT Impulse Code
	{
		// Work out our tangent vector, with is perpendicular
		// to our collision normal
		SAM::TVector<float, 3> tangent;
		tangent = dv - (normal * dv.Dot(normal));
		tangent = tangent.Normalize();

		SAM::TVector<float, 3> ac = c0->GetInverseInertia() * r0.Cross(tangent);
		SAM::TVector<float, 3> bc = c1->GetInverseInertia() * r1.Cross(tangent);
		SAM::TVector<float, 3> crossTangent = ac.Cross(r0) + bc.Cross(r1);

		float kTangent = invMass0 + invMass1 + tangent.Dot(crossTangent);

		float massTangent = 1.0f / kTangent;

		float vt = dv.Dot(tangent);
		float dPt = massTangent * (-vt);
		
		const static float FRICTION = 0.6f;
		float maxPt = FRICTION * dPn;
		dPt = SAM::Clamp(dPt, -maxPt, maxPt);
		
		// Apply contact impulse
		SAM::TVector<float, 3> P = tangent * dPt;

		c0->SetVelocity(c0->GetVelocity() + (P * invMass0));
		c0->SetAngularVelocity(c0->GetAngularVelocity() + (c0->GetInverseInertia() * r0.Cross(P)));

		c1->SetVelocity(c1->GetVelocity() + (P * invMass1));
		c1->SetAngularVelocity(c1->GetAngularVelocity() + (c1->GetInverseInertia() * r1.Cross(P)));
	}
}

/*
*	\brief	Handle collision against another rigid body
*/
void CSpaghettiRigidBodyBox::HandleCollision(
		CSpaghettiRigidBody *otherRigidBody								//!< The other rigid body to compare against
	)
{
	if (!m_flags.isEnabled)
		return;

	if (m_bounds->Intersects(otherRigidBody->GetBounds()))
	{
		int numPoints = m_bounds->GetHitPoints().size();

		for (int k = 0; k < numPoints; ++k)
		{

			SAM::TVector<float, 3> hitPoint	= m_bounds->GetHitPoints()[k];
			SAM::TVector<float, 3> normal = m_bounds->GetHitNormal();
			float penDepth		 = m_bounds->GetPenetration();

			AddCollisionImpulse(this, otherRigidBody, hitPoint, normal, 3.0f, penDepth);
		}
	}
}