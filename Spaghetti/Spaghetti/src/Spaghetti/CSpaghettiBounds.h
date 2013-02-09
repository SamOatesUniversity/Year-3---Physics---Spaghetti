#pragma once

#include "../../../SAM/SAM/SAM.h"
#include "CCollision.h"

#include <OGRE/Ogre.h>

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
	Ogre::Vector3				m_position;												//!< The position of the bounds in world space
	Ogre::Matrix4				m_xform;												//!< The world xform of the bounds
	Ogre::Vector3				m_axis[3];												//!< The axis in local space
	CSpaghettiRigidBody			*m_body;												//!< The rigidbody these bounds belong too

public:
								//! Class constructor
								CSpaghettiBounds();

								//! Class destructor
								~CSpaghettiBounds();

								//! Does this bounding box intersect with another
	virtual const bool			Intersects(
									CSpaghettiBounds *other,							//!< The bounding box to test against
									std::vector<CCollision> &collision
								) = 0;

								//! Set the position of the bounding box
	virtual void				Transform(
									Ogre::Vector3 position,								//!< The position of the bounds in world space
									Ogre::Quaternion rotation							//!< The rotation of the bounds in local space
								)
								{
									m_position = position;
								}

								//! Get the position of the bounding box
	Ogre::Vector3				GetPosition() const
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

								//! Get an axis by a given id
	Ogre::Vector3				&GetAxis(
										const int axisIndex								//!< The axis to get x = 0; y = 1; z = 2
									)
								{
									return m_axis[axisIndex];
								}

								//! Set the body these bounds belong too
	void						SetBody(
										CSpaghettiRigidBody *body
									)
								{
									m_body = body;
								}

								//! Get the body these bounds belong too
	CSpaghettiRigidBody			*GetBody()
								{
									return m_body;
								}
};