#include "main.h"


// I declare a function in which I will make my whole application.
// This is easy then to add more things later in that function.
// The main will call this function and take care of the global try/catch.
void AnOgreApplication()
{
	
	// I construct my object that will allow me to initialise Ogre easily.
	OgreEasy::SimpleOgreInit lOgreInit;

	if(!lOgreInit.initOgre())
	{
		std::cout<<"Impossible to init Ogre correctly."<<std::endl;
		return;
	}

	
	// I wait until the window is closed.
	// The "message pump" thing is something you will see in most GUI application.
	// It allow the binding of messages between the application and the OS.
	// These messages are most of the time : keystroke, mouse moved, ... or window closed.
	// If I don't do this, the message are never caught, and the window won't close.
	while(!lOgreInit.mWindow->isClosed())
	{

		Ogre::WindowEventUtilities::messagePump();
	}


	return;
}

// The entry point to the Orge application 
// It calls AnOgreApplication() to setup scene
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


