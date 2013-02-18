#include "main.h"

#include "Camera.h"
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
	cameraNode->setPosition(Ogre::Vector3(0.0f, 4.0f, -2500.0f));
	static_cast<Ogre::Camera*>(cameraNode->getAttachedObject("MainCamera"))->lookAt(0.0f, 4.0f, 0.0f);

	CCamera *const camera = new CCamera(cameraNode, static_cast<Ogre::Camera*>(cameraNode->getAttachedObject("MainCamera")));

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

	Ogre::RaySceneQuery* g_RayScnQuery = application->GetSceneManager()->createRayQuery(Ogre::Ray());

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
		
		deltatTime = deltatTime * 0.00001f;

		// capture the mouse and keyboard
		keyboard->capture();
		mouse->capture();

		// update the camera position
		camera->Update(keyboard, mouse);

		OIS::MouseState ms = mouse->getMouseState();

		if (ms.buttons != 0)
		{
			Ogre::Ray mouseRay = static_cast<Ogre::Camera*>(cameraNode->getAttachedObject("MainCamera"))->getCameraToViewportRay((float)ms.X.abs / (float)ms.width, (float)ms.Y.abs / (float)ms.height);
			g_RayScnQuery->setRay(mouseRay);

			Ogre::RaySceneQueryResult& result = g_RayScnQuery->execute();
			if (result.size() == 0 && ms.buttons == 2)
			{
				sceneManager->SetPickedNode(nullptr);
			}
			else
			{
				Ogre::RaySceneQueryResult::iterator iter = result.begin();
				Ogre::RaySceneQueryResult::const_iterator pickEnd = result.end();

				for (iter; iter != pickEnd; ++iter)
				{
					// don't select debug objects
					if ((*iter).movable->getName() == "debug_object")
						continue;

					// if we have a valid scene node, tell the scene manager
					if (Ogre::SceneNode *pickedNode = (*iter).movable->getParentSceneNode())
						sceneManager->SetPickedNode(pickedNode);
				}
			}
		}

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

		if (keyboard->isKeyDown(OIS::KC_ESCAPE))
			break;

		// update all our physics
		world->Update(deltatTime);

		std::vector<CCollision> collisions = world->GetLastUpdateCollisions();
		for (unsigned int collisionIndex = 0; collisionIndex < collisions.size(); ++collisionIndex)
		{
			Ogre::Vector3 collisionPoint = collisions[collisionIndex].collisionPoint;
			Ogre::Vector3 collisionNormal = collisions[collisionIndex].collisionNormal;

			DebugDrawer::getSingleton().drawLine(collisionPoint, collisionPoint + (collisionNormal * 500.0f), Ogre::ColourValue::White);
		}		

		// update the current scene
		sceneManager->Update(world);

		// update the application
		Ogre::WindowEventUtilities::messagePump();
		if (!application->GetOgreWrapper().GetWindow()->isClosed())
			application->Run(false, false);
	}

	application->GetSceneManager()->destroyQuery(g_RayScnQuery);

	// clean up
	delete camera;
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


