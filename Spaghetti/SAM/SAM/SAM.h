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

	/*
	*	\Brief Return the larger of two given values
	*/
	template<class TYPE>
	TYPE Max(TYPE a, TYPE b)
	{
 		return a > b ? a : b;
	}

	/*
	*	\Brief Return the smaller of two given values
	*/
	template<class TYPE>
	TYPE Min(TYPE a, TYPE b)
	{
 		return a < b ? a : b;
	}

}