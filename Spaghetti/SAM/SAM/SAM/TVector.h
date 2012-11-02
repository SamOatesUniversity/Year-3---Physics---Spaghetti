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