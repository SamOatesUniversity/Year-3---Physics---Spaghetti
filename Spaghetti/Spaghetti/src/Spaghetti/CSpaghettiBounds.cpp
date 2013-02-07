#include "CSpaghettiBounds.h"

/*
*	\brief	Class constructor		
*/
CSpaghettiBounds::CSpaghettiBounds()
{
	m_type = BoundsType::Noof;
	m_position = Ogre::Vector3(0, 0, 0);
	m_axis[0] = Ogre::Vector3(1, 0, 0);
	m_axis[1] = Ogre::Vector3(0, 1, 0);	
	m_axis[2] = Ogre::Vector3(0, 0, 1);
	m_body = nullptr;
}

/*
*	\brief	Class destructor
*/
CSpaghettiBounds::~CSpaghettiBounds()
{
}

