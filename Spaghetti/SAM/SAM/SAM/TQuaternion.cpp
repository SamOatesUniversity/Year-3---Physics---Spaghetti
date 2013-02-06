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
	const float len = Length();

	m_w /= len;
	m_vector.Set(m_vector.X() / len, m_vector.Y() / len, m_vector.Z() / len);
}

/*
*	\brief	Convert the quaternion to a 3x3 matrix
*/
TMatrix<float, 3, 3> TQuaternion::ToMatrix3x3()
{
	TMatrix<float, 3, 3> matrix;

	matrix[0][0] = 1 - 2 * (m_vector.Y() * m_vector.Y()) - 2 * (m_vector.Z() * m_vector.Z());
	matrix[0][1] = 2 * m_vector.X() * m_vector.Z() + 2 * m_vector.Y() * m_w;
	matrix[0][2] = 2 * m_vector.X() * m_vector.Y() - 2 * m_vector.Z() * m_w;

	matrix[1][0] = 2 * m_vector.Y() * m_vector.Z() - 2 * m_vector.X() * m_w;
	matrix[1][1] = 1 - 2 * (m_vector.X() * m_vector.X()) - 2 * (m_vector.Z() * m_vector.Z());
	matrix[1][2] = 2 * m_vector.X() * m_vector.Y() + 2 * m_vector.Z() * m_w;

	matrix[2][0] = 2 * m_vector.Y() * m_vector.Z() + 2 * m_vector.X() * m_w;
	matrix[2][1] = 2 * m_vector.X() * m_vector.Z() - 2 * m_vector.Y() * m_w;
	matrix[2][2] = 1 - 2 * (m_vector.X() * m_vector.X()) - 2 * (m_vector.Y() * m_vector.Y());

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