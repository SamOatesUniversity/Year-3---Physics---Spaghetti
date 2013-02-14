#include "CSpaghettiBoundsBox.h"

/*
*	\brief	Class constructor		
*/
CSpaghettiBoundsBox::CSpaghettiBoundsBox()
{
	m_type = BoundsType::Box;
	m_min = Ogre::Vector3::ZERO;
	m_max = Ogre::Vector3::ZERO;
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

	m_boxSize = m_max - m_min;
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
	if (axis.squaredLength() < 0.0001f) 
		return true;

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
		// box on box

		CSpaghettiBoundsBox *const otherBox = static_cast<CSpaghettiBoundsBox*>(other);

		Ogre::Vector3 faces[6] = {
			Ogre::Vector3(-1,	0,	 0),
			Ogre::Vector3( 1,	0,	 0),
			Ogre::Vector3( 0,  -1,	 0),
			Ogre::Vector3( 0,   1,	 0),
			Ogre::Vector3( 0,	0,	-1),
			Ogre::Vector3( 0,	0,	 1)
		};
		
		Ogre::Vector3 p1Max = m_position + m_max;
		Ogre::Vector3 p1Min = m_position + m_min;

		Ogre::Vector3 p2Max = otherBox->GetPosition() + otherBox->GetMax();
		Ogre::Vector3 p2Min = otherBox->GetPosition() + otherBox->GetMin();

		float distances[6] = {
			(p2Max.x - p1Min.x), // distance of box 'b' to face on 'left' side of 'a'.
			(p1Max.x - p2Min.x), // distance of box 'b' to face on 'right' side of 'a'.
			(p2Max.y - p1Min.y), // distance of box 'b' to face on 'bottom' side of 'a'.
			(p1Max.y - p2Min.y), // distance of box 'b' to face on 'top' side of 'a'.
			(p2Max.z - p1Min.z), // distance of box 'b' to face on 'far' side of 'a'.
			(p1Max.z - p2Min.z), // distance of box 'b' to face on 'near' side of 'a'.
		};

		Ogre::Vector3 collisionNormal = faces[0];
		float closestDistance = distances[0];

		for(int faceIndex = 0; faceIndex < 6; ++faceIndex)
		{
			// box does not intersect face. So boxes don't intersect at all.
			if(distances[faceIndex] < 0.0f) 
				return false;

			if (distances[faceIndex] < closestDistance)
			{
				closestDistance = distances[faceIndex];
				collisionNormal = faces[faceIndex];
			}
		}

		CCollision newCollision;
		newCollision.bodyOne = GetBody();
		newCollision.bodyTwo = otherBox->GetBody();
		newCollision.collisionNormal = collisionNormal;
		newCollision.collisionPoint = m_position + (collisionNormal * (GetBoxSize() * 0.5f));

		collision.push_back(newCollision);

		return true;
	}
	else if (other->GetType() == BoundsType::Sphere)
	{
		// sphere on box

		CSpaghettiBoundsSphere *const otherSphere = static_cast<CSpaghettiBoundsSphere*>(other);

		// quick aabb test, for early out
		if (m_position.x + m_max.x < otherSphere->GetPosition().x - otherSphere->GetRadius()) return false;
		if (m_position.x + m_min.x > otherSphere->GetPosition().x + otherSphere->GetRadius()) return false;
		if (m_position.y + m_max.y < otherSphere->GetPosition().y - otherSphere->GetRadius()) return false;
		if (m_position.y + m_min.y > otherSphere->GetPosition().y + otherSphere->GetRadius()) return false;
		if (m_position.z + m_max.z < otherSphere->GetPosition().z - otherSphere->GetRadius()) return false;
		if (m_position.z + m_min.z > otherSphere->GetPosition().z + otherSphere->GetRadius()) return false;

		// potentially a collision, check against object aligned box

		// transform the spheres center of mass into the boxes space
		Ogre::Vector3 sphereCenter = otherSphere->GetPosition();
		Ogre::Vector3 sphereInBoxSpace = sphereCenter;
		sphereInBoxSpace.x -= m_xform[0][3];
		sphereInBoxSpace.y -= m_xform[1][3];
		sphereInBoxSpace.z -= m_xform[2][3];

		sphereInBoxSpace = Ogre::Vector3(
			sphereInBoxSpace.x * m_xform[0][0] +
			sphereInBoxSpace.y * m_xform[1][0] +
			sphereInBoxSpace.z * m_xform[2][0],

			sphereInBoxSpace.x * m_xform[0][1] +
			sphereInBoxSpace.y * m_xform[1][1] +
			sphereInBoxSpace.z * m_xform[2][1],

			sphereInBoxSpace.x * m_xform[0][2] +
			sphereInBoxSpace.y * m_xform[1][2] +
			sphereInBoxSpace.z * m_xform[2][2]			
		);

		float dist = 0;
		Ogre::Vector3 closestPoint = Ogre::Vector3::ZERO;
		Ogre::Vector3 halfBox = GetBoxSize() * 0.5f;

		// Clamp each coordinate to the box.
		dist = sphereInBoxSpace.x;
		if (dist > halfBox.x) dist = halfBox.x;
		if (dist < -halfBox.x) dist = -halfBox.x;
		closestPoint.x = dist;

		dist = sphereInBoxSpace.y;
		if (dist > halfBox.y) dist = halfBox.y;
		if (dist < -halfBox.y) dist = -halfBox.y;
		closestPoint.y = dist;

		dist = sphereInBoxSpace.z;
		if (dist > halfBox.z) dist = halfBox.z;
		if (dist < -halfBox.z) dist = -halfBox.z;
		closestPoint.z = dist;

		// Check we're in contact
		dist = (closestPoint - sphereInBoxSpace).squaredLength();
		if (dist > otherSphere->GetRadius() * otherSphere->GetRadius())
			return false;

		// Calculate the collision points
		Ogre::Vector3 closestPtWorld = m_xform * closestPoint;
		Ogre::Vector3 collisionNormal = sphereCenter - closestPtWorld;
		collisionNormal.normalise();

		// add the collision to our collision list
		CCollision newCollision;
		newCollision.bodyOne = GetBody();
		newCollision.bodyTwo = otherSphere->GetBody();
		newCollision.collisionNormal = collisionNormal;
		newCollision.collisionPoint = closestPtWorld;

		collision.push_back(newCollision);

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

	Ogre::Vector3 scale = Ogre::Vector3(1, 1, 1);
	m_xform.makeTransform(m_position, scale, rotation);

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
