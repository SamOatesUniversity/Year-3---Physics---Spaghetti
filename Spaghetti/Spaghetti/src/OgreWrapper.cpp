#include "OgreWrapper.h"

/*
*	\brief Class constructor
*/
OgreWrapper::OgreWrapper() : 
	m_root(NULL), 
	m_window(NULL)
{
}

/*
*	\brief Class destructor
*/
OgreWrapper::~OgreWrapper()
{
}

/*
*	\brief Initializes Ogre for use
*/
const bool OgreWrapper::Initialize()
{
	const Ogre::String configFileName = "";
	const Ogre::String pluginsFileName = "";
	const Ogre::String logFileName = "Ogre.log";

	// Create our ogre root
	m_root = new Ogre::Root(configFileName, pluginsFileName, logFileName);

	// Load the plugins we want to use with ogre
	std::vector<Ogre::String> plugins;
	plugins.push_back("RenderSystem_GL");

	for (std::vector<Ogre::String>::iterator it = plugins.begin(); it != plugins.end(); ++it)
	{
		Ogre::String &pluginName = (*it);
#ifdef OGRE_DEBUG_MODE
		pluginName.append("_d");
#endif
		m_root->loadPlugin(pluginName);
	}

	// Get the list of render drivers available
	const Ogre::RenderSystemList& renderDrivers = m_root->getAvailableRenderers();
	if (renderDrivers.size() == 0)
		return false;

	// just grab the first render driver (should be openGL)
	m_root->setRenderSystem(renderDrivers.at(0));

	// initialize the root, but don't create a window by default
	m_root->initialise(false, "", "");

	// setup some window variables
	const Ogre::String windowTitle = "Sam Oates - Spaghetti Physics";
	const unsigned int width = 800;
	const unsigned int height = 600;
	Ogre::NameValuePairList windowParameters;

	// create our window
	m_window = m_root->createRenderWindow(windowTitle, width, height, false, &windowParameters);

	return true;
}
