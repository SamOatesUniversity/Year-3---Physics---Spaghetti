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