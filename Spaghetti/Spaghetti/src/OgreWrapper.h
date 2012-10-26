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
	Ogre::Root						*m_root;									//!< The root ogre object
	Ogre::RenderWindow				*m_window;									//!< The ogre window object

private:
									//! Copy constructor
									OgreWrapper(
										const OgreWrapper &source				//!< The source instance of the class to copy from
									);

									//! Assignment operator
	OgreWrapper&					operator=(
										const OgreWrapper &source				//!< The source instance of the class to copy from
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

