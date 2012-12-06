#include "CSpaghettiBoundingBox.h"

/*
*	\brief	Class constructor		
*/
CSpaghettiBoundingBox::CSpaghettiBoundingBox()
{

}

/*
*	\brief	Class destructor
*/
CSpaghettiBoundingBox::~CSpaghettiBoundingBox()
{

}

/*
*	\brief	Set the corners of the bounding box, the min and max will also be calculated in here
*/
void CSpaghettiBoundingBox::SetCorners( 
		SAM::TVector<float, 3> *corners					//!< A pointer to the positions of the corners
	)
{
	for (int cornerIndex = 0; cornerIndex < NOOF_BOUNDINGBOX_CORNERS; ++cornerIndex)
	{
		m_corner[cornerIndex] = corners[cornerIndex];

		if (m_corner[cornerIndex].SumOf() < m_min.SumOf())
			m_min = m_corner[cornerIndex];

		if (m_corner[cornerIndex].SumOf() > m_max.SumOf())
			m_max = m_corner[cornerIndex];
	}
}
