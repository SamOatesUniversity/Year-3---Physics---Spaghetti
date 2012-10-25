#pragma once

//Ogre headers
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


#include "SimpleOgreInit.h"


using namespace Ogre;
using namespace OgreEasy;


class OgreApplication
{
public:
	OgreApplication(void);
	~OgreApplication(void);
	void cleanUp();

	bool init(); // init SimpleOgreInit 
	void createCamera(Ogre::String  name);
	void createOIS(); // create output/input system
	void createLights();
	void createScene(); // setup everything we want to render

	void run(bool updateOption=false, bool synchroOption=true); // run the application
	
	Ogre::Timer* createTimer();
	void resetTimer();

	const OgreEasy::SimpleOgreInit& getSimpleOgreInitHandle(){return lOgreInit;} 
	Ogre::SceneManager*  getSceneManager(){return lScene;}
	Ogre::SceneNode* getChildSceneNodeByName(Ogre::String name);

	OIS::Keyboard* getKeyboard(){return lKeyboard;}
	OIS::Mouse* getMouse(){return lMouse;}

    
private:
	
	void addChildNodeToList(Ogre::String name,  Ogre::SceneNode *cNode);
	struct sceneNode
	{
		Ogre::String Name;
		Ogre::SceneNode*  Node;
	};

	std::vector<sceneNode>  childSceneNodeList;

	OgreEasy::SimpleOgreInit lOgreInit;
	Ogre::Root* lRoot;
	Ogre::RenderWindow*lWindow;
	Ogre::SceneManager* lScene;
	Ogre::SceneNode* lRootSceneNode;
	Ogre::Viewport* vp;

	// O/I: mouse and keyboard inputs
    //Create the necessary objects of OIS to manage Keyboard and Mouse.
	OIS::InputManager* lInputManager;
	OIS::Mouse* lMouse;
	OIS::Keyboard* lKeyboard;

	// timer
	Ogre::Timer* lOgreTimer;

	//resource groups
	Ogre::String lNameOfResourceGroup;
	Ogre::String lDirectoryToLoadMeshes; 
	Ogre::String lDirectoryToLoadTextures; 

	Ogre::SceneNode						*m_camera;					//!< A pointer to the camrera node
	Ogre::SceneNode						*m_ninjaNode;				//!< A pointer to the player enity node

};

