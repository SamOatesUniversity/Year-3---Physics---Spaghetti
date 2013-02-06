#include "CSpaghettiBounds.h"

/*
*	\brief	Class constructor		
*/
CSpaghettiBounds::CSpaghettiBounds()
{
	m_position.Set(0, 0, 0);
	m_rotation.Set(0, 0, 0, 1);
	m_type = BoundsType::Noof;
}

/*
*	\brief	Class destructor
*/
CSpaghettiBounds::~CSpaghettiBounds()
{
}

