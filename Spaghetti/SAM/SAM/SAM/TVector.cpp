#include "TVector.h"

namespace SAM
{
	/*
	*	\Brief Class constructor
	*/
	template <class TYPE, unsigned int SIZE>
	TVector<TYPE, SIZE>::TVector()
	{
		// initialize all elements just so they aren't full of garbage
		for (unsigned int index = 0; index < SIZE; ++index)
		{
			m_element[index] = NULL;
		}
	}

	/*
	*	\Brief Class destructor
	*/
	template <class TYPE, unsigned int SIZE>
	TVector<TYPE, SIZE>::~TVector()
	{
		
	}
}