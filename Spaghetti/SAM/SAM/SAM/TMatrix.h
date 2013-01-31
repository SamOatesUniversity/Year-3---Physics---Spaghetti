#pragma once

#include <math.h>
#include "TVector.h"

namespace SAM 
{
	template <class TYPE, unsigned int ROWS, unsigned int COLUMNS>
	class TMatrix {

	private:

		TVector<TYPE, COLUMNS>										m_element[ROWS];

	public:
																	//! Class constructor
																	TMatrix()
																	{
																		Identity();
																	}

																	//! Class destructor
																	~TMatrix();

																	//! Change the matrix to an identity matrix
		void														Identity()
																	{
																		// can only default to identity matrix, if we are a square matrix
																		if (COLUMNS == ROWS)
																		{
																			unsigned int columnIndex = 0;
																			for (int rowIndex = 0; rowIndex < ROWS; ++rowIndex)
																				m_element[rowIndex][columnIndex++] = static_cast<TYPE>(1);
																		}
																	}

																	//! Change the matrix to a zero matrix
		void														Zero()
																	{
																		for (int rowIndex = 0; rowIndex < ROWS; ++rowIndex)
																			for (int columnIndex = 0; columnIndex < COLUMNS; ++columnIndex)
																				m_element[rowIndex][columnIndex] = static_cast<TYPE>(0);
																	}
	};

	//! Matrix 2x2
	template <class TYPE>
	class TMatrix<TYPE, 2, 2> {

	private:

		TVector<TYPE, 2>											m_element[2];

	public:
																	//! Class constructor
																	TMatrix()
																	{
																		Identity();
																	}

																	//! overload the subscript operator
		TVector<TYPE, 2>											&operator[] (const unsigned int index)
																	{
																		return m_element[index];
																	}

																	//! Change the matrix to an identity matrix
		void														Identity()
																	{																
																		unsigned int columnIndex = 0;
																		for (int rowIndex = 0; rowIndex < 2; ++rowIndex)
																			m_element[rowIndex][columnIndex++] = static_cast<TYPE>(1);
																	}

																	//! Change the matrix to a zero matrix
		void														Zero()
																	{
																		for (int rowIndex = 0; rowIndex < 2; ++rowIndex)
																			for (int columnIndex = 0; columnIndex < 2; ++columnIndex)
																				m_element[rowIndex][columnIndex] = static_cast<TYPE>(0);
																	}
	};

	//! Matrix 3x3
	template <class TYPE>
	class TMatrix<TYPE, 3, 3> {

	private:

		TVector<TYPE, 3>											m_element[3];

	public:
																	//! Class constructor
																	TMatrix()
																	{
																		Identity();
																	}

																	//! overload the subscript operator
		TVector<TYPE, 3>											&operator[] (const unsigned int index)
																	{
																		return m_element[index];
																	}

																	//! Change the matrix to an identity matrix
		void														Identity()
																	{																
																		unsigned int columnIndex = 0;
																		for (int rowIndex = 0; rowIndex < 3; ++rowIndex)
																			m_element[rowIndex][columnIndex++] = static_cast<TYPE>(1);
																	}

																	//! Change the matrix to a zero matrix
		void														Zero()
																	{
																		for (int rowIndex = 0; rowIndex < 3; ++rowIndex)
																			for (int columnIndex = 0; columnIndex < 3; ++columnIndex)
																				m_element[rowIndex][columnIndex] = static_cast<TYPE>(0);
																	}

																	//! Calculate the inverse matrix of this matrix
		TMatrix<TYPE, 3, 3>											Inverse()
																	{
																		const float invDeterminant = 1.0f / Determinant();

																		TMatrix<TYPE, 3, 3>	inverse;

																		inverse[0][0] =	 ((m_element[1][1] * m_element[2][2]) - (m_element[2][1] * m_element[1][2])) * invDeterminant;
																		inverse[0][1] = -((m_element[0][1] * m_element[2][2]) - (m_element[0][2] * m_element[2][1])) * invDeterminant;
																		inverse[0][2] =  ((m_element[0][1] * m_element[1][2]) - (m_element[0][2] * m_element[1][1])) * invDeterminant;

																		inverse[1][0] = -((m_element[1][0] * m_element[2][2]) - (m_element[1][2] * m_element[2][0])) * invDeterminant;
																		inverse[1][1] =  ((m_element[0][0] * m_element[2][2]) - (m_element[0][2] * m_element[2][0])) * invDeterminant;
																		inverse[1][2] = -((m_element[0][0] * m_element[1][2]) - (m_element[1][0] * m_element[0][2])) * invDeterminant;

																		inverse[2][0] =  ((m_element[1][0] * m_element[2][1]) - (m_element[2][0] * m_element[1][1])) * invDeterminant;
																		inverse[2][1] = -((m_element[0][0] * m_element[2][1]) - (m_element[2][0] * m_element[0][1])) * invDeterminant;
																		inverse[2][2] =  ((m_element[0][0] * m_element[1][1]) - (m_element[1][0] * m_element[0][1])) * invDeterminant;

																		return inverse;
																	}

																	//! Calculate the matrices determinant
		TYPE														Determinant()
																	{
																		return	(m_element[0][0] * (m_element[1][1] * m_element[2][2] - m_element[1][2] * m_element[2][1])) - 
																				(m_element[0][1] * (m_element[2][2] * m_element[1][0] - m_element[1][2] * m_element[2][0])) + 
																				(m_element[0][2] * (m_element[1][0] * m_element[2][1] - m_element[1][1] * m_element[2][0]));
																	}

																	//! Return a transposed version of this matrix
		TMatrix<TYPE, 3, 3>											Transpose()
																	{
																		TMatrix<TYPE, 3, 3>	transpose;

																		transpose[0][0] = m_element[0][0]; 
																		transpose[0][1] = m_element[1][0]; 
																		transpose[0][2] = m_element[2][0]; 

																		transpose[1][0] = m_element[0][1];
																		transpose[1][1] = m_element[1][1];
																		transpose[1][2] = m_element[2][1];

																		transpose[2][0] = m_element[0][2];
																		transpose[2][1] = m_element[1][2];
																		transpose[2][2] = m_element[2][2];

																		return transpose;
																	}

																	//! Add a matrix by another matrix
		TMatrix<TYPE, 3, 3>											operator+(TMatrix<TYPE, 3, 3> &other)
																	{
																		TMatrix<TYPE, 3, 3> result;
																		
																		for (int rowIndex = 0; rowIndex < 3; ++rowIndex)
																		{
																			for (int columnIndex = 0; columnIndex < 3; ++columnIndex)
																			{
																				result[rowIndex][columnIndex] = m_element[rowIndex][columnIndex] + other[rowIndex][columnIndex];
																			}
																		}

																		return result;
																	}

																	//! Multiply a matrix by another matrix
		TMatrix<TYPE, 3, 3>											operator*(TMatrix<TYPE, 3, 3> &other)
																	{
																		TMatrix<TYPE, 3, 3> result;
																		
																		for (int rowIndex = 0; rowIndex < 3; ++rowIndex)
																		{
																			for (int columnIndex = 0; columnIndex < 3; ++columnIndex)
																			{
																				result[rowIndex][columnIndex] = 0;
																				for (int multiplyIndex = 0; multiplyIndex < 3; ++multiplyIndex)
																				{
																					result[rowIndex][columnIndex] = result[rowIndex][columnIndex] + (m_element[rowIndex][multiplyIndex] * other[multiplyIndex][columnIndex]);
																				}
																			}
																		}

																		return result;
																	}

																	//! Multiply a matrix by a vector
		TVector<TYPE, 3>											operator*(TVector<TYPE, 3> &other)
																	{
																		TVector<TYPE, 3> result;

																		result[0] = (m_element[0][0] * other[0]) + (m_element[0][1] * other[1]) + (m_element[0][2] * other[2]);
																		result[1] = (m_element[1][0] * other[0]) + (m_element[1][1] * other[1]) + (m_element[1][2] * other[2]);
																		result[2] = (m_element[2][0] * other[0]) + (m_element[2][1] * other[1]) + (m_element[2][2] * other[2]);

																		return result;
																	}

																	//! Multiply a matrix by a scaler
		TMatrix<TYPE, 3, 3>											operator*(TYPE scaler)
																	{
																		TMatrix<TYPE, 3, 3> result;

																		for (int rowIndex = 0; rowIndex < 3; ++rowIndex)
																		{
																			for (int columnIndex = 0; columnIndex < 3; ++columnIndex)
																			{
																				result[rowIndex][columnIndex] = m_element[rowIndex][columnIndex] * scaler;
																			}
																		}

																		return result;
																	}

	};

	//! Matrix 4x4
	template <class TYPE>
	class TMatrix<TYPE, 4, 4> {

	private:

		TVector<TYPE, 4>											m_element[4];

	public:
																	//! Class constructor
																	TMatrix()
																	{
																		Identity();
																	}

																	//! overload the subscript operator
		TVector<TYPE, 4>											&operator[] (const unsigned int index)
																	{
																		return m_element[index];
																	}

																	//! Change the matrix to an identity matrix
		void														Identity()
																	{		
																		Zero();														
																		unsigned int columnIndex = 0;
																		for (int rowIndex = 0; rowIndex < 4; ++rowIndex)
																			m_element[rowIndex][columnIndex++] = static_cast<TYPE>(1);
																	}

																	//! Change the matrix to a zero matrix
		void														Zero()
																	{
																		for (int rowIndex = 0; rowIndex < 4; ++rowIndex)
																			for (int columnIndex = 0; columnIndex < 4; ++columnIndex)
																				m_element[rowIndex][columnIndex] = static_cast<TYPE>(0);
																	}

																	//! 
		TVector<TYPE, 3>											Transform(TVector<TYPE, 3> vector)
																	{
																		float norm = m_element[0][3] * vector.X() + m_element[1][3] * vector.Y() + m_element[2][3] * vector.Z() + m_element[3][3];

																		TVector<TYPE, 3> result;

																		result.SetX((m_element[0][0] * vector.X() + m_element[1][0] * vector.Y() + m_element[2][0] * vector.Z() + m_element[3][0]) / norm);
																		result.SetY((m_element[0][1] * vector.X() + m_element[1][1] * vector.Y() + m_element[2][1] * vector.Z() + m_element[3][1]) / norm);
																		result.SetZ((m_element[0][2] * vector.X() + m_element[1][2] * vector.Y() + m_element[2][2] * vector.Z() + m_element[3][2]) / norm);

																		return result;
																	}

																	//! 
		void														Translate(TYPE x, TYPE y, TYPE z)
																	{
																		Identity();
																		m_element[3][0] = x;
																		m_element[3][1] = y;
																		m_element[3][2] = z;
																	}

																	//! Multiply a matrix by another matrix
		TMatrix<TYPE, 4, 4>											operator*(TMatrix<TYPE, 4, 4> &other)
																	{
																		TMatrix<TYPE, 4, 4> result;
																		
																		for (int rowIndex = 0; rowIndex < 4; ++rowIndex)
																		{
																			for (int columnIndex = 0; columnIndex < 4; ++columnIndex)
																			{
																				result[rowIndex][columnIndex] = 0;
																				for (int multiplyIndex = 0; multiplyIndex < 4; ++multiplyIndex)
																				{
																					result[rowIndex][columnIndex] = result[rowIndex][columnIndex] + (m_element[rowIndex][multiplyIndex] * other[multiplyIndex][columnIndex]);
																				}
																			}
																		}

																		return result;
																	}
	};

}