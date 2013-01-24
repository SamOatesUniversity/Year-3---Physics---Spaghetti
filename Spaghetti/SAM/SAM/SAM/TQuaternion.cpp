#include "TQuaternion.h"

using namespace SAM;

/*
*	\brief	Class constructor
*/
TQuaternion::TQuaternion()
{
	m_vector.Set(0.0f, 0.0f, 0.0f);
	m_w = 0.0f;
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
	matrix[0][1] = 2 * m_vector.X() * m_vector.Y() - 2 * m_vector.Z() * m_w;
	matrix[0][2] = 2 * m_vector.X() * m_vector.Z() + 2 * m_vector.Y() * m_w;

	matrix[1][0] = 2 * m_vector.X() * m_vector.Y() + 2 * m_vector.Z() * m_w;
	matrix[1][1] = 1 - 2 * (m_vector.X() * m_vector.X()) - 2 * (m_vector.Z() * m_vector.Z());
	matrix[1][2] = 2 * m_vector.Y() * m_vector.Z() - 2 * m_vector.X() * m_w;

	matrix[2][0] = 2 * m_vector.X() * m_vector.Z() - 2 * m_vector.Y() * m_w;
	matrix[2][1] = 2 * m_vector.Y() * m_vector.Z() + 2 * m_vector.X() * m_w;
	matrix[2][2] = 1 - 2 * (m_vector.X() * m_vector.X()) - 2 * (m_vector.Y() * m_vector.Y());

	return matrix;
}