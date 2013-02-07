#pragma once

#include <Ogre/Ogre.h>

class CSpaghettiRigidBody;

class CCollision {

public:
	CSpaghettiRigidBody	*bodyOne;
	CSpaghettiRigidBody	*bodyTwo;
	Ogre::Vector3 collisionPoint;
	Ogre::Vector3 collisionNormal;
};
