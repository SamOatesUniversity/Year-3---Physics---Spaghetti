#include "OgreApplication.h"

/*
*	\brief Class constructor
*/
OgreApplication::OgreApplication(void) : 
	m_scene(NULL),		m_rootSceneNode(NULL), 
	m_vp(NULL),			m_inputManager(NULL), 
	m_mouse(NULL),		m_keyboard(NULL), 
	m_timer(NULL)
{
}

/*
*	\brief Class destructor
*/
OgreApplication::~OgreApplication(void)
{
	Release();
}

/*
*	\brief Free all allocated resources
*/
void OgreApplication::Release()
{
	m_scene->destroyAllCameras();
	m_scene->destroyAllLights();
	m_scene->destroyAllManualObjects();
	m_scene->destroyAllEntities();
	m_rootSceneNode->removeAndDestroyAllChildren();   
}

/*
*	\brief Free all allocated resources
*/
const bool OgreApplication::Initialize()
{
	
	if(!m_ogreWrapper.Initialize())
	{
		std::cout << __FILE__ << " (" << __LINE__ << ") - " << "Failed to initialise the application" << std::endl; 
		return false;
	}
	
	Ogre::Root *const root = m_ogreWrapper.GetRoot();
	Ogre::RenderWindow *const window = m_ogreWrapper.GetWindow();
	m_scene = root->createSceneManager(Ogre::ST_GENERIC, "SceneManager");
	m_rootSceneNode = m_scene->getRootSceneNode();
	
	return true;
}

/*
*	\brief Create a camera and attach it to a scene node
*/
Ogre::SceneNode *OgreApplication::CreateCamera(
		Ogre::String name								//!< A name to give to the camera node
	)
{
	// Create a camera using SceneManager. It represents a 'point of view' in the scene.
	Ogre::Camera *const camera = m_scene->createCamera(name);

	// Create a child scene node of the RootSceneNode,
	// Attach the camera to a new SceneNode. It will be easier then to move it in the scene.
	m_camera = m_rootSceneNode->createChildSceneNode(name.append("Node"));
	m_camera->attachObject(camera);

	// Setup viewport parameters 
	// Add viewport to the window with the camera 
	const float viewportWidth = 1.0f;
	const float viewportHeight = 1.0f;
	const float viewportLeft = (1.0f - viewportWidth) * 0.5f;
	const float viewportTop = (1.0f - viewportHeight) * 0.5f;
	const unsigned short viewportZOrder = 100;
	
	Ogre::RenderWindow *const window = m_ogreWrapper.GetWindow();
	m_vp = window->addViewport(camera, viewportZOrder, viewportLeft, viewportTop, viewportWidth, viewportHeight);

	// Set the viewport to draw the scene automatically
	// whenever the window is updated
	m_vp->setAutoUpdated(true);

	// Choose a color for this viewport. 
	// Have a bright colour for now
	m_vp->setBackgroundColour(Ogre::ColourValue(0.2f, 0.2f, 0.3f));

	// Setup the visual ratio of the camera. To make it looks real, same as the viewport.
	camera->setAspectRatio(static_cast<float>(m_vp->getActualWidth()) / static_cast<float>(m_vp->getActualHeight()));
	camera->setNearClipDistance(0.1f);
	camera->setFarClipDistance(3000.0f); 

	AddNodeToList(m_camera->getName(), m_camera);
	
	// Set the window to be active
	window->setActive(true);

	// Update the content of the window not automatically.
	window->setAutoUpdated(false);

	return m_camera;
}

/*
*	\brief Create the lights in the scene
*/
void OgreApplication::CreateLights()
{
	// Create a new SceneNode - light Scene Node
	// Create a light in the SceneManager, and set the type of the light: {LT_POINT = 0, LT_DIRECTIONAL = 1, LT_SPOTLIGHT = 2 }

	// create a directional light
	Ogre::SceneNode* lLightSceneNodeDir = NULL;
	Ogre::String tempName = "DirectionLight";
	Ogre::Light* lDirectionalLight = m_scene->createLight(tempName);
	lDirectionalLight->setType(Ogre::Light::LT_DIRECTIONAL);

	//Set light clolur to white
	lDirectionalLight->setDiffuseColour(0.8f, 0.8f, 0.8f); 
	lDirectionalLight->setSpecularColour(1.0f, 1.0f, 1.0f);

	//Set light node as the child node of the RootSceneNode
	//Attach the light to the chid node
	lLightSceneNodeDir = m_rootSceneNode->createChildSceneNode(tempName.append("Node"));
	lLightSceneNodeDir->attachObject(lDirectionalLight);
	//Set light position : rember must set the light position after attached it to SceneNode
	lLightSceneNodeDir->setDirection(1.0f, 1.0f, -1.0f);

	// add directional light to the child node list
	AddNodeToList(lLightSceneNodeDir->getName(), lLightSceneNodeDir);

	// create a spot light
	Ogre::SceneNode* lLightSceneNodeSpot = NULL;
	tempName = "SpotLight";
	Ogre::Light* lSpotLight = m_scene->createLight(tempName);
	lSpotLight->setType(Ogre::Light::LT_SPOTLIGHT);

	//Set light clolur to red
	lSpotLight->setDiffuseColour(0.0f, 0.8f, 0.8f); 
	lSpotLight->setSpecularColour(0.0f, 1.0f, 1.0f);

	//Set light node as the child node of the RootSceneNode
	//Attach the light to the chid node
	lLightSceneNodeSpot = m_rootSceneNode->createChildSceneNode(tempName.append("Node"));
	lLightSceneNodeSpot->attachObject(lSpotLight);
	lLightSceneNodeSpot->setDirection(1.0f, 1.0f, 0.0f);
	lLightSceneNodeSpot->setPosition(Ogre::Vector3(50.0, 80.0, 0.0));
	//set the spot light corn beam width
	lSpotLight->setSpotlightRange(Ogre::Degree(20), Ogre::Degree(50));

	// add to child node list
	AddNodeToList(lLightSceneNodeSpot->getName(), lLightSceneNodeSpot);

	// tell the SceneManager where the mesh objects are stored and prepare the sence for load and render entities       //
	// First, add an ambient color to lit the scene. The ambient color is managed in the scenemanager.
	// If you want to learn more about ambient/specular/diffuse color, check the 'basic material tutorial'.
	Ogre::ColourValue lAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	m_scene->setAmbientLight(lAmbientColour);

	// There are three shadow techniques: Ogre::SHADOWTYPE_TEXTURE_MODULATIVE - least expensive and accurate,
	// Ogre::SHADOWTYPE_STENCIL_MODULATIVE - less expensive and accurate,  Ogre::SHADOWTYPE_STENCIL_ADDITIVE -most expensive and accurate
	m_scene->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
}

/*
*	\brief Create the ogre scene
*/
void OgreApplication::CreateScene()
{
	CreateLights();

	// 2. Secondly, get a reference on the material manager, which is a singleton. 
	Ogre::MaterialManager& materialManager = Ogre::MaterialManager::getSingleton();

	const Ogre::String resourceGroupName = "Physics Simulation";

	// Create a resource group manager and create the resource group
	Ogre::ResourceGroupManager& lRgMgr = Ogre::ResourceGroupManager::getSingleton();
	lRgMgr.createResourceGroup(resourceGroupName);

	bool lIsRecursive = false;

	// Assign directories for meshes, and textures, and add to the resource group "Physics Simulation"
	lRgMgr.addResourceLocation("../../media/meshes", "FileSystem", resourceGroupName, lIsRecursive);
	lRgMgr.addResourceLocation("../../media/textures", "FileSystem", resourceGroupName, lIsRecursive);

	// The function 'initialiseResourceGroup' parses scripts if any in the locations.
	lRgMgr.initialiseResourceGroup(resourceGroupName);

	// Files that can be loaded are loaded.
	lRgMgr.loadResourceGroup(resourceGroupName);
	
	// Third,  setup surface color using material manager.
	// It often requires the object to have correct normals (see my manual object construction),
	// and some lights in the scene. need to check Ogre for Ogre::Technique
	{
		Ogre::MaterialPtr lMaterial = materialManager.create("GroundTexture",resourceGroupName);
		Ogre::Technique* lFirstTechnique = lMaterial->getTechnique(0);
		Ogre::Pass* lFirstPass = lFirstTechnique->getPass(0);

		Ogre::TextureUnitState* lTextureUnit = lFirstPass->createTextureUnitState();
		lTextureUnit->setTextureName("Dirt.jpg", Ogre::TEX_TYPE_2D);
		lTextureUnit->setTextureCoordSet(0);

	}

	{
		Ogre::MaterialPtr lMaterial = materialManager.create("M_SurfaceColor",resourceGroupName);
		Ogre::Technique* lFirstTechnique = lMaterial->getTechnique(0);
		Ogre::Pass* lFirstPass = lFirstTechnique->getPass(0);

		// Lighting is allowed on this pass.
		lFirstPass->setLightingEnabled(true);

		// Emissive / self illumination is the color 'produced' by the object.
		// Color values vary between 0.0(minimum) to 1.0 (maximum).
		//Ogre::ColourValue lSelfIllumnationColour(0.1f, 0.3f, 0.1f, 1.0f);
		//lFirstPass->setSelfIllumination(lSelfIllumnationColour);

		// diffuse color is the traditionnal color of the object surface.
		Ogre::ColourValue lDiffuseColour(0.7f, 0.0f, 0.0f, 1.0f);
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

	// add the second object
	Ogre::String cubeName = "Cube"; 
	Ogre::Entity* lCube = m_scene->createEntity(cubeName, "cube.mesh");
	lCube->setCastShadows(true);

	// create a scene node to the entity scene node for our character
	Ogre::SceneNode* lCubeNode = m_rootSceneNode->createChildSceneNode(cubeName.append("Node"));
	//attach the mesh
	lCubeNode->attachObject(lCube);
	lCubeNode->setPosition(0.0f, 10.0f, 0.0f);
	lCubeNode->scale(0.05f, 0.05f, 0.05f);
	lCube->setMaterialName("M_SurfaceColor");

	// add to child scene node list
	AddNodeToList(lCubeNode->getName(), lCubeNode);

	// make a floor to project shadow onto
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"ground", 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane, 
		50, 50, 20, 20, 
		true, 
		1, 5, 5, 
		Ogre::Vector3::UNIT_Z
	);

	Ogre::String floorName = "Floor";
	Ogre::Entity* lGround = m_scene->createEntity(floorName, "ground");
	lGround->setCastShadows(false);
	Ogre::SceneNode* lFloorNode = m_rootSceneNode->createChildSceneNode(floorName.append("Node"));
	lFloorNode->attachObject(lGround);
	lGround->setMaterialName("GroundTexture");

	// add to child scene node list
	AddNodeToList(lFloorNode->getName(), lFloorNode);
}

/*
*	\brief Add a new node to our node list
*/
void OgreApplication::AddNodeToList(
		const Ogre::String name,					//!< Name of the node to add to the list
		Ogre::SceneNode *const node					//!< Pointer to the node to add to the list
	)
{
	SceneNodeAndName newSceneNode;
	newSceneNode.name = name;
	newSceneNode.node = node;
	m_sceneNodes.push_back(newSceneNode);
}

/*
*	\brief The main run loop
*/
void OgreApplication::Run(
		const bool updateOption,					//!< 
		const bool synchroOption					//!< 
	)
{
	Ogre::RenderWindow *const window = m_ogreWrapper.GetWindow();
	Ogre::Root *const root = m_ogreWrapper.GetRoot();

	window->update(updateOption);
    window->swapBuffers(synchroOption);
    root->renderOneFrame();
}

/*
*	\brief Create the input output system
*/
void OgreApplication::CreateIOS()
{
	Ogre::RenderWindow *const window = m_ogreWrapper.GetWindow();

	//The input/output system needs an handle to a window as input, in a string format. 
	//1/ Create a size_t object that we need to use to retrieve a window handle from our render window 
	size_t windowHandle = 0;
	window->getCustomAttribute("WINDOW", &windowHandle);

	//2/convert it into a string
	std::ostringstream windowHndStr;
	windowHndStr << windowHandle;
	const std::string windowsHandleAsString = windowHndStr.str();

	// 3/ we translate it into a format accepted by OIS (the input library). 
	OIS::ParamList lSpecialParameters;
	lSpecialParameters.insert(std::make_pair(std::string("WINDOW"), windowsHandleAsString));

	// don't steel all the focus
	lSpecialParameters.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
    lSpecialParameters.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
    lSpecialParameters.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
    lSpecialParameters.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));

	//4/ we create the input/output system itself.
	m_inputManager = OIS::InputManager::createInputSystem( lSpecialParameters );
	
	//creation of the keyboard-representing object 
	const bool bufferedKeys = false;
	m_keyboard = static_cast<OIS::Keyboard*>(m_inputManager->createInputObject(OIS::OISKeyboard, bufferedKeys));
	
	//creation of the mouse-representing object 
	const bool bufferedMouse = false;
	m_mouse = static_cast<OIS::Mouse*>(m_inputManager->createInputObject( OIS::OISMouse, bufferedMouse));
	
	//then must tell the mouse how much it is allowed to move. 
	int top, left;
	unsigned int width, height, colourDepth;
    
	window->getMetrics(width, height, colourDepth, left, top);
	const OIS::MouseState &infoSouris = m_mouse->getMouseState();
	infoSouris.width = width; 
	infoSouris.height = height;
}

/*
*	\brief Create a timer
*/
Ogre::Timer* OgreApplication::CreateTimer()
{
	m_timer = m_ogreWrapper.GetRoot()->getTimer();
	return m_timer;
}
