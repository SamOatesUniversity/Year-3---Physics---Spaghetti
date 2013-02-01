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

															//! Set the values of the quaternion
		void												Set(float x, float y, float z, float w)
															{
																m_vector.Set(x, y, z);
																m_w = w;
															}

															//! Normalize the quaternion
		void												Normalize();

															//! Get the magnitude of the quaternion
		float												Length() const;

															//! Convert the quaternion to a 3x3 matrix
		TMatrix<float, 3, 3>								ToMatrix3x3();

															//! Convert a 3x3 matrix to a quaternion
		void												FromMatrix3x3(
																TMatrix<float, 3, 3> matrix
															);

															//! Get the scalar part of the quaternion
		float												W() const
															{
																return m_w;
															}

															//! Get the scalar part of the quaternion
		float												X() const
															{
																return m_vector.X();
															}

															//! Get the scalar part of the quaternion
		float												Y() const
															{
																return m_vector.Y();
															}

															//! Get the scalar part of the quaternion
		float												Z() const
															{
																return m_vector.Z();
															}

	};

}