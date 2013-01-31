#include "CSpaghettiBounds.h"

/*
*	\brief	Class constructor		
*/
CSpaghettiBounds::CSpaghettiBounds()
{
	m_axis[0].Set(1, 0, 0);
	m_axis[1].Set(0, 1, 0);
	m_axis[2].Set(0, 0, 1);

	m_matWorld.Translate(m_position.X(), m_position.Y(), m_position.Z());
}

/*
*	\brief	Class destructor
*/
CSpaghettiBounds::~CSpaghettiBounds()
{

}

