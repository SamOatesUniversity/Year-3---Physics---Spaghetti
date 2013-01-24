#pragma once

#include <math.h>
#include "TVector.h"
#include "TMatrix.h"

namespace SAM {

	class TQuaternion {

	private:

		float												m_w;											//!< Scalar part of the quaternion
		TVector<float, 3>									m_vector;										//!< Vector part of the quaternion

	public:

															//! Class constructor
															TQuaternion();

															//! Class destructor
															~TQuaternion();

															//! Normalize the quaternion
		void												Normalize();

															//! Get the magnitude of the quaternion
		float												Length() const;

															//! Convert the quaternion to a 3x3 matrix
		TMatrix<float, 3, 3>								ToMatrix3x3();

	};

}