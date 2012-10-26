

#include "main.h"


//global variables
bool bCamera = true;
OgreApplication  *application = NULL; 
Ogre::Real   velocity = 0.1f;


// I declare a function in which I will make my whole application.
// This is easy then to add more things later in that function.
// The main will call this function and take care of the global try/catch.
void AnOgreApplication()
{
	
	//----------------- Setup Application ------------------//
	application = new OgreApplication;

	// init application, setup scene graph
	if(!application->init())
	{
		std::cout<<"Impossible to init application correctly."<<std::endl;

		return;
	}
	
	// create a camera, setup viewport 
	Ogre::String  nameOfCamera ="MyCamera";
	application->createCamera(nameOfCamera);

	// add a viewport grid
	//application->addViewportGrid();

	//create O/I system for keyboard and mouse inputs
	application->createOIS();
	application->createScene();

	// create a timer to access system time information
	Ogre::Timer* lTimer = application->createTimer();
    application->resetTimer();

	unsigned long lLastTime_Ms = lTimer->getMilliseconds();
    unsigned long lDeltaTime_Ms = 0;
    float lDeltaTime_s = 0.0f;


	// get the keyboard and mouse to capture their input information
	OIS::Keyboard  *keyboard = application->getKeyboard();
	OIS::Mouse     *mouse = application->getMouse();

	//get child scene nodes that we want to animate
	// 1. we want to make camera move with keyboard and mouse input,
	//    so get the camera's scene node
	Ogre::String  nameOfChildNode = nameOfCamera.append("Node");
	Ogre::SceneNode *cameraNode = application->getChildSceneNodeByName(nameOfChildNode);

	// 2. we want to make the charater move
	Ogre::String nameOfSceneNode = "EntityNode";
    Ogre::SceneNode *objectSceneNode = application->getChildSceneNodeByName(nameOfSceneNode);
	Ogre::Node* objectNode= objectSceneNode->getChild("NinjaNode");
	
	//----------------- End of Setup Application ------------------//

    // enter the game loop.................
	while(!application->getSimpleOgreInitHandle().mWindow->isClosed())
	{

		//I evaluate the current time and the time elapsed since last frame 
		//I also prepare the next iteration. 

		{
			unsigned long lCurrentTime_Ms = lTimer->getMilliseconds();
			lDeltaTime_Ms = lCurrentTime_Ms - lLastTime_Ms;
			if(lDeltaTime_Ms == 0)
			{
			continue;
			}
			lLastTime_Ms = lCurrentTime_Ms;
			lDeltaTime_s = 0.001f * float(lDeltaTime_Ms);
		}


		// get the keyboard to capture its input information
		// process keyboard inputs with smooth movement
		keyboard->capture();

		{
			float lCoeff = 200.0f * lDeltaTime_s;
			Ogre::Vector3 lTranslation(Ogre::Vector3::ZERO);

			if(keyboard->isKeyDown(OIS::KC_T))
			{
				lTranslation.z -= lCoeff;
			}
			if(keyboard->isKeyDown(OIS::KC_G))
			{
				lTranslation.z += lCoeff;
			}
			if(keyboard->isKeyDown(OIS::KC_F))
			{
				lTranslation.x -= lCoeff;
			}
			if(keyboard->isKeyDown(OIS::KC_H))
			{
				lTranslation.x += lCoeff;
			}
			if(lTranslation != Ogre::Vector3::ZERO)
			{
				
				cameraNode->translate(lTranslation, Ogre::Node::TS_LOCAL);
			}
			if(keyboard->isKeyDown(OIS::KC_SPACE))
			{
				bCamera = !bCamera;
			}
			if(keyboard->isKeyDown(OIS::KC_ESCAPE))
			{
				break;
			}
		}
			
		// same for the mouse
		 mouse->capture();

		 {
			const OIS::MouseState& lMouseState = mouse->getMouseState();
			
			if(bCamera)
			{
				float lMouseX = float(lMouseState.X.rel) / float(application->getSimpleOgreInitHandle().mWindow->getWidth());
				float lMouseY = float(lMouseState.Y.rel) / float(application->getSimpleOgreInitHandle().mWindow->getHeight());
				float lRotCoeff = -5.0f;
				Ogre::Radian lAngleX(lMouseX * lRotCoeff);
				Ogre::Radian lAngleY(lMouseY * lRotCoeff);
				//If the 'player' don't make loopings, 'yaw in world' + 'pitch in local' is often enough for a camera controler. 

				cameraNode->yaw(lAngleX, Ogre::Node::TS_WORLD);
				cameraNode->pitch(lAngleY, Ogre::Node::TS_LOCAL);
			}
		}
		 

		 //----------- Update Physics Computation-------------------//  
		 // simply moving the character 	
		 
		 PhysicsComputation(objectNode);
		 
		//---------------- End of Update Physics -------------------//


		//----------------------------------------------------------------------//
		// Update window false
		bool updateWin =false;
		bool lVerticalSynchro = true;
		application->run(updateWin, lVerticalSynchro);
		
		Ogre::WindowEventUtilities::messagePump();
	}

	// clean up
	delete application;
}

void PhysicsComputation(Ogre::Node* objectNode)
{

	Ogre::Vector3 pos = objectNode->getPosition();
	
		if(pos.x <-12.0f )
		{
			velocity = 0.1f;
			objectNode->setOrientation(-1.0, 0.0, 1.0, 0.0);
		}
		if(pos.x>=12.0f)
		{
			velocity = -0.1f;
			objectNode->setOrientation(1.0, 0.0, 1.0, 0.0);
		}
		
		objectNode->setPosition(pos.x + velocity, 0.0, -5.0f);

}
		
int main()
{
	//show - hide console window
	HWND hwnd = GetConsoleWindow();
	ShowWindow(hwnd, 1);

	
	try
	{

		AnOgreApplication();
		std::cout<<"end of the program"<<std::endl;
	}catch(Ogre::Exception &e)
	{
		MWARNING("!!!!Ogre::Exception!!!!\n"<<e.what());
	}catch(std::exception &e)
	{
		MWARNING("!!!!std::exception!!!!\n"<<e.what());
	}


	return 0;
}


