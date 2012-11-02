#include "SAM.h"

namespace SAM 
{

	/*
	*	\Brief Convert a value given in radians into degrees
	*/
	template<class TYPE>
	TYPE RadToDegrees(TYPE val)
	{
		return static_cast<TYPE>(TYPE * 57.2958);
	}

	/*
	*	\Brief Convert a value given in degrees into radians
	*/
	template<class TYPE>
	TYPE DegreesToRad(TYPE val)
	{
		return static_cast<TYPE>(TYPE * 0.01745);
	}

}