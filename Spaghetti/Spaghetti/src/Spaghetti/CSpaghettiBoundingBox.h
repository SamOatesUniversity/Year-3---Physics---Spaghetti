#pragma once

#include "../../../SAM/SAM/SAM.h"

#define NOOF_BOUNDINGBOX_CORNERS	8

class CSpaghettiBoundingBox {

private:

	SAM::TVector<float, 3>		m_corner[NOOF_BOUNDINGBOX_CORNERS];						//!< The local positions of the bounding boxes corners
	SAM::TVector<float, 3>		m_min;													//!< The minimum corner
	SAM::TVector<float, 3>		m_max;													//!< The maximum corner

	SAM::TVector<float, 3>		m_position;

public:

								//! Class constructor
								CSpaghettiBoundingBox();

								//! Class destructor
								~CSpaghettiBoundingBox();

								//! Other ride the equals operator
	CSpaghettiBoundingBox		&operator=(const CSpaghettiBoundingBox &otherBox)
								{
									m_min = otherBox.GetMin();
									m_max = otherBox.GetMax();
									
									for (int cornerIndex = 0; cornerIndex < NOOF_BOUNDINGBOX_CORNERS; ++cornerIndex)
										m_corner[cornerIndex] = otherBox.GetCorner(cornerIndex);

									return *this;
								}

								//! Set the corners of the bounding box, the min and max will also be calculated in here
	void						SetCorners(
									SAM::TVector<float, 3> *corners						//!< A pointer to the positions of the corners
								);

								//! Get the minimum bound
	SAM::TVector<float, 3>		GetMin() const
								{
									return m_min;
								}

								//! Get the maximum bound
	SAM::TVector<float, 3>		GetMax() const
								{
									return m_max;
								}

								//! Get a given corner
	SAM::TVector<float, 3>		GetCorner(
									const int index										//!< The corner index id
								) const
								{
									return m_corner[index];
								}

								//! Does this bounding box intersect with another
	const bool					Intersects(
									CSpaghettiBoundingBox &otherBox						//!< The bounding box to test against
								);

								//! 
	void						Transform(
									SAM::TVector<float, 3> &position					//!< 
								)
								{
									m_position = position;
								}

								//!
	SAM::TVector<float, 3>		GetTransform() const
								{
									return m_position;
								}
};