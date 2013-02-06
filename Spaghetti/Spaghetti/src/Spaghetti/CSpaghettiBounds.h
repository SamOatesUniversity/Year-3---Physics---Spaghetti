#pragma once

#include "../../../SAM/SAM/SAM.h"
#include <vector>
#include <iostream>

struct BoundsType {
	enum Enum {
		Box,
		Sphere,
		Noof
	};
};

class CSpaghettiBounds {

protected:

	BoundsType::Enum			m_type;													//!< The type of bounds
	SAM::TVector<float, 3>		m_position;												//!< The position of the bounds in world space
	SAM::TQuaternion			m_rotation;												//!< The rotation of the bounds
	SAM::TVector3				m_axis[3];

public:
								//! Class constructor
								CSpaghettiBounds();

								//! Class destructor
								~CSpaghettiBounds();

								//! Does this bounding box intersect with another
	virtual const bool			Intersects(
									CSpaghettiBounds  *other							//!< The bounding box to test against
								) = 0;

								//! Set the position of the bounding box
	virtual void				Transform(
									SAM::TVector<float, 3> &position,					//!< The position of the bounds in world space
									SAM::TQuaternion &rotation							//!< The rotation of the bounds in local space
								)
								{
									m_position = position;
									m_rotation = rotation;
								}

								//! Get the position of the bounding box
	SAM::TVector<float, 3>		GetPosition() const
								{
									return m_position;
								}

								//! Get the type of bounds
	BoundsType::Enum			GetType() const
								{
									return m_type;
								}

								//! Get the height of the bounding box
	virtual float				Height() const = 0;

								//! Get the width of the bounding box
	virtual float				Width() const = 0;

								//! Get the depth of the bounding box
	virtual float				Depth() const = 0;

								//! Get a sphere bound representation of the bounds
	virtual const float			GetRadius() const = 0;

								//! 
	SAM::TVector3				&GetAxis(
										const int axisIndex								//!< 
									)
								{
									return m_axis[axisIndex];
								}
};