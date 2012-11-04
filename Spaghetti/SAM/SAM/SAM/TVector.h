#pragma once

namespace SAM 
{
	template <class TYPE, unsigned int SIZE>
	class TVector {
	private:

		TYPE										m_element[SIZE];						//!< The elements of the vector

	public:
													//! Class constructor
													TVector();

													//! Class destructor
													~TVector();
	};

	template <class TYPE>
	class TVector<TYPE, 2> {
	private:

		TYPE										m_element[2];							//!< The elements of the vector

	public:
													//! Get the X element of the vector
		const TYPE									&X() const
													{
														return m_element[0];
													}

													//! Get the Y element of the vector
		const TYPE									&Y() const
													{
														return m_element[1];
													}

													//! Sets the elements in the vector
		void										Set(
														TYPE x,								//!< The x element of the vector
														TYPE y								//!< The y element of the vector
													)
													{
														m_element[0] = x;
														m_element[1] = y;
													}
	};

	template <class TYPE>
	class TVector<TYPE, 3> {
	private:

		TYPE										m_element[3];							//!< The elements of the vector

	public:
													//! Get the X element of the vector
		const TYPE									&X() const
													{
														return m_element[0];
													}

													//! Get the Y element of the vector
		const TYPE									&Y() const
													{
														return m_element[1];
													}

													//! Get the Z element of the vector
		const TYPE									&Z() const
													{
														return m_element[2];
													}
	
													//! Sets the elements in the vector
		void										Set(
														TYPE x,								//!< The x element of the vector
														TYPE y,								//!< The y element of the vector
														TYPE z								//!< The z element of the vector
													)
													{
														m_element[0] = x;
														m_element[1] = y;
														m_element[2] = z;
													}

													//! Set the x element of the vector
		void										SetX(
														TYPE x
													)
													{
														m_element[0] = x;
													}

													//! Set the y element of the vector
		void										SetY(
														TYPE y
													)
													{
														m_element[1] = y;
													}

													//! Set the z element of the vector
		void										SetZ(
														TYPE z
													)
													{
														m_element[2] = z;
													}

													//! Get the length of the vector
		TYPE										Length() const
													{
														return sqrt((m_element[0] * m_element[0]) + (m_element[1] * m_element[1]) + (m_element[2] * m_element[2]));
													}

													//! Add another vector to this vector and return the result as a new vector
		TVector<TYPE, 3>							operator+(const TVector<TYPE, 3> &otherVector) const
													{
														TVector<TYPE, 3> result;
														result.Set(m_element[0] + otherVector.X(), m_element[1] + otherVector.Y(), m_element[2] + otherVector.Z());
														return result;
													}

													//! Subtract another vector to this vector and return the result as a new vector
		TVector<TYPE, 3>							operator-(const TVector<TYPE, 3> &otherVector) const
													{
														TVector<TYPE, 3> result;
														result.Set(m_element[0] - otherVector.X(), m_element[1] - otherVector.Y(), m_element[2] - otherVector.Z());
														return result;
													}
	
													//! Scale a vector by a value
		TVector<TYPE, 3>							operator*(const TYPE other) const
													{
														TVector<TYPE, 3> result;
														result.Set(m_element[0] * other, m_element[1] * other, m_element[2] * other);
														return result;
													}
	};

	template <class TYPE>
	class TVector<TYPE, 4> {
	private:

		TYPE										m_element[4];							//!< The elements of the vector
			
	public:
													//! Get the X element of the vector
		const TYPE									&X() const
													{
														return m_element[0];
													}

													//! Get the Y element of the vector
		const TYPE									&Y() const
													{
														return m_element[1];
													}

													//! Get the Z element of the vector
		const TYPE									&Z() const
													{
														return m_element[2];
													}

													//! Get the W element of the vector
		const TYPE									&W() const
													{
														return m_element[3];
													}

													//! Sets the elements in the vector
		void										Set(
														TYPE x,								//!< The x element of the vector
														TYPE y,								//!< The y element of the vector
														TYPE z,								//!< The z element of the vector
														TYPE w								//!< The w element of the vector
													)
													{
														m_element[0] = x;
														m_element[1] = y;
														m_element[2] = z;
														m_element[3] = w;
													}
	};

}