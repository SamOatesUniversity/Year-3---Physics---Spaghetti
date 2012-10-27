#include "main.h"

/*
*	\brief Setup and run the ogre based application.
*/
void RunOgreApplication()
{
	// Create and initialise the application
	OgreApplication *const application = new OgreApplication;
	
	if (!application->Initialize())
	{
		std::cout << __FILE__ << " (" << __LINE__ << ") - " << "Failed to initialise the application" << std::endl; 
		return;
	}
	
	// create a camera, setup viewport 
	Ogre::String cameraName = "MainCamera";
	Ogre::SceneNode *const cameraNode = application->CreateCamera(cameraName);
	cameraNode->setPosition(Ogre::Vector3(20.0f, 5.0f, 20.0f));
	static_cast<Ogre::Camera*>(cameraNode->getAttachedObject("MainCamera"))->lookAt(0.0f, 0.0f, 0.0f);

	//create O/I system for keyboard and mouse inputs
	application->CreateIOS();
	application->CreateScene();

	// create a timer to access system time information
	Ogre::Timer *const timer = application->CreateTimer();
    timer->reset();

	unsigned long lastTime = timer->getMilliseconds();
    unsigned long deltatTime = 0;

	// get the keyboard and mouse to capture their input information
	OIS::Keyboard *const keyboard = application->GetKeyboard();
	OIS::Mouse *const mouse = application->GetMouse();
	
	// clear all events before we go into our main loop
	application->GetOgreWrapper().GetRoot()->clearEventTimes();

	// Main game loop
	while(!application->GetOgreWrapper().GetWindow()->isClosed())
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
		application->Run(false, true);
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


