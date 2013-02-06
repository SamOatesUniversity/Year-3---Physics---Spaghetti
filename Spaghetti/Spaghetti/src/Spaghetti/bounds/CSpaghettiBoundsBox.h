#pragma once

#include "../CSpaghettiBounds.h"
#include "CSpaghettiBoundsSphere.h"

#define NOOF_BOUNDINGBOX_CORNERS	8

class CSpaghettiBoundsBox : public CSpaghettiBounds {

private:

	SAM::TVector<float, 3>		m_corner[NOOF_BOUNDINGBOX_CORNERS];						//!< The local positions of the bounding boxes corners
	SAM::TVector<float, 3>		m_aaCorner[NOOF_BOUNDINGBOX_CORNERS];					//!< The axis aligned positions of the bounding boxes corners
	SAM::TVector<float, 3>		m_min;													//!< The minimum corner
	SAM::TVector<float, 3>		m_max;													//!< The maximum corner

public:

								//! Class constructor
								CSpaghettiBoundsBox();

								//! Class destructor
								~CSpaghettiBoundsBox();

								//! Other ride the equals operator
	CSpaghettiBoundsBox			&operator=(const CSpaghettiBoundsBox &otherBox)
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

								//! Set the position of the bounding box
	virtual void				Transform(
									SAM::TVector<float, 3> &position,					//!< The position of the bounds in world space
									SAM::TMatrix<float, 3, 3> &rotation					//!< The rotation of the bounds in local space
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
									CSpaghettiBounds *other,							//!< The bounding box to test against
									std::vector<CCollision> &collision
								);

								//! Get the height of the bounding box
	float						Height() const
								{
									return m_max.Y() - m_min.Y();
								}

								//! Get the width of the bounding box
	float						Width() const
								{
									return m_max.X() - m_min.X();
								}

								//! Get the depth of the bounding box
	float						Depth() const
								{
									return m_max.Z() - m_min.Z();
								}

								//! Get the spheres radius
	const float					GetRadius() const
								{
									return m_min.DistanceTo(m_max) * 0.5f;
								}
};