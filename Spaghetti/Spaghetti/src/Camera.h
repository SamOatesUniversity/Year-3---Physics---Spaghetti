#pragma once

#include "Ogre/Ogre.h"
#include "OIS/Ois.h"

class CCamera
{
private:

	Ogre::SceneNode*							m_sceneNode;
	Ogre::Camera*								m_camera;

	Ogre::Vector3								m_rotation;

public:
												//! Class constructor
												CCamera(
													Ogre::SceneNode* node,
													Ogre::Camera* camera
												);

												//! Class destructor
												~CCamera(void);

												//! Update the camera with respect to the keyboard and mouse input
	void										Update(
													OIS::Keyboard *kb,
													OIS::Mouse *ms
												);
};

