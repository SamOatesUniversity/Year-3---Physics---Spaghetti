#include "TQuaternion.h"

using namespace SAM;

/*
*	\brief	Class constructor
*/
TQuaternion::TQuaternion()
{
	m_vector.Set(0.0f, 0.0f, 0.0f);
	m_w = 1.0f;
}

/*
*	\brief	Class destructor
*/
TQuaternion::~TQuaternion()
{

}

/*
*	\brief	Get the magnitude of the quaternion
*/
float TQuaternion::Length() const
{
	return sqrt((m_w * m_w) + (m_vector.X() * m_vector.X()) + (m_vector.Y() * m_vector.Y()) + (m_vector.Z() * m_vector.Z()));
}

/*
*	\brief	Normalize the quaternion
*/
void TQuaternion::Normalize()
{
	float d = m_w*m_w + m_vector[0]*m_vector[0] + m_vector[1]*m_vector[1] + m_vector[2]*m_vector[2];

	// Check for zero length quaternion, and use the no-rotation
	// quaternion in that case.
	if (d == 0) {
		m_w = 1;
		return;
	}

	d = 1.0f / sqrt(d);
	m_w *= d;
	m_vector[0] *= d;
	m_vector[1] *= d;
	m_vector[2] *= d;
}

/*
*	\brief	Convert the quaternion to a 3x3 matrix
*/
TMatrix<float, 3, 3> TQuaternion::ToMatrix3x3()
{
	TMatrix<float, 3, 3> matrix;
	
	float fTx  = m_vector[0]+m_vector[0];
	float fTy  = m_vector[1]+m_vector[1];
	float fTz  = m_vector[2]+m_vector[2];
	float fTwx = fTx*m_w;
	float fTwy = fTy*m_w;
	float fTwz = fTz*m_w;
	float fTxx = fTx*m_vector[0];
	float fTxy = fTy*m_vector[0];
	float fTxz = fTz*m_vector[0];
	float fTyy = fTy*m_vector[1];
	float fTyz = fTz*m_vector[1];
	float fTzz = fTz*m_vector[2];

	matrix[0][0] = 1.0f-(fTyy+fTzz);
	matrix[0][1] = fTxy-fTwz;
	matrix[0][2] = fTxz+fTwy;
	matrix[1][0] = fTxy+fTwz;
	matrix[1][1] = 1.0f-(fTxx+fTzz);
	matrix[1][2] = fTyz-fTwx;
	matrix[2][0] = fTxz-fTwy;
	matrix[2][1] = fTyz+fTwx;
	matrix[2][2] = 1.0f-(fTxx+fTyy);

	return matrix;
}

/*
*	\brief	Convert a 3x3 matrix to a quaternion
*/
void TQuaternion::FromMatrix3x3(
		TMatrix<float, 3, 3> matrix
	)
{
	const float trace = matrix[0][0] + matrix[1][1] + matrix[2][2];

	if (trace > 0)
    {
        float S = static_cast<float>(sqrt(trace + 1.0) * 2.0f);
        float inverseS = 1.0f / S;
        m_w = 0.25f * S;
		m_vector.SetX((matrix[1][2] - matrix[2][1]) * inverseS);
		m_vector.SetY((matrix[2][0] - matrix[0][2]) * inverseS);
        m_vector.SetZ((matrix[0][1] - matrix[1][0]) * inverseS);
    }
	else
	{
		static int s_iNext[3] = { 1, 2, 0 };

		int i = 0;
		if (matrix[1][1] > matrix[0][0]) i = 1;
		if (matrix[2][2] > matrix[i][i]) i = 2;
		int j = s_iNext[i];
		int k = s_iNext[j];

		float fRoot = sqrt(matrix[i][i] - matrix[j][j] - matrix[k][k] + 1.0f);
		m_vector[i] = 0.5f * fRoot;
		fRoot = 0.5f / fRoot;

		m_w = (matrix[k][j] - matrix[j][k]) * fRoot;
		m_vector[j] = (matrix[j][i] + matrix[i][j]) * fRoot;
		m_vector[k] = (matrix[k][i] + matrix[i][k]) * fRoot;
	}
}