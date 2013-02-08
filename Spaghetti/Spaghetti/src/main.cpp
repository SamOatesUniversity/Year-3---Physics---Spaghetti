#include "main.h"
#include "SceneLoading/CSceneManager.h"

/*
*	\brief Setup and run the ogre based application.
*/
void RunOgreApplication()
{
	OgreApplication *const application = new OgreApplication;

	// Create and initialise the application	
	if (application == nullptr || !application->Initialize())
	{
		std::cout << __FILE__ << " (" << __LINE__ << ") - " << "Failed to initialise the application" << std::endl; 
		system("PAUSE");
		return;
	}

	// create a camera, setup viewport 
	Ogre::String cameraName = "MainCamera";
	Ogre::SceneNode *const cameraNode = application->CreateCamera(cameraName);
	cameraNode->setPosition(Ogre::Vector3(0.0f, 200.0f, -2500.0f));
	static_cast<Ogre::Camera*>(cameraNode->getAttachedObject("MainCamera"))->lookAt(0.0f, 4.0f, 0.0f);

	//create O/I system for keyboard and mouse inputs
	application->CreateIOS();
	application->CreateScene();

	// create a timer to access system time information
	Ogre::Timer *const timer = application->CreateTimer();
    timer->reset();

	unsigned long lastTime = timer->getMilliseconds();
    float deltatTime = 0.0f;

	// get the keyboard and mouse to capture their input information
	OIS::Keyboard *const keyboard = application->GetKeyboard();
	OIS::Mouse *const mouse = application->GetMouse();
	
	// clear all events before we go into our main loop
	application->GetOgreWrapper().GetRoot()->clearEventTimes();

	CSceneManager *sceneManager = new CSceneManager();
	if (sceneManager->LoadScenes("scenes") == 0)
	{
		std::cout << __FILE__ << " (" << __LINE__ << ") - " << "Failed to load any .scene files, exiting" << std::endl; 
		system("PAUSE");
		return;
	}

	Ogre::Vector3 cameraPosition = cameraNode->getPosition();

	CSpaghetti *const spaghetti = new CSpaghetti();
	CSpaghettiWorld *const world = spaghetti->CreateWorld();
	world->SetPaused(true);

	// setup and create the current scene
	sceneManager->SetupCurrentScene(application, spaghetti, world);

	// Main game loop
	while (!application->GetOgreWrapper().GetWindow()->isClosed())
	{
		// calculate the delta time
		const unsigned long currentTime = timer->getMicroseconds();
		if (lastTime == 0) lastTime = currentTime;

		deltatTime = static_cast<float>(currentTime - lastTime);
		lastTime = currentTime;
		if (deltatTime == 0)
			continue;
		
		deltatTime = deltatTime / 1000000.0f;

		// capture the mouse and keyboard
		keyboard->capture();
		mouse->capture();

		// update the camera position
		cameraNode->setPosition(Ogre::Vector3(300.0f, 50.0f, 200.0f));
		static_cast<Ogre::Camera*>(cameraNode->getAttachedObject("MainCamera"))->lookAt(0.0f, 0.0f, 0.0f);
		
		static const float cameraSpeed = 10.0f;
		if (keyboard->isKeyDown(OIS::KC_A))
			cameraPosition.x-=cameraSpeed;
		else if (keyboard->isKeyDown(OIS::KC_D))
			cameraPosition.x+=cameraSpeed;
		else if (keyboard->isKeyDown(OIS::KC_W))
			cameraPosition.z-=cameraSpeed;
		else if (keyboard->isKeyDown(OIS::KC_S))
			cameraPosition.z+=cameraSpeed;

		cameraNode->setPosition(cameraPosition);
		static_cast<Ogre::Camera*>(cameraNode->getAttachedObject("MainCamera"))->lookAt(0.0f, 0.0f, 0.0f);

		// toggle the worlds pause state
		if (keyboard->isKeyDown(OIS::KC_RETURN))
		{
			world->SetPaused(!world->IsPaused());
			while (keyboard->isKeyDown(OIS::KC_RETURN)) keyboard->capture();
		}

		// check keys 1 - 9 and change the scene if pressed
		for (int keyIndex = OIS::KC_1; keyIndex <= OIS::KC_9; ++keyIndex)
		{
			OIS::KeyCode kc = (OIS::KeyCode)keyIndex;
			if (keyboard->isKeyDown(kc))
			{
				sceneManager->SetCurrentScene(keyIndex - 1, application, spaghetti, world);
				world->SetPaused(true);
				while (keyboard->isKeyDown(kc)) keyboard->capture();
			}
		}

		// update all our physics
		world->Update(deltatTime);

		// update the current scene
		sceneManager->Update();

		// update the application
		Ogre::WindowEventUtilities::messagePump();
		if (!application->GetOgreWrapper().GetWindow()->isClosed())
			application->Run(false, false);
	}

	// clean up
	delete sceneManager;
	delete application;
	delete spaghetti;
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


