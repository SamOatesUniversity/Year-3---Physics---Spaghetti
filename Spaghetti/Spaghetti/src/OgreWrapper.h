#ifndef _INCLUDE_OGRE_WRAPPER_
#define _INCLUDE_OGRE_WRAPPER_

// Required ogre header
#include "OGRE/OgreRoot.h"
#include "OGRE/OgreRenderSystem.h"
#include "OGRE/OgreRenderWindow.h"

// OgreWrapper class deceleration
class OgreWrapper
{
private:
	Ogre::Root						*m_root;									//!< 
	Ogre::RenderWindow				*m_window;									//!< 

private:
									//! Copy constructor
									OgreWrapper(
										const OgreWrapper &source				//!< 
									);

									//! Assignment operator
	OgreWrapper&					operator=(
										const OgreWrapper &source				//!< 
									);

public:
									//! Class constructor
									OgreWrapper();
	
									//! Class destructor
									~OgreWrapper();

									//! Initializes Ogre for use
	const bool						Initialize();

									//! Access to the ogre root
	Ogre::Root						*GetRoot() const
									{
										return m_root;
									}

									//! Access to the ogre window
	Ogre::RenderWindow				*GetWindow() const
									{
										return m_window;
									}
};

#endif

