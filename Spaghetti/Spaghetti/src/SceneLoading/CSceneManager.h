#pragma once

#include <Windows.h>
#include <direct.h>
#include <vector>
#include <sstream>

#include "CScene.h"
#include "..\OgreApplication.h"
#include "..\Spaghetti\Spaghetti.h"

class CSceneManager {

private:

	std::vector<CScene*>					m_scene;								//!< A list of loaded scenes
	unsigned int							m_currentScene;							//!< The index of the current scene

	std::vector<CSpaghettiRigidBody*>		m_body;									//!< A list of current rigid bodies

private:
											//! Create a node from a given node map
	void									CreateNodeFromMap(
												OgreApplication *application, 
												CSpaghetti *spaghetti,
												CSpaghettiWorld *world,
												NodeMap *nodeMap 
											);

											//! 
	Ogre::Vector3							ParseVector3(
												std::string vectorbuffer			//!< 
											);

public:
											//! Class constructor
											CSceneManager();

											//! Class destructor
											~CSceneManager();

											//! Load all the .scene files from a given folder
											//! Returns the number of scenes that were loaded
	const int								LoadScenes(
												std::string sceneFolder				//!< The folder to search for .scene files
											);

											//! Setup the current scene for use with ogre
	void									SetupCurrentScene(
												OgreApplication *application,
												CSpaghetti *spaghetti,
												CSpaghettiWorld *world
											);

											//! Update all the nodes in the scene
	void									Update();

											//! Set the current scene
	void									SetCurrentScene(
												int index,												//!< The index to set the current scene too
												OgreApplication *application, 
												CSpaghetti *spaghetti,
												CSpaghettiWorld *world
											);

											//! 
	void									ReleaseActiveScene( 
												OgreApplication *const application, 
												CSpaghetti *const spaghetti, 
												CSpaghettiWorld *const world 
											);
};