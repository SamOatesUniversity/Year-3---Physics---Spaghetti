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

const bool TestAABB(
		CSpaghettiBoundsBox *boxOne,
		CSpaghettiBoundsBox *boxTwo,
		std::vector<CCollision> &collision
	)
{
	static const int noofBoxFaces = 6;

	Ogre::Vector3 faces[noofBoxFaces] = {
		Ogre::Vector3(-1,	0,	 0),
		Ogre::Vector3( 1,	0,	 0),
		Ogre::Vector3( 0,  -1,	 0),
		Ogre::Vector3( 0,   1,	 0),
		Ogre::Vector3( 0,	0,	-1),
		Ogre::Vector3( 0,	0,	 1)
	};

	Ogre::Vector3 p1Max = boxOne->GetPosition() + boxOne->GetMax();
	Ogre::Vector3 p1Min = boxOne->GetPosition() + boxOne->GetMin();

	Ogre::Vector3 p2Max = boxTwo->GetPosition() + boxTwo->GetMax();
	Ogre::Vector3 p2Min = boxTwo->GetPosition() + boxTwo->GetMin();

	float distances[noofBoxFaces] = {
		(p2Max.x - p1Min.x),
		(p1Max.x - p2Min.x),
		(p2Max.y - p1Min.y),
		(p1Max.y - p2Min.y),
		(p2Max.z - p1Min.z), 
		(p1Max.z - p2Min.z),
	};

	Ogre::Vector3 collisionNormal = faces[0];
	float closestDistance = distances[0];

	for(int faceIndex = 0; faceIndex < noofBoxFaces; ++faceIndex)
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
	newCollision.bodyOne = boxOne->GetBody();
	newCollision.bodyTwo = boxTwo->GetBody();
	newCollision.collisionNormal = collisionNormal;
	newCollision.collisionPoint = boxOne->GetPosition() + (collisionNormal * (boxOne->GetBoxSize() * 0.5f));
	newCollision.penetration = closestDistance;
	collision.push_back(newCollision);

	return true;
}

float vecMultiply(
		Ogre::Vector3 vec1,
		Ogre::Vector3 vec2
	)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z + vec2.z;
}

Ogre::Vector3 contactPoint(
		Ogre::Vector3 &pOne,
		Ogre::Vector3 &dOne,
		float oneSize,
		Ogre::Vector3 &pTwo,
		Ogre::Vector3 &dTwo,
		float twoSize,
		bool useOne
	)
{
	Ogre::Vector3 toSt, cOne, cTwo;
	float dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
	float denom, mua, mub;

	smOne = dOne.squaredLength();
	smTwo = dTwo.squaredLength();
	dpOneTwo = vecMultiply(dTwo, dOne);

	toSt = pOne - pTwo;
	dpStaOne = vecMultiply(dOne, toSt);
	dpStaTwo = vecMultiply(dTwo, toSt);

	denom = smOne * smTwo - dpOneTwo * dpOneTwo;

	// Zero denominator indicates parallel lines
	if (abs(denom) < 0.0001f) {
		return useOne ? pOne : pTwo;
	}

	mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
	mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

	// If either of the edges has the nearest point out
	// of bounds, then the edges aren't crossed, we have
	// an edge-face contact. Our point is on the edge, which
	// we know from the useOne parameter.
	if (mua > oneSize || mua < -oneSize || mub > twoSize || mub < -twoSize)
	{
		return useOne ? pOne : pTwo;
	}
	else
	{
		cOne = pOne + dOne * mua;
		cTwo = pTwo + dTwo * mub;
		return cOne * 0.5f + cTwo * 0.5f;
	}
}

void FillPointFaceBoxBox(
		CSpaghettiBoundsBox *boxOne,
		CSpaghettiBoundsBox *boxTwo,
		Ogre::Vector3 toCenter,
		std::vector<CCollision> &collision,
		unsigned best,
		float pen
	)
{
	Ogre::Vector3 normal = boxOne->GetAxis(best);
	if (vecMultiply(boxOne->GetAxis(best), toCenter) > 0)
		normal = normal * -1.0f;
	
	// Work out which vertex of box two we're colliding with.
	// Using toCentre doesn't work!
	Ogre::Vector3 vertex = boxTwo->GetBoxSize() * 0.5f;
	if (vecMultiply(boxTwo->GetAxis(0), normal) < 0) vertex.x = -vertex.x;
	if (vecMultiply(boxTwo->GetAxis(1), normal) < 0) vertex.y = -vertex.y;
	if (vecMultiply(boxTwo->GetAxis(2), normal) < 0) vertex.z = -vertex.z;

	CCollision newCollision;
	newCollision.bodyOne = boxOne->GetBody();
	newCollision.bodyTwo = boxTwo->GetBody();
	newCollision.collisionNormal = normal;
	newCollision.collisionPoint = boxTwo->GetTransform() * vertex;
	newCollision.penetration = pen;
	collision.push_back(newCollision);
}

const bool TestOBB(
	CSpaghettiBoundsBox *boxOne,
	CSpaghettiBoundsBox *boxTwo,
	std::vector<CCollision> &collision
	)
{
	Ogre::Vector3 toCenter = boxTwo->GetPosition() - boxOne->GetPosition();

	float pen = 0xFFFFFFFF;
	unsigned int best = (unsigned int)-1;

	if (!CheckForAxisOverlap(boxOne, boxTwo, boxOne->GetAxis(0), toCenter, pen, best, 0)) return false;
	if (!CheckForAxisOverlap(boxOne, boxTwo, boxOne->GetAxis(1), toCenter, pen, best, 1)) return false;
	if (!CheckForAxisOverlap(boxOne, boxTwo, boxOne->GetAxis(2), toCenter, pen, best, 2)) return false;

	if (!CheckForAxisOverlap(boxOne, boxTwo, boxTwo->GetAxis(0), toCenter, pen, best, 3)) return false;
	if (!CheckForAxisOverlap(boxOne, boxTwo, boxTwo->GetAxis(1), toCenter, pen, best, 4)) return false;
	if (!CheckForAxisOverlap(boxOne, boxTwo, boxTwo->GetAxis(2), toCenter, pen, best, 5)) return false;

	unsigned int bestSingleAxis = best;

	if (!CheckForAxisOverlap(boxOne, boxTwo, boxOne->GetAxis(0).crossProduct(boxTwo->GetAxis(0)), toCenter, pen, best, 6)) return false;
	if (!CheckForAxisOverlap(boxOne, boxTwo, boxOne->GetAxis(0).crossProduct(boxTwo->GetAxis(1)), toCenter, pen, best, 7)) return false;
	if (!CheckForAxisOverlap(boxOne, boxTwo, boxOne->GetAxis(0).crossProduct(boxTwo->GetAxis(2)), toCenter, pen, best, 8)) return false;

	if (!CheckForAxisOverlap(boxOne, boxTwo, boxOne->GetAxis(1).crossProduct(boxTwo->GetAxis(0)), toCenter, pen, best, 9)) return false;
	if (!CheckForAxisOverlap(boxOne, boxTwo, boxOne->GetAxis(1).crossProduct(boxTwo->GetAxis(1)), toCenter, pen, best, 10)) return false;
	if (!CheckForAxisOverlap(boxOne, boxTwo, boxOne->GetAxis(1).crossProduct(boxTwo->GetAxis(2)), toCenter, pen, best, 11)) return false;

	if (!CheckForAxisOverlap(boxOne, boxTwo, boxOne->GetAxis(2).crossProduct(boxTwo->GetAxis(0)), toCenter, pen, best, 12)) return false;
	if (!CheckForAxisOverlap(boxOne, boxTwo, boxOne->GetAxis(2).crossProduct(boxTwo->GetAxis(1)), toCenter, pen, best, 13)) return false;
	if (!CheckForAxisOverlap(boxOne, boxTwo, boxOne->GetAxis(2).crossProduct(boxTwo->GetAxis(2)), toCenter, pen, best, 14)) return false;

	if (best < 3)
	{
		// We've got a vertex of box two on a face of box one.
		FillPointFaceBoxBox(boxOne, boxTwo, toCenter, collision, best, pen);
		return true;
	}
	else if (best < 6)
	{
		// We've got a vertex of box one on a face of box two.
		// We use the same algorithm as above, but swap around
		// one and two (and therefore also the vector between their
		// centers).
		FillPointFaceBoxBox(boxTwo, boxOne, toCenter * -1.0f, collision, best - 3, pen);
		return true;
	}
	else
	{
		// We've got an edge-edge contact. Find out which axes
		best -= 6;
		unsigned int oneAxisIndex = best / 3;
		unsigned int twoAxisIndex = best % 3;
		Ogre::Vector3 oneAxis = boxOne->GetAxis(oneAxisIndex);
		Ogre::Vector3 twoAxis = boxTwo->GetAxis(twoAxisIndex);
		Ogre::Vector3 axis = oneAxis.crossProduct(twoAxis);
		axis.normalise();

		// The axis should point from box one to box two.
		if (vecMultiply(axis, toCenter) > 0) axis = axis * -1.0f;

		// We have the axes, but not the edges: each axis has 4 edges parallel
		// to it, we need to find which of the 4 for each object. We do
		// that by finding the point in the center of the edge. We know
		// its component in the direction of the box's collision axis is zero
		// (its a mid-point) and we determine which of the extremes in each
		// of the other axes is closest.
		Ogre::Vector3 ptOnOneEdge = boxOne->GetBoxSize() * 0.5f;
		Ogre::Vector3 ptOnTwoEdge = boxTwo->GetBoxSize() * 0.5f;
		for (unsigned i = 0; i < 3; ++i)
		{
			if (i == oneAxisIndex) 
				ptOnOneEdge[i] = 0;
			else if (vecMultiply(boxOne->GetAxis(i), axis) > 0) 
				ptOnOneEdge[i] = -ptOnOneEdge[i];

			if (i == twoAxisIndex) 
				ptOnTwoEdge[i] = 0;
			else if (vecMultiply(boxTwo->GetAxis(i), axis) < 0) 
				ptOnTwoEdge[i] = -ptOnTwoEdge[i];
		}

		// Move them into world coordinates (they are already oriented
		// correctly, since they have been derived from the axes).
		ptOnOneEdge = boxOne->GetTransform() * ptOnOneEdge;
		ptOnTwoEdge = boxTwo->GetTransform() * ptOnTwoEdge;

		// So we have a point and a direction for the colliding edges.
		// We need to find out point of closest approach of the two
		// line-segments.
		Ogre::Vector3 vertex = contactPoint(
			ptOnOneEdge, oneAxis, (boxOne->GetBoxSize() * 0.5f)[oneAxisIndex],
			ptOnTwoEdge, twoAxis, (boxTwo->GetBoxSize() * 0.5f)[twoAxisIndex],
			bestSingleAxis > 2
		);

		CCollision newCollision;
		newCollision.bodyOne = boxOne->GetBody();
		newCollision.bodyTwo = boxTwo->GetBody();
		newCollision.collisionNormal = axis;
		newCollision.collisionPoint = vertex;
		newCollision.penetration = pen;
		collision.push_back(newCollision);

		return true;
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

		static const bool useAABB = true; 

		if (useAABB)
			return TestAABB(this, otherBox, collision);

		return TestOBB(this, otherBox, collision);
	}
	else if (other->GetType() == BoundsType::Sphere)
	{
		// box on sphere
		CSpaghettiBoundsSphere *otherSphere = static_cast<CSpaghettiBoundsSphere*>(other);

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

		float dist = 0.0f;
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

		// Compile the contact
		Ogre::Vector3 closestPtWorld = m_xform * closestPoint;
		Ogre::Vector3 collisionNormal = (closestPtWorld - sphereCenter).normalisedCopy();

		CCollision newCollision;
		newCollision.bodyOne = GetBody();
		newCollision.bodyTwo = otherSphere->GetBody();
		newCollision.collisionNormal = collisionNormal;
		newCollision.collisionPoint = closestPtWorld;
		newCollision.penetration = otherSphere->GetRadius() - sqrt(dist);
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

	//m_boxSize = m_max - m_min;
}
