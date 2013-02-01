#pragma once

#include "../../../SAM/SAM/SAM.h"

#include <vector>
class CSpaghettiRigidBody;

class CCollision {

public:

	struct CollisionPoints {
		SAM::TVector3	point;
		SAM::TVector3	normal;
		SAM::TVector3	positionOne;
		SAM::TVector3	positionTwo;
		float			penetration;
	};

	CSpaghettiRigidBody						*m_bodyOne;
	CSpaghettiRigidBody						*m_bodyTwo;
	int										m_noofPoints;
	std::vector<CollisionPoints>			m_points;
};
