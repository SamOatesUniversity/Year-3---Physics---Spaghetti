#pragma once

#include "../../../SAM/SAM/SAM.h"
#include "CCollision.h"

#include <Windows.h>
#include <xnamath.h>

#include <vector>
#include <iostream>

struct BoundsType {
	enum Enum {
		Box,
		Sphere,
		Noof
	};
};

class CSpaghettiRigidBody;

class CSpaghettiBounds {

protected:

	BoundsType::Enum			m_type;													//!< The type of bounds
	SAM::TVector<float, 3>		m_position;												//!< The position of the bounds in world space
	SAM::TMatrix<float, 4, 4>	m_xform;												//!< The matrix representing the world xform of the bounds
	CSpaghettiRigidBody			*m_body;												//!< The rigidbody these bounds belong too

	SAM::TVector3				m_axis[3];

public:
								//! Class constructor
								CSpaghettiBounds();

								//! Class destructor
								~CSpaghettiBounds();

								//! Does this bounding box intersect with another
	virtual const bool			Intersects(
									CSpaghettiBounds  *other,							//!< The bounding box to test against
									std::vector<CCollision> &collision
								) = 0;

								//! Set the position of the bounding box
	virtual void				Transform(
									SAM::TVector<float, 3> &position,					//!< The position of the bounds in world space
									SAM::TMatrix<float, 3, 3> &rotation					//!< The rotation of the bounds in local space
								)
								{
									m_position = position;
								}

	SAM::TMatrix<float, 4, 4>	GetTransform()
								{ 
									return m_xform; 
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

								//! 
	void						SetBody(
										CSpaghettiRigidBody *body
									)
								{
									m_body = body;
								}

								//! 
	CSpaghettiRigidBody			*GetBody()
								{
									return m_body;
								}
};