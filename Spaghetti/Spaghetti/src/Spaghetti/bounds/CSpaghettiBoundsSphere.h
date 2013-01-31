#pragma once

#include "../CSpaghettiBounds.h"
#include "CSpaghettiBoundsBox.h"

class CSpaghettiBoundsSphere : public CSpaghettiBounds {

private:

	float						m_diameter;

public:

								//! Class constructor
								CSpaghettiBoundsSphere();

								//! Class destructor
								~CSpaghettiBoundsSphere();

								//! Other ride the equals operator
	CSpaghettiBoundsSphere		&operator=(const CSpaghettiBoundsSphere &other)
								{
									m_diameter = other.GetDiameter();
									return *this;
								}

								//! Get the spheres diameter
	void						SetDiameter(
										const float diameter
									)
								{
									m_diameter = diameter;
								}

								//! Get the spheres diameter
	const float					GetDiameter() const
								{
									return m_diameter;
								}

								//! Get the spheres radius
	const float					GetRadius()
								{
									return (m_diameter * 0.5f);
								}

								//! Does this bounding box intersect with another
	const bool					Intersects(
									CSpaghettiBounds *other							//!< The bounding box to test against
								);

								//! Get the height of the bounding box
	float						Height() const
								{
									return m_diameter;
								}

								//! Get the width of the bounding box
	float						Width() const
								{
									return m_diameter;
								}

								//! Get the depth of the bounding box
	float						Depth() const
								{
									return m_diameter;
								}
};