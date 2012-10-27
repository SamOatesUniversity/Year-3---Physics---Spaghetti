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

// OgreApplication class declaration
class OgreApplication
{
private:
	OgreWrapper								m_ogreWrapper;								//!< 

	Ogre::SceneManager						*m_scene;									//!< 
	Ogre::SceneNode							*m_rootSceneNode;							//!< 
	Ogre::Viewport							*m_vp;										//!< 

	OIS::InputManager						*m_inputManager;							//!< 
	OIS::Mouse								*m_mouse;									//!< 
	OIS::Keyboard							*m_keyboard;								//!< 

	Ogre::Timer								*m_timer;									//!< 

	Ogre::String							m_nameOfResourceGroup;						//!< 
	Ogre::String							m_directoryToLoadMeshes;					//!< 
	Ogre::String							m_directoryToLoadTextures;					//!< 

	Ogre::SceneNode							*m_camera;									//!< A pointer to the camrera node

public:
											//! Class constructor
											OgreApplication();

											//! Class destructor
											~OgreApplication();

											//! Free all allocated resources
	void									Release();

											//! Initialize the ogre application class
	const bool								Initialize();

											//! 
	void									CreateCamera(
												const Ogre::String name					//!<
											);

											//! 
	void									CreateOIS();

											//! 
	void									CreateLights();

											//! 
	void									CreateScene();

											//! 
	void									Run(
												bool updateOption = false,				//!< 
												bool synchroOption = true				//!< 
											);
	
											//! 
	Ogre::Timer								*CreateTimer();
	
											//! 
	void									ResetTimer();
		
											//! 
	const OgreWrapper						&GetOgreWrapper()
											{
												return m_ogreWrapper;
											}

											//! 
	Ogre::SceneManager						*GetSceneManager() const
											{
												return m_scene;
											}

											//! 
	Ogre::SceneNode							*GetChildSceneNodeByName(
												const Ogre::String name					//!< 
											) const;

											//! 
	OIS::Keyboard							*GetKeyboard() const
											{
												return m_keyboard;
											}

											//! 
	OIS::Mouse								*GetMouse() const
											{
												return m_mouse;
											}
};

#endif