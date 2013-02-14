#include "CSpaghettiRigidBodySphere.h"

/*
*	\brief Class constructor
*/
CSpaghettiRigidBodySphere::CSpaghettiRigidBodySphere(
	void *renderObject									//!< A pointer to an object this rigid body will represent
	) : CSpaghettiRigidBody(renderObject)
{

}

/*
*	\brief Class destructor
*/
CSpaghettiRigidBodySphere::~CSpaghettiRigidBodySphere()
{

}

/*
*	\brief	Calculate the inertia based upon the bounding sphere
*/
void CSpaghettiRigidBodySphere::CalculateInertiaBodyTensor()
{
	static const float twoOverFive = 2.0f / 5.0f;
	const float radius = m_bounds->Height() * 0.5f;
	const float radiusSquared = radius * radius;

	m_inertiaTensor[0][0] = twoOverFive * (m_mass * radiusSquared); m_inertiaTensor[0][1] = 0; m_inertiaTensor[0][2] = 0;
	m_inertiaTensor[1][0] = 0; m_inertiaTensor[1][1] = twoOverFive * (m_mass * radiusSquared);  m_inertiaTensor[1][2] = 0;
	m_inertiaTensor[2][0] = 0; m_inertiaTensor[2][1] = 0; m_inertiaTensor[2][2] = twoOverFive * (m_mass * radiusSquared);

	m_inertiaTensorBodyInverse = m_inertiaTensor.Inverse();
}
