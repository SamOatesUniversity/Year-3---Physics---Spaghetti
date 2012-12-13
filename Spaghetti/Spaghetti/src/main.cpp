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

	CSpaghetti *const spaghetti = new CSpaghetti();
	CSpaghettiWorld *const world = spaghetti->CreateWorld();

	// create the static floor
	CSpaghettiRigidBody *floorBody = nullptr;
	if (Ogre::SceneNode *const floorNode = application->CreateEntityFromMesh("cube.mesh", "floorNode")) 
	{
		floorNode->setScale(0.5f, 0.01f, 0.5f);
		floorNode->showBoundingBox(true);

		floorBody = spaghetti->CreateRigidBody(floorNode, world);
		floorBody->SetIsStatic(true);

		Ogre::Entity *const meshEntity = application->GetSceneManager()->getEntity("cube");
		Ogre::AxisAlignedBox meshBoundingBox = meshEntity->getBoundingBox();

		SAM::TVector<float, 3> boundingBoxCorners[NOOF_BOUNDINGBOX_CORNERS];
		const Ogre::Vector3 *const boundCorners = meshBoundingBox.getAllCorners();

		const Ogre::Vector3 entityScale = floorNode->getScale();
		for (int corner = 0; corner < NOOF_BOUNDINGBOX_CORNERS; ++corner)
		{
			const Ogre::Vector3 currenCorner = boundCorners[corner];
			boundingBoxCorners[corner].Set(
				currenCorner.x * entityScale.x, 
				currenCorner.y * entityScale.y, 
				currenCorner.z * entityScale.z
			);
		}

		CSpaghettiBoundingBox boundingBox;
		boundingBox.SetCorners(boundingBoxCorners);
		floorBody->SetBoundingBox(boundingBox);
	}

	// create some things to bounce around
	static const int noofBoxes = 4;
	CSpaghettiRigidBody* box[noofBoxes];
	for (int boxIndex = 0; boxIndex < noofBoxes; ++boxIndex)
	{
		std::stringstream nodeName;
		nodeName << "CubeNode-" << boxIndex;

		if (Ogre::SceneNode *const cubeNode = application->CreateEntityFromMesh("ogrehead.mesh", nodeName.str())) 
		{
			static const float meshScale = 0.05f;
			cubeNode->setScale(meshScale, meshScale, meshScale);
			cubeNode->showBoundingBox(true);

			box[boxIndex] = spaghetti->CreateRigidBody(cubeNode, world);
			box[boxIndex]->SetPosition(0.0f, 10.0f + (boxIndex * 15.0f), 0.0f);

			Ogre::Entity *const meshEntity = application->GetSceneManager()->getEntity("ogrehead");
			Ogre::AxisAlignedBox meshBoundingBox = meshEntity->getBoundingBox();

			SAM::TVector<float, 3> boundingBoxCorners[NOOF_BOUNDINGBOX_CORNERS];
			const Ogre::Vector3 *const boundCorners = meshBoundingBox.getAllCorners();
			
			for (int corner = 0; corner < NOOF_BOUNDINGBOX_CORNERS; ++corner)
			{
				const Ogre::Vector3 currenCorner = boundCorners[corner];
				boundingBoxCorners[corner].Set(currenCorner.x, currenCorner.y, currenCorner.z);
				boundingBoxCorners[corner] = boundingBoxCorners[corner] * meshScale;
			}

			CSpaghettiBoundingBox boundingBox;
			boundingBox.SetCorners(boundingBoxCorners);
			box[boxIndex]->SetBoundingBox(boundingBox);

			SAM::TVector<float, 3> velocity;
			velocity.Set(0.0025f, 0.0f, 0.0025f);
			box[boxIndex]->SetVelocity(velocity);
		}
	}

	// Main game loop
	while (!application->GetOgreWrapper().GetWindow()->isClosed())
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
		world->Update(deltatTime);

		SAM::TVector<float, 3> floorPosition = floorBody->GetPosition();
		static_cast<Ogre::SceneNode*>(floorBody->GetRenderObject())->setPosition(floorPosition.X(), floorPosition.Y(), floorPosition.Z());

		// update the graphical representations of the physics
		for (int boxIndex = 0; boxIndex < noofBoxes; ++boxIndex)
		{
			SAM::TVector<float, 3> position = box[boxIndex]->GetPosition();
			static_cast<Ogre::SceneNode*>(box[boxIndex]->GetRenderObject())->setPosition(position.X(), position.Y(), position.Z());
		}

		// update the application
		Ogre::WindowEventUtilities::messagePump();
		if (!application->GetOgreWrapper().GetWindow()->isClosed())
			application->Run(false, true);
	}

	// clean up
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


