#include "CSpaghettiBounds.h"

/*
*	\brief	Class constructor		
*/
CSpaghettiBounds::CSpaghettiBounds()
{
	m_position.Set(0, 0, 0);
	m_rotation.Set(0, 0, 0, 1);
	m_type = BoundsType::Noof;

	m_axis[0].Set(1, 0, 0);
	m_axis[1].Set(0, 1, 0);	
	m_axis[2].Set(0, 0, 1);
}

/*
*	\brief	Class destructor
*/
CSpaghettiBounds::~CSpaghettiBounds()
{
}

