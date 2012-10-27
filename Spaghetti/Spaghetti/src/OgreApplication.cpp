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
Ogre::SceneNode *OgreApplication::CreateCamera(Ogre::String name)
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

}

/*
*	\brief Create the ogre scene
*/
void OgreApplication::CreateScene()
{

}

/*
*	\brief Add a new node to our node list
*/
void OgreApplication::AddNodeToList(
		const Ogre::String name, 
		Ogre::SceneNode *const node
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
		const bool updateOption, 
		const bool synchroOption
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

	// dont steel all the focus
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
