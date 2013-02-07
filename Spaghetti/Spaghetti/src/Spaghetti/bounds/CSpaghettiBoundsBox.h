#pragma once

#include "../CSpaghettiBounds.h"
#include "CSpaghettiBoundsSphere.h"

#define NOOF_BOUNDINGBOX_CORNERS	8

class CSpaghettiBoundsBox : public CSpaghettiBounds {

private:

	Ogre::Vector3				m_corner[NOOF_BOUNDINGBOX_CORNERS];						//!< The local positions of the bounding boxes corners
	Ogre::Vector3				m_aaCorner[NOOF_BOUNDINGBOX_CORNERS];					//!< The axis aligned positions of the bounding boxes corners
	Ogre::Vector3				m_min;													//!< The minimum corner
	Ogre::Vector3				m_max;													//!< The maximum corner

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
									Ogre::Vector3 *corners						//!< A pointer to the positions of the corners
								);

								//! Set the position of the bounding box
	virtual void				Transform(
									Ogre::Vector3 position,					//!< The position of the bounds in world space
									Ogre::Quaternion rotation					//!< The rotation of the bounds in local space
								);								

								//! Get the minimum bound
	Ogre::Vector3				GetMin() const
								{
									return m_min;
								}

								//! Get the maximum bound
	Ogre::Vector3				GetMax() const
								{
									return m_max;
								}

								//! Get a given corner
	Ogre::Vector3				GetCorner(
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
									return m_max.y - m_min.y;
								}

								//! Get the width of the bounding box
	float						Width() const
								{
									return m_max.x - m_min.x;
								}

								//! Get the depth of the bounding box
	float						Depth() const
								{
									return m_max.z - m_min.z;
								}

								//! Get the spheres radius
	const float					GetRadius() const
								{
									return (m_max - m_min).length() * 0.5f;
								}
};