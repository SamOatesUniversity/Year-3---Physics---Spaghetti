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
	};

}