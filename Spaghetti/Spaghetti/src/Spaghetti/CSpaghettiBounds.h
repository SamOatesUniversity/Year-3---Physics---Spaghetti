#pragma once

#include "../../../SAM/SAM/SAM.h"

class CSpaghettiBoundsBox;

class CSpaghettiBounds {

protected:

	SAM::TVector<float, 3>		m_position;

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
	void						Transform(
									SAM::TVector<float, 3> &position					//!< 
								)
								{
									m_position = position;
								}

								//! Get the position of the bounding box
	SAM::TVector<float, 3>		GetTransform() const
								{
									return m_position;
								}

								//! Get the height of the bounding box
	virtual float				Height() const = 0;

								//! Get the width of the bounding box
	virtual float				Width() const = 0;

								//! Get the depth of the bounding box
	virtual float				Depth() const = 0;
};