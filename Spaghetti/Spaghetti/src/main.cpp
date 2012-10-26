#include "main.h"

/*
*	\brief Setup and run the ogre based application.
*/
void RunOgreApplication()
{
	// Create and initialise the application
	OgreApplication *const application = new OgreApplication;
	
	if (!application->init())
	{
		std::cout << __FILE__ << " (" << __LINE__ << ") - " << "Failed to initialise the application" << std::endl; 
		return;
	}
	
	// create a camera, setup viewport 
	Ogre::String cameraName = "MainCamera";
	application->createCamera(cameraName);
	Ogre::String nameOfChildNode = cameraName.append("Node");
	Ogre::SceneNode *const cameraNode = application->getChildSceneNodeByName(nameOfChildNode);

	//create O/I system for keyboard and mouse inputs
	application->createOIS();
	application->createScene();

	// create a timer to access system time information
	Ogre::Timer *const timer = application->createTimer();
    application->resetTimer();

	unsigned long lastTime = timer->getMilliseconds();
    unsigned long deltatTime = 0;

	// get the keyboard and mouse to capture their input information
	OIS::Keyboard *const keyboard = application->getKeyboard();
	OIS::Mouse *const mouse = application->getMouse();
	
	// Main game loop
	while(!application->getSimpleOgreInitHandle().mWindow->isClosed())
	{
		// calculate the delta time
		const unsigned long currentTime = timer->getMicroseconds();
		deltatTime = currentTime - lastTime;
		lastTime = currentTime;
		if (deltatTime == 0)
			continue;
		
		// capture the mouse and keyboard
		keyboard->capture();
		mouse->capture();

		// update all our physics
		//spaghetti->Update? or something magical

		// update the application
		application->run(false, true);
		Ogre::WindowEventUtilities::messagePump();
	}

	// clean up
	delete application;
}

/*
*	\brief Main entry point of the application
*/		
int main()
{
	HWND hwnd = GetConsoleWindow();
	ShowWindow(hwnd, SW_SHOW);
	RunOgreApplication();	
	return 0;
}


