#pragma once

#include "../../../SAM/SAM/SAM.h"

#include <vector>
class CSpaghettiRigidBody;

class CCollision {

public:
	CSpaghettiRigidBody	*bodyOne;
	CSpaghettiRigidBody	*bodyTwo;
	SAM::TVector3 collisionPoint;
	SAM::TVector3 collisionNormal;
};
