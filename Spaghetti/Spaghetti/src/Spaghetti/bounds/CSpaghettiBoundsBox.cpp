#include "CSpaghettiBoundsBox.h"

/*
*	\brief	Class constructor		
*/
CSpaghettiBoundsBox::CSpaghettiBoundsBox()
{
	m_type = BoundsType::Box;
}

/*
*	\brief	Class destructor
*/
CSpaghettiBoundsBox::~CSpaghettiBoundsBox()
{

}

/*
*	\brief	Set the corners of the bounding box, the min and max will also be calculated in here
*/
void CSpaghettiBoundsBox::SetCorners( 
		Ogre::Vector3 *corners					//!< A pointer to the positions of the corners
	)
{
	for (int cornerIndex = 0; cornerIndex < NOOF_BOUNDINGBOX_CORNERS; ++cornerIndex)
	{
		m_corner[cornerIndex] = corners[cornerIndex];
		m_aaCorner[cornerIndex] = corners[cornerIndex];

		if (m_corner[cornerIndex].x < m_min.x) m_min.x = m_corner[cornerIndex].x;
		if (m_corner[cornerIndex].y < m_min.y) m_min.y = m_corner[cornerIndex].y;
		if (m_corner[cornerIndex].z < m_min.z) m_min.z = m_corner[cornerIndex].z;

		if (m_corner[cornerIndex].x > m_max.x) m_max.x = m_corner[cornerIndex].x;
		if (m_corner[cornerIndex].y > m_max.y) m_max.y = m_corner[cornerIndex].y;
		if (m_corner[cornerIndex].z > m_max.z) m_max.z = m_corner[cornerIndex].z;
	}
}

/*
*	\brief	
*/
float TransformToAxis(
		CSpaghettiBoundsBox *box,
		const Ogre::Vector3 &axis
	)
{
	const float pa = axis.x * box->GetAxis(0).x + axis.y * box->GetAxis(0).y + axis.z * box->GetAxis(0).z;
	const float pb = axis.x * box->GetAxis(1).x + axis.y * box->GetAxis(1).y + axis.z * box->GetAxis(1).z;
	const float pc = axis.x * box->GetAxis(2).x + axis.y * box->GetAxis(2).y + axis.z * box->GetAxis(2).z;
	return	(box->Width() * 0.5f) * fabsf(pa) + (box->Height() * 0.5f) * fabsf(pb) + (box->Depth() * 0.5f) * fabsf(pc);
}

/*
*	\brief 
*/
const bool CheckForAxisOverlap(
		CSpaghettiBoundsBox *one,
		CSpaghettiBoundsBox *two,
		Ogre::Vector3 &axis,
		Ogre::Vector3 &toCenter,
		float &penetration,
		unsigned int &best,
		unsigned int id
	)
{
	if (axis.squaredLength() < 0.0001f) return true;
	axis.normalise();

	// Project the half-size of one onto axis
	const float oneProject = TransformToAxis(one, axis);
	const float twoProject = TransformToAxis(two, axis);

	// Project this onto the axis
	const float distance = fabsf(toCenter.x * axis.x + toCenter.y * axis.y + toCenter.z * axis.z);
	const float p = oneProject + twoProject - distance;

	if (p < 0)
		return false;

	if (p < penetration)
	{
		penetration = p;
		best = id;
	}

	return true;
}

/*
*	\brief	Does this bounding box intersect with another
*/
const bool CSpaghettiBoundsBox::Intersects( 
		CSpaghettiBounds *other,								//!< The bounding box to test against
		std::vector<CCollision> &collision	
	)
{
	collision.clear();

	if (other->GetType() == BoundsType::Box)
	{
		CSpaghettiBoundsBox *const otherBox = static_cast<CSpaghettiBoundsBox*>(other);
		
		if (m_position.x + m_max.x < otherBox->GetPosition().x + otherBox->GetMin().x) return false;
		if (m_position.x + m_min.x > otherBox->GetPosition().x + otherBox->GetMax().x) return false;
		if (m_position.y + m_max.y < otherBox->GetPosition().y + otherBox->GetMin().y) return false;
		if (m_position.y + m_min.y > otherBox->GetPosition().y + otherBox->GetMax().y) return false;
		if (m_position.z + m_max.z < otherBox->GetPosition().z + otherBox->GetMin().z) return false;
		if (m_position.z + m_min.z > otherBox->GetPosition().z + otherBox->GetMax().z) return false;

		CCollision newCollision;
		newCollision.bodyOne = GetBody();
		newCollision.bodyTwo = otherBox->GetBody();
		newCollision.collisionNormal = Ogre::Vector3(0, -1, 0);
		newCollision.collisionPoint = m_position - Ogre::Vector3(0, Height() * 0.5f, 0);

		collision.push_back(newCollision);

		return true;
	}
	else if (other->GetType() == BoundsType::Sphere)
	{
		CSpaghettiBoundsSphere *const otherSphere = static_cast<CSpaghettiBoundsSphere*>(other);

		if (m_position.x + m_max.x < otherSphere->GetPosition().x - otherSphere->GetRadius()) return false;
		if (m_position.x + m_min.x > otherSphere->GetPosition().x + otherSphere->GetRadius()) return false;
		if (m_position.y + m_max.y < otherSphere->GetPosition().y - otherSphere->GetRadius()) return false;
		if (m_position.y + m_min.y > otherSphere->GetPosition().y + otherSphere->GetRadius()) return false;
		if (m_position.z + m_max.z < otherSphere->GetPosition().z - otherSphere->GetRadius()) return false;
		if (m_position.z + m_min.z > otherSphere->GetPosition().z + otherSphere->GetRadius()) return false;

		return true;
	}
	
	return false;
}

void CSpaghettiBoundsBox::Transform( 
		Ogre::Vector3 position,							//!< The position of the bounds in world space 
		Ogre::Quaternion rotation						//!< The rotation of the bounds in local space 
	)
{
	m_position = position;

	m_axis[0] = Ogre::Vector3(1, 0, 0);
	m_axis[1] = Ogre::Vector3(0, 1, 0);	
	m_axis[2] = Ogre::Vector3(0, 0, 1);
	m_axis[0] = rotation * m_axis[0];
	m_axis[1] = rotation * m_axis[1];
	m_axis[2] = rotation * m_axis[2];

	m_min = Ogre::Vector3::ZERO;
	m_max = Ogre::Vector3::ZERO;

	for (int cornerIndex = 0; cornerIndex < NOOF_BOUNDINGBOX_CORNERS; ++cornerIndex)
	{
		m_corner[cornerIndex] = rotation * m_aaCorner[cornerIndex];

		if (m_corner[cornerIndex].x < m_min.x) m_min.x = m_corner[cornerIndex].x;
		if (m_corner[cornerIndex].y < m_min.y) m_min.y = m_corner[cornerIndex].y;
		if (m_corner[cornerIndex].z < m_min.z) m_min.z = m_corner[cornerIndex].z;

		if (m_corner[cornerIndex].x > m_max.x) m_max.x = m_corner[cornerIndex].x;
		if (m_corner[cornerIndex].y > m_max.y) m_max.y = m_corner[cornerIndex].y;
		if (m_corner[cornerIndex].z > m_max.z) m_max.z = m_corner[cornerIndex].z;
	}
}
