#include "main.h"

/*
*	\brief Setup and run the ogre based application.
*/
void RunOgreApplication()
{
	// Create and initialise the application
	OgreApplication *const application = new OgreApplication;
	
	if (application == nullptr || !application->Initialize())
	{
		std::cout << __FILE__ << " (" << __LINE__ << ") - " << "Failed to initialise the application" << std::endl; 
		return;
	}

	// create a camera, setup viewport 
	Ogre::String cameraName = "MainCamera";
	Ogre::SceneNode *const cameraNode = application->CreateCamera(cameraName);
	cameraNode->setPosition(Ogre::Vector3(1500.0f, 500.0f, 00.0f));
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

	CSpaghetti *const spaghetti = new CSpaghetti();
	CSpaghettiWorld *const world = spaghetti->CreateWorld();

	// create some things to bounce around
	static const int noofBoxes = 10;
	CSpaghettiRigidBody* box[noofBoxes];
	for (int boxIndex = 0; boxIndex < noofBoxes; ++boxIndex)
	{
		std::stringstream nodeName;
		nodeName << "CubeNode-" << boxIndex;

		if (Ogre::SceneNode *const cubeNode = application->CreateEntityFromMesh("cube.mesh", nodeName.str())) 
		{
			static const float meshScale = 1.0f;
			cubeNode->setScale(meshScale, meshScale, meshScale);

			box[boxIndex] = spaghetti->CreateRigidBody(cubeNode, world, RigidBodyType::Box);
			box[boxIndex]->SetPosition(150.0f, (boxIndex + 1) * (150.0f + (boxIndex * 25.0f)), 150.0f);

			Ogre::Entity *const meshEntity = application->GetSceneManager()->getEntity("cube");
			Ogre::AxisAlignedBox meshBoundingBox = meshEntity->getBoundingBox();

			SAM::TVector<float, 3> boundingBoxCorners[NOOF_BOUNDINGBOX_CORNERS];
			const Ogre::Vector3 *const boundCorners = meshBoundingBox.getAllCorners();

			for (int corner = 0; corner < NOOF_BOUNDINGBOX_CORNERS; ++corner)
			{
				const Ogre::Vector3 currenCorner = boundCorners[corner];
				boundingBoxCorners[corner].Set(
					currenCorner.x * meshScale, 
					currenCorner.y * meshScale, 
					currenCorner.z * meshScale
					);
			}

			CSpaghettiBoundsBox *const boundingBox = new CSpaghettiBoundsBox();
			boundingBox->SetCorners(boundingBoxCorners);
			box[boxIndex]->SetBounds(boundingBox);
		}
	}
	
	// create the static floor
	CSpaghettiRigidBody *floorBody = nullptr;
	if (Ogre::SceneNode *const floorNode = application->CreateEntityFromMesh("cube.mesh", "floorNode")) 
	{
		floorNode->setScale(20.0f, 0.1f, 20.0f);

		floorBody = spaghetti->CreateRigidBody(floorNode, world, RigidBodyType::Box);
		floorBody->SetPosition(0.0f, -5.0f, 0.0f);
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

		CSpaghettiBoundsBox *const boundingBox = new CSpaghettiBoundsBox();
		boundingBox->SetCorners(boundingBoxCorners);
		floorBody->SetBounds(boundingBox);
	}

	Ogre::Vector3 cameraPosition = cameraNode->getPosition();

	// Main game loop
	while (!application->GetOgreWrapper().GetWindow()->isClosed())
	{
		// calculate the delta time
		const unsigned long currentTime = timer->getMicroseconds();
		if (lastTime == 0) lastTime = currentTime;

		deltatTime = (currentTime - lastTime);
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
		
		if (keyboard->isKeyDown(OIS::KeyCode::KC_A))
			cameraPosition.x--;
		else if (keyboard->isKeyDown(OIS::KeyCode::KC_D))
			cameraPosition.x++;
		else if (keyboard->isKeyDown(OIS::KeyCode::KC_W))
			cameraPosition.z--;
		else if (keyboard->isKeyDown(OIS::KeyCode::KC_S))
			cameraPosition.z++;

		cameraNode->setPosition(cameraPosition);
		static_cast<Ogre::Camera*>(cameraNode->getAttachedObject("MainCamera"))->lookAt(0.0f, 0.0f, 0.0f);

		// update all our physics
		world->Update(deltatTime);

		SAM::TVector<float, 3> floorPosition = floorBody->GetPosition();
		static_cast<Ogre::SceneNode*>(floorBody->GetRenderObject())->setPosition(floorPosition.X(), floorPosition.Y(), floorPosition.Z());

		// update the graphical representations of the physics
		for (int boxIndex = 0; boxIndex < noofBoxes; ++boxIndex)
		{
			Ogre::SceneNode *const node = static_cast<Ogre::SceneNode*>(box[boxIndex]->GetRenderObject());
			SAM::TVector<float, 3> position = box[boxIndex]->GetPosition();
			SAM::TQuaternion orientation = box[boxIndex]->GetOrientation();

			node->setPosition(position.X(), position.Y(), position.Z());
			node->setOrientation(orientation.W(), orientation.X(), orientation.Y(), orientation.Z());
		}

		// update the application
		Ogre::WindowEventUtilities::messagePump();
		if (!application->GetOgreWrapper().GetWindow()->isClosed())
			application->Run(false, false);
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


