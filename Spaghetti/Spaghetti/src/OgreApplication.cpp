#include "OgreApplication.h"


OgreApplication::OgreApplication(void):lRoot(NULL), lWindow(NULL), lScene(NULL), lRootSceneNode(NULL), vp(NULL),
	lInputManager(NULL), lMouse(NULL), lKeyboard(NULL), lOgreTimer(NULL)
{

}

OgreApplication::~OgreApplication(void)
{
	cleanUp();

	lRoot = NULL;
	lWindow = NULL;
	lScene = NULL;
	lRootSceneNode = NULL;
	vp = NULL;

	lInputManager = NULL;
	lMouse = NULL;
	lKeyboard = NULL;
	lOgreTimer = NULL;
}

void OgreApplication::cleanUp()
{
	// Let's cleanup!
	{
		lWindow->removeAllViewports();
	}
	{   
		lScene->destroyAllCameras();
		lScene->destroyAllLights();
		lScene->destroyAllManualObjects();
		lScene->destroyAllEntities();
		lRootSceneNode->removeAndDestroyAllChildren();
	}
	{   
		childSceneNodeList.clear();

	}
}


bool OgreApplication::init()
{
	
	if(!lOgreInit.initOgre())
	{
		std::cout<<"Impossible to init Ogre correctly."<<std::endl;
		
		return false;
	}
	
	// Create  Root, RenderWindow, SceneManager, and SceneNode 
	// Get RootSceneNode from SceneManager
	lRoot = lOgreInit.mRoot.get();
	lWindow = lOgreInit.mWindow;
	lScene = lRoot->createSceneManager(Ogre::ST_GENERIC, "MySceneManager");
	lRootSceneNode = lScene->getRootSceneNode();


	this->childSceneNodeList.clear();
	// setup resource directories
	/* Choose a name for a resource group. A resource group is a collection of data related to surface materials, textures
	 etc. for a game level, or a game object.
	 Create a resource group.
	 Often, a resourcegroup is a good way to store the data corresponding
	 to a level in a game.*/
	lNameOfResourceGroup = "Physics Simulation";
	lDirectoryToLoadMeshes = "../../media/meshes";
	lDirectoryToLoadTextures = "../../media/textures";

	return true;
}

void OgreApplication::createCamera(Ogre::String  name)
{

	// Create a camera using SceneManager. It represents a 'point of view' in the scene.
	Ogre::Camera* lCamera = lScene->createCamera(name);

	// Create a child scene node of the RootSceneNode,
	// Attach the camera to a new SceneNode. It will be easier then to move it in the scene.
	Ogre::SceneNode* m_camera = lRootSceneNode->createChildSceneNode(name.append("Node"));
	m_camera->attachObject(lCamera);

	// Setup viewport parameters 
	// Add viewport to the window with the camera 
	float lViewportWidth = 1.0f;//0.88f;
	float lViewportHeight = 1.0f;//0.88f;
	float lViewportLeft	= (1.0f - lViewportWidth) * 0.5f;
	float lViewportTop = (1.0f - lViewportHeight) * 0.5f;
	unsigned short lMainViewportZOrder = 100;
	
	vp = lWindow->addViewport(lCamera, lMainViewportZOrder, lViewportLeft, lViewportTop, lViewportWidth, lViewportHeight);

	// Set the viewport to draw the scene automatically
	// whenever the window is updated by the call lWindow->update();
	vp->setAutoUpdated(true);

	// Choose a color for this viewport. 
	// Have a bright colour for now
	vp->setBackgroundColour(Ogre::ColourValue(0.2f, 0.2f, 0.3f));

	// Setup the visual ratio of the camera. To make it looks real, same as the viewport.
	float ratio = float(vp->getActualWidth()) / float(vp->getActualHeight());
	lCamera->setAspectRatio(ratio);
	lCamera->setNearClipDistance(1.5f);
	lCamera->setFarClipDistance(3000.0f); 
	m_camera->setPosition(Ogre::Vector3(0.00f, 20.0f, 30.0f));
	lCamera->lookAt(0.0f, 0.0f, 0.0f);

	this->addChildNodeToList(m_camera->getName(), m_camera);
	

	// Set the window to be active
	lWindow->setActive(true);

	// Update the content of the window not automatically.
	lWindow->setAutoUpdated(false);

	// cleaning of windows events managed by Ogre::WindowEventUtilities::...
	// I call it after a 'pause in window updating', in order to maintain smoothness.
	// Explanation : if you clicked 2000 times when the windows was being created, there are
	// at least 2000 messages created by the OS to listen to. This is made to clean them.
	lRoot->clearEventTimes();

}

Ogre::SceneNode* OgreApplication::getChildSceneNodeByName(Ogre::String name)
{
	Ogre::SceneNode *tempNode;

	for(unsigned int i = 0; i<childSceneNodeList.size(); i++)
	{
		if(childSceneNodeList[i].Name == name)
		{
			 tempNode = childSceneNodeList[i].Node;
			 break;
		}
	}
	return tempNode; 
}

void OgreApplication::addChildNodeToList(Ogre::String name, Ogre::SceneNode *cNode)
{
	sceneNode  tempSceneNode;
	tempSceneNode.Name = name;
	tempSceneNode.Node = cNode;
	childSceneNodeList.push_back(tempSceneNode);

}

void OgreApplication::createLights()
{
	// Create a new SceneNode - light Scene Node
	// Create a light in the SceneManager, and set the type of the light: {LT_POINT = 0, LT_DIRECTIONAL = 1, LT_SPOTLIGHT = 2 }

	// create a directional light
	 Ogre::SceneNode* lLightSceneNodeDir = NULL;
	 Ogre::String tempName = "DirectionLight";
     Ogre::Light* lDirectionalLight = lScene->createLight(tempName);
	 lDirectionalLight->setType(Ogre::Light::LT_DIRECTIONAL);

	//Set light clolur to white
	lDirectionalLight->setDiffuseColour(0.8f, 0.8f, 0.8f); 
	lDirectionalLight->setSpecularColour(1.0f, 1.0f, 1.0f);
	
	//Set light node as the child node of the RootSceneNode
	//Attach the light to the chid node
	lLightSceneNodeDir = lRootSceneNode->createChildSceneNode(tempName.append("Node"));
	lLightSceneNodeDir->attachObject(lDirectionalLight);
	//Set light position : rember must set the light position after attached it to SceneNode
	lLightSceneNodeDir->setDirection(1.0f, 1.0f, -1.0f);
	
	// add directional light to the child node list
	this->addChildNodeToList(lLightSceneNodeDir->getName(), lLightSceneNodeDir);
	
	// create a spot light
    Ogre::SceneNode* lLightSceneNodeSpot = NULL;
	tempName = "SpotLight";
    Ogre::Light* lSpotLight = lScene->createLight(tempName);
	lSpotLight->setType(Ogre::Light::LT_SPOTLIGHT);

	//Set light clolur to red
	lSpotLight->setDiffuseColour(0.0f, 0.8f, 0.8f); 
	lSpotLight->setSpecularColour(0.0f, 1.0f, 1.0f);
	
	//Set light node as the child node of the RootSceneNode
	//Attach the light to the chid node
	lLightSceneNodeSpot = lRootSceneNode->createChildSceneNode(tempName.append("Node"));
	lLightSceneNodeSpot->attachObject(lSpotLight);
	lLightSceneNodeSpot->setDirection(1.0f, 1.0f, 0.0f);
	lLightSceneNodeSpot->setPosition(Ogre::Vector3(50.0, 80.0, 0.0));
	//set the spot light corn beam width
	lSpotLight->setSpotlightRange(Ogre::Degree(20), Ogre::Degree(50));

	// add to child node list
	this->addChildNodeToList(lLightSceneNodeSpot->getName(), lLightSceneNodeSpot);

	// tell the SceneManager where the mesh objects are stored and prepare the sence for load and render entities       //
	// First, add an ambient color to lit the scene. The ambient color is managed in the scenemanager.
	// If you want to learn more about ambient/specular/diffuse color, check the 'basic material tutorial'.
	Ogre::ColourValue lAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	lScene->setAmbientLight(lAmbientColour);

	// There are three shadow techniques: Ogre::SHADOWTYPE_TEXTURE_MODULATIVE - least expensive and accurate,
	// Ogre::SHADOWTYPE_STENCIL_MODULATIVE - less expensive and accurate,  Ogre::SHADOWTYPE_STENCIL_ADDITIVE -most expensive and accurate
	lScene->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
}

void OgreApplication::createScene()
{
	//1. Firstly, create some lights to illuminate the scene
	createLights();

	// 2. Secondly, get a reference on the material manager, which is a singleton. 
    Ogre::MaterialManager& lMaterialManager = Ogre::MaterialManager::getSingleton();


	{
		// Create a resource group manager and create the resource group
		Ogre::ResourceGroupManager& lRgMgr = Ogre::ResourceGroupManager::getSingleton();
		lRgMgr.createResourceGroup(lNameOfResourceGroup);

		bool lIsRecursive = false;

		// Assign directories for meshes, and textures, and add to the resource group "Physics Simulation"
	    lRgMgr.addResourceLocation(lDirectoryToLoadMeshes, "FileSystem", lNameOfResourceGroup, lIsRecursive);
		lRgMgr.addResourceLocation(lDirectoryToLoadTextures, "FileSystem", lNameOfResourceGroup, lIsRecursive);
		
		// The function 'initialiseResourceGroup' parses scripts if any in the locations.
		lRgMgr.initialiseResourceGroup(lNameOfResourceGroup);

		// Files that can be loaded are loaded.
		lRgMgr.loadResourceGroup(lNameOfResourceGroup);


		// Third,  setup surface color using material manager.
		// It often requires the object to have correct normals (see my manual object construction),
		// and some lights in the scene. need to check Ogre for Ogre::Technique
		{
			Ogre::MaterialPtr lMaterial = lMaterialManager.create("GroundTexture",lNameOfResourceGroup);
			Ogre::Technique* lFirstTechnique = lMaterial->getTechnique(0);
			Ogre::Pass* lFirstPass = lFirstTechnique->getPass(0);

			Ogre::TextureUnitState* lTextureUnit = lFirstPass->createTextureUnitState();
			lTextureUnit->setTextureName("Dirt.jpg", Ogre::TEX_TYPE_2D);
			lTextureUnit->setTextureCoordSet(0);
	
		}

		{
			Ogre::MaterialPtr lMaterial = lMaterialManager.create("M_SurfaceColor",lNameOfResourceGroup);
			Ogre::Technique* lFirstTechnique = lMaterial->getTechnique(0);
			Ogre::Pass* lFirstPass = lFirstTechnique->getPass(0);

			// Lighting is allowed on this pass.
			lFirstPass->setLightingEnabled(true);

			// Emissive / self illumination is the color 'produced' by the object.
			// Color values vary between 0.0(minimum) to 1.0 (maximum).
			//Ogre::ColourValue lSelfIllumnationColour(0.1f, 0.3f, 0.1f, 1.0f);
			//lFirstPass->setSelfIllumination(lSelfIllumnationColour);

			// diffuse color is the traditionnal color of the object surface.
			Ogre::ColourValue lDiffuseColour(0.2f, 0.9f, 0.2f, 1.0f);
			lFirstPass->setDiffuse(lDiffuseColour);

			// ambient colour is linked to ambient lighting to the surface.
			// If there is no ambient lighting, then this has no influence.
			// It the ambient lighting is at 1, then this colour is fully added.
			// This is often use to change the general feeling of a whole scene.
			Ogre::ColourValue lAmbientColour(0.1f, 0.4f, 0.1f, 1.0f);
			lFirstPass->setAmbient(lAmbientColour);

			// specular colour describes how a surface reflects light, generating specular highlight on the surface
			Ogre::ColourValue lSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
			lFirstPass->setSpecular(lSpecularColour);

			// Shininess is the 'inverse of specular color splattering' coefficient.
			// If this is big (e.g : 64) you get a very tiny dot with a quite strong color (on round surface).
			// If this is 0, you get a simple color layer (the dot has become very wide).
			Ogre::Real lShininess = 64.0f;
			lFirstPass->setShininess(lShininess);
			
		}

	}

	//	After the previous setup for the scene, we can now load the object and lit it with AmbientLight
	// load a character mesh
	Ogre::String  tempName = "Ninja"; 
	Ogre::Entity* lNinja = lScene->createEntity(tempName, "ninja.mesh");
    lNinja->setCastShadows(true);

	// create a scene node to hold all the mesh entities that we will compute physics later.
	Ogre::SceneNode* lEntityNode = lRootSceneNode->createChildSceneNode("EntityNode");
	
	// create a scene node to the entity scene node for our character
	m_ninjaNode = lEntityNode->createChildSceneNode(tempName.append("Node"));
	//attach the mesh
	m_ninjaNode->attachObject(lNinja);
	m_ninjaNode->setPosition(-12.0f, 0.0f, -10.0f);
	m_ninjaNode->scale(0.05f, 0.05f, 0.05f);
	const Ogre::String& lNajaMaterialName = "M_SurfaceColor";
	lNinja->setMaterialName(lNajaMaterialName);


	// add the second object
    tempName = "Cube"; 
	Ogre::Entity* lCube = lScene->createEntity(tempName, "cube.mesh");
    lCube->setCastShadows(true);

	// create a scene node to the entity scene node for our character
	Ogre::SceneNode* lCubeNode = lEntityNode->createChildSceneNode(tempName.append("Node"));
	//attach the mesh
	lCubeNode->attachObject(lCube);
	lCubeNode->setPosition(0.0f, 0.0f, 0.0f);
	lCubeNode->scale(0.05f, 0.05f, 0.05f);
	lCube->setMaterialName(lNajaMaterialName);

	// add to child scene node list
	this->addChildNodeToList(lEntityNode->getName(), lEntityNode);

	// make a floor to project shaow onto
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 50, 50, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
	
	tempName = "Floor";
	Ogre::Entity* lGround = lScene->createEntity(tempName, "ground");
    lGround->setCastShadows(false);
	Ogre::SceneNode* lFloorNode = lRootSceneNode->createChildSceneNode(tempName.append("Node"));
	lFloorNode->attachObject(lGround);
	const Ogre::String& lMaterialName = "GroundTexture";
	lGround->setMaterialName(lMaterialName);


	// add to child scene node list
	this->addChildNodeToList(lFloorNode->getName(), lFloorNode);

}

void OgreApplication::run(bool updateOption, bool synchroOption)
{
	lWindow->update(updateOption);
    lWindow->swapBuffers(synchroOption);
    lRoot->renderOneFrame();
}


void OgreApplication::createOIS()
{

	//The input/output system needs an handle to a window as input, in a string format. 
	//1/ Create a size_t object that we need to use to retrieve a window handle from our render window 
	size_t windowHandle = 0;
	lWindow->getCustomAttribute("WINDOW", &windowHandle);

	//2/convert it into a string
	std::string windowsHandleAsString="";
	{
		std::ostringstream windowHndStr;
		windowHndStr << windowHandle;
		windowsHandleAsString = windowHndStr.str();
	}

	// 3/ we translate it into a format accepted by OIS (the input library). 

	OIS::ParamList lSpecialParameters;
	lSpecialParameters.insert(std::make_pair(std::string("WINDOW"), windowsHandleAsString));

	// dont steel all the focus
	lSpecialParameters.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
    lSpecialParameters.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
    lSpecialParameters.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
    lSpecialParameters.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));

	//4/ we create the input/output system itself.
	lInputManager = OIS::InputManager::createInputSystem( lSpecialParameters );
	
	//the events can be stored or not in a buffer 
	bool lBufferedKeys = false;
	bool lBufferedMouse = false;

	//creation of the keyboard-representing object 
	lKeyboard = static_cast<OIS::Keyboard*>(lInputManager->createInputObject(OIS::OISKeyboard, lBufferedKeys));
	//creation of the mouse-representing object 
	lMouse = static_cast<OIS::Mouse*>(lInputManager->createInputObject( OIS::OISMouse, lBufferedMouse));
	//then must tell the mouse how much it is allowed to move. 
	unsigned int width, height, colourDepth;
    int top, left;
	lWindow->getMetrics(width, height, colourDepth, left, top);
	const OIS::MouseState &infoSouris = lMouse->getMouseState();
	infoSouris.width = width; 
	infoSouris.height = height; 
	

}

Ogre::Timer* OgreApplication::createTimer()
{
	lOgreTimer = lRoot->getTimer();

	return lOgreTimer;
}

void OgreApplication::resetTimer()
{
	lOgreTimer->reset();
}