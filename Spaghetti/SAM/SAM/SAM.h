#pragma once

#include "SAM/TVector.h"
#include "SAM/TMatrix.h"
#include "SAM/TQuaternion.h"

namespace SAM 
{

	static const float PI = 3.14159f;							//!< Global define of PI

	/*
	*	\Brief Convert a value given in radians into degrees
	*/
	template<class TYPE>
	TYPE RadToDegrees(TYPE val);

	/*
	*	\Brief Convert a value given in degrees into radians
	*/
	template<class TYPE>
	TYPE DegreesToRad(TYPE val);

	/*
	*	\Brief Clamp a value between a minimum and maximum value
	*/
	template<class TYPE>
	TYPE Clamp(TYPE val, TYPE min, TYPE max)
	{
		if (val > max) return max;
		if (val < min) return min;
		return val;
	}

}