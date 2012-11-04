#ifndef _INCLUDE_OGRE_APPLICATION_
#define _INCLUDE_OGRE_APPLICATION_

// Required Ogre includes
#include "OGRE/OgreRoot.h"
#include "OGRE/OgreRenderSystem.h"
#include "OGRE/OgreRenderWindow.h"
#include "OGRE/OgreWindowEventUtilities.h"
#include "OGRE/OgreManualObject.h"
#include "OGRE/OgreMaterialManager.h"
#include "OGRE/OgreEntity.h"
#include "OGRE/OgreParticleSystem.h"
#include "OGRE/OgreParticleSystemManager.h"
#include "OGRE/OgreParticleEmitter.h"
#include "OGRE/OgreOverlayManager.h"
#include "OGRE/OgreOverlayContainer.h"
#include "OGRE/OgreTextAreaOverlayElement.h"
#include "OGRE/OgreMeshManager.h"
#include "OIS/Ois.h"

// My Ogre wrapper
#include "OgreWrapper.h"

struct SceneNodeAndName {
	Ogre::String name;
	Ogre::SceneNode *node;
};

// OgreApplication class declaration
class OgreApplication
{
private:
	OgreWrapper								m_ogreWrapper;								//!< Instance of our ogre intialisation wrapper

	Ogre::SceneManager						*m_scene;									//!< The instance of the ogre scene
	Ogre::SceneNode							*m_rootSceneNode;							//!< The root node of our scene
	Ogre::Viewport							*m_vp;										//!< The render viewport
	Ogre::Timer								*m_timer;									//!< 

	OIS::InputManager						*m_inputManager;							//!< Instance of the input manager
	OIS::Mouse								*m_mouse;									//!< Instance of the mouse device
	OIS::Keyboard							*m_keyboard;								//!< Instance of the keyboard device
	
	std::vector<SceneNodeAndName>			m_sceneNodes;								//!< The list of all our nodes in the scene. Identified by a name

	Ogre::SceneNode							*m_camera;									//!< A pointer to the camrera node for easy access

private:
											//! Create all the lights in the scene
	void									CreateLights();

											//! Add a scene node to out list of scene nodes
	void									AddNodeToList(
												const Ogre::String name,				//!< Name of the scene node to add
												Ogre::SceneNode *const node				//!< A pointer to the scene node to add
											);

public:
											//! Class constructor
											OgreApplication();

											//! Class destructor
											~OgreApplication();

											//! Free all allocated resources
	void									Release();

											//! Initialize the ogre application class
	const bool								Initialize();

											//! Create a camera and attach it to a scene node
	Ogre::SceneNode							*CreateCamera(
												Ogre::String name						//!< A name to give to the camera
											);

											//! Create the input output system
	void									CreateIOS();

											//! Create the ogre scene
	void									CreateScene();

											//! The main run loop
	void									Run(
												bool updateOption = false,				//!< 
												bool synchroOption = true				//!< 
											);
	
											//! Create a timer
	Ogre::Timer								*CreateTimer();
		
											//! Get access to the ogre wrapper instance
	const OgreWrapper						&GetOgreWrapper()
											{
												return m_ogreWrapper;
											}

											//! Get external access to the scene pointer
	Ogre::SceneManager						*GetSceneManager() const
											{
												return m_scene;
											}

											//! Get external access to the keyboard pointer
	OIS::Keyboard							*GetKeyboard() const
											{
												return m_keyboard;
											}

											//! Get external access to the mouse pointer
	OIS::Mouse								*GetMouse() const
											{
												return m_mouse;
											}

	Ogre::SceneNode							*FindNodeByName(
												const Ogre::String name
											) const;
};

#endif