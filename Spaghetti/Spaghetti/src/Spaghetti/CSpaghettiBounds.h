#pragma once

#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "../../../SAM/SAM/SAM.h"
#include <vector>

struct BoundsType {
	enum Enum {
		Box,
		Sphere,
		Noof
	};
};

class CSpaghettiBounds {

protected:

	BoundsType::Enum			m_type;

	SAM::TVector<float, 3>		m_position;

	SAM::TVector<float, 3>		m_axis[3];			// Local x-y-z Axes
	SAM::TMatrix<float, 4, 4>	m_matWorld;

	std::vector<SAM::TVector3>	m_hitPoint;
	SAM::TVector<float, 3>		m_hitNormal;
	float						m_penetration;

public:

	SAM::TVector<float, 3>		m_e;				// Positive halfwidths along each axis

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
									SAM::TVector<float, 3> &position,					//!< 
									SAM::TMatrix<float, 3, 3> &rotation					//!< 
								)
								{
									m_position = position;

									m_axis[0].Set(1, 0, 0);
									m_axis[1].Set(0, 1, 0);
									m_axis[2].Set(0, 0, 1);

									SAM::TMatrix<float, 4, 4> rotationMat = rotation.ToMatrix4x4();

									m_axis[0] = rotationMat.Transform(m_axis[0]);
									m_axis[1] = rotationMat.Transform(m_axis[1]);
									m_axis[2] = rotationMat.Transform(m_axis[2]);

									SAM::TMatrix<float, 4, 4> xformMat;
									xformMat.Translate(m_position.X(), m_position.Y(), m_position.Z());

									m_matWorld = rotationMat * xformMat;
								}

								//! Get the position of the bounding box
	SAM::TVector<float, 3>		GetTransform() const
								{
									return m_position;
								}

								//! Get local axis
	SAM::TVector<float, 3>		GetAxis(const int index) const 
								{
									return m_axis[index];
								}

								//! 
	SAM::TMatrix<float, 4, 4>	GetWorldMatrix() const
								{
									return m_matWorld;
								}

								//! Get the type of bounds
	BoundsType::Enum			GetType() 
								{
									return m_type;
								}

								//! Get the height of the bounding box
	virtual float				Height() const = 0;

								//! Get the width of the bounding box
	virtual float				Width() const = 0;

								//! Get the depth of the bounding box
	virtual float				Depth() const = 0;

								//! Get the sphere radius
	virtual const float			GetRadius() = 0;

								//! 
	std::vector<SAM::TVector3>	&GetHitPoints()
								{
									return m_hitPoint;
								}

								//!
	SAM::TVector<float, 3>		GetHitNormal()
								{
									return m_hitNormal;
								}

								//! 
	float						GetPenetration()
								{
									return m_penetration;
								}

};