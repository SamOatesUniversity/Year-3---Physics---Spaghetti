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

	m_e = m_max - m_min;
	m_e = m_e * 0.5f;
}

void CalculateInterval(
		const CSpaghettiBoundsBox *box, 
		SAM::TVector<float, 3> &axis, 
		float& min, 
		float& max
	)
{
	float x = box->m_e.X();
	float y = box->m_e.Y();
	float z = box->m_e.Z();

	SAM::TVector<float, 3> Vertex[8];
	Vertex[0].Set(   x,    y,     z	);
	Vertex[1].Set(  -x,    y,	  z	);
	Vertex[2].Set(   x,   -y,	  z	);
	Vertex[3].Set(  -x,   -y,	  z	);
	Vertex[4].Set(   x,    y,	 -z	);
	Vertex[5].Set(  -x,    y,	 -z	);
	Vertex[6].Set(   x,   -y,	 -z	);
	Vertex[7].Set(  -x,   -y,	 -z	);

	SAM::TMatrix<float, 4, 4> worldXform = box->GetWorldMatrix();
	for (int vertIndex = 0; vertIndex < 8; ++vertIndex)
	{
		Vertex[vertIndex] = worldXform.Transform(Vertex[vertIndex]);
	}

	min = max = Vertex[0].Dot(axis);
	for (int vertIndex = 0; vertIndex < 8; ++vertIndex)
	{
		float d = Vertex[vertIndex].Dot(axis);
		if (d<min) min = d;
		if (d>max) max = d;
	}
}

bool SpanIntersect(
		const CSpaghettiBoundsBox *box0, 
		const CSpaghettiBoundsBox *box1,
		const SAM::TVector<float, 3> &axisc,
		SAM::TVector<float, 3> &hitNormal,
		float &penetration
	)
{
	SAM::TVector<float, 3> axis = axisc;
	

	const float lq = axis.LengthSquared();
	if (lq <= 0.02f)
		return true;

	axis = axis.Normalize();

	float mina, maxa;
	float minb, maxb;
	CalculateInterval(box0, axis, mina, maxa);
	CalculateInterval(box1, axis, minb, maxb);

	float lena = maxa - mina;
	float lenb = maxb - minb;

	float minv = SAM::Min( mina, minb );
	float maxv = SAM::Max( maxa, maxb );
	float lenv = maxv - minv;

	if ( lenv > (lena + lenb) )
		return false;

	const float p = (lena + lenb) - lenv;
	if (p < penetration)
	{
		penetration = p;
		hitNormal = axis;
		if (minb < mina) hitNormal = hitNormal * -1.0f;
	}

	return true;
}

int GetNumHitPoints( 
		const CSpaghettiBoundsBox *box,
		SAM::TVector<float, 3> &hitNormal,
		const float penetration,
		SAM::TVector<float, 3> verts[8],
		int* vertIndexs
	)
{

	float x = box->m_e.X();
	float y = box->m_e.Y();
	float z = box->m_e.Z();

	SAM::TVector<float, 3> Vertex[8];
	Vertex[0].Set( -x,  -y,  -z	);
	Vertex[1].Set(  x,  -y,	 -z	);
	Vertex[2].Set(  x,  -y,	  z	);
	Vertex[3].Set( -x,  -y,	  z	);
	Vertex[4].Set( -x,   y,	 -z	);
	Vertex[5].Set(  x,   y,	 -z	);
	Vertex[6].Set(  x,   y,	  z	);
	Vertex[7].Set( -x,   y,	  z	);

	SAM::TMatrix<float, 4, 4> worldXform = box->GetWorldMatrix();
	for (int vertIndex = 0; vertIndex < 8; ++vertIndex)
	{
		Vertex[vertIndex] = worldXform.Transform(Vertex[vertIndex]);
	}

	SAM::TVector<float, 3> planePoint = Vertex[0];
	float maxdist = Vertex[0].Dot(hitNormal);

	for (int vertIndex = 0; vertIndex < 8; ++vertIndex)
	{
		const float d = Vertex[vertIndex].Dot(hitNormal);
		if (d > maxdist)
		{
			maxdist = d;
			planePoint = Vertex[vertIndex];
		}
	}

	float d = planePoint.Dot(hitNormal);
	d -= penetration + 0.01f;

	int numVerts = 0;
	for (int vertIndex = 0; vertIndex < 8; ++vertIndex)
	{
		float side = Vertex[vertIndex].Dot(hitNormal) - d;

		if (side > 0)
		{
			verts[numVerts] = Vertex[vertIndex];
			vertIndexs[numVerts] = vertIndex;
			numVerts++;
		}
	}
	return numVerts;
}

void SortVertices(
		SAM::TVector<float, 3>* verts0,
		int* vertIndexs0)
{
	int faces[6][4] =
	{
		{4,0,3,7},
		{1,5,6,2},
		{0,1,2,3},
		{7,6,5,4},
		{5,1,0,4},
		{6,7,3,2}
	};

	int faceSet = -1;
	SAM::TVector<float, 3> temp[4]; // New correct clockwise order

	// Work out which face to use
	for (int i=0; i<6 && faceSet==-1; i++)
	{
		int numFound = 0;
		for (int j=0; j<4; j++)
		{
			if (vertIndexs0[j]==faces[i][j])
			{
				temp[numFound] = verts0[j];
				numFound++;

				if (numFound==4)
				{
					faceSet = i;
					break;
				}
			}
		}
	}

	if (faceSet < 0)
	{
		int errorHasOccured = 1;
	}
	else
	{
		for (int i=0; i<4; i++)
		{
			verts0[i] = temp[i];
		}
	}
}

bool VertInsideFace(
		SAM::TVector<float, 3> *verts0, 
		SAM::TVector<float, 3> &p0, 
		float planeErr
	)
{
	// Work out the normal for the face
	SAM::TVector3 v0 = verts0[1] - verts0[0];
	SAM::TVector3 v1 = verts0[2] - verts0[0];
	SAM::TVector3 n  = v1.Cross(v0);
	n = n.Normalize();

	for (int i=0; i<4; i++)
	{
		SAM::TVector3 s0 = verts0[i];
		SAM::TVector3 s1 = verts0[(i+1)%4];
		SAM::TVector3 sx = s0 + n*10.0f;

		// Work out the normal for the face
		SAM::TVector3 sv0 = s1 - s0;
		SAM::TVector3 sv1 = sx - s0;
		SAM::TVector3 sn  = sv1.Cross(sv0);
		sn = sn.Normalize();

		float d  = s0.Dot(sn);
		float d0 = p0.Dot(sn) - d;

		// Outside the plane
		if (d0 > planeErr)
		{
			return false;
		}
	}

	return true;
}

void ClipFaceFaceVerts(
		SAM::TVector<float, 3>* verts0,
		int* vertIndexs0,
		SAM::TVector<float, 3>* verts1,
		int* vertIndexs1,
		SAM::TVector<float, 3>* vertsX,
		int* numVertsX
	)
{
	SortVertices(verts0, vertIndexs0);
	SortVertices(verts1, vertIndexs1);

	// Work out the normal for the face
	SAM::TVector<float, 3> v0 = verts0[1] - verts0[0];
	SAM::TVector<float, 3> v1 = verts0[2] - verts0[0];
	SAM::TVector<float, 3> n  = v1.Cross(v0);
	n = n.Normalize();

	// Project all the vertices onto a shared plane, plane0
	SAM::TVector<float, 3> vertsTemp1[4];
	for (int vertIndex = 0; vertIndex < 4; ++vertIndex)
	{
		vertsTemp1[vertIndex] = verts1[vertIndex] + (n * n.Dot(verts0[0]-verts1[vertIndex]));
	}


	static SAM::TVector<float, 3> temp[50];
	int numVerts = 0;

	for (int c = 0; c < 2; ++c)
	{
		SAM::TVector<float, 3>* vertA = vertsTemp1;
		SAM::TVector<float, 3>* vertB = verts0;
		if (c == 1)
		{
			vertA = verts0;
			vertB = vertsTemp1;
		}

		// Work out the normal for the face
		SAM::TVector<float, 3> v0 = vertA[1] - vertA[0];
		SAM::TVector<float, 3> v1 = vertA[2] - vertA[0];
		SAM::TVector<float, 3> n  = v1.Cross(v0);
		n = n.Normalize();

		for (int i=0; i<4; i++)
		{
			SAM::TVector<float, 3> s0 = vertA[i];
			SAM::TVector<float, 3> s1 = vertA[(i+1)%4];
			SAM::TVector<float, 3> sx = s0 + n*10.0f;

			// Work out the normal for the face
			SAM::TVector<float, 3> sv0 = s1 - s0;
			SAM::TVector<float, 3> sv1 = sx - s0;
			SAM::TVector<float, 3> sn  = sv1.Cross(sv0);
			sn = sn.Normalize();

			float d = s0.Dot(sn);

			for (int j=0; j<4; j++)
			{
				SAM::TVector<float, 3> p0 = vertB[j];
				SAM::TVector<float, 3> p1 = vertB[(j+1)%4]; // Loops back to the 0th for the last one

				float d0 = p0.Dot(sn) - d;
				float d1 = p1.Dot(sn) - d;

				// Check there on opposite sides of the plane
				if ( (d0 * d1) < 0.0f)
				{
					SAM::TVector<float, 3> pX = p0 + ((p1 - p0) * (sn.Dot(s0-p0) / sn.Dot(p1-p0)));

					if (VertInsideFace(vertA, pX, 0.5f))
					{
						temp[numVerts] = pX;
						numVerts++;
					}
				}

				if (VertInsideFace(vertA, p0, 0.0f))
				{
					temp[numVerts] = p0;
					numVerts++;
				}
			}
		}
	}

	// Remove verts which are very close to each other
	for (int i=0; i<numVerts; i++)
	{
		for (int j=i; j<numVerts; j++)
		{
			if (i!=j)
			{
				float dist = (temp[i] - temp[j]).LengthSquared();

				if (dist < 6.5f)
				{

					for (int k=j; k<numVerts; k++)
					{
						temp[k] = temp[k+1];
					}
					numVerts--;
				}
			}
		}
	}

	*numVertsX = numVerts;
	for (int i=0; i<numVerts; i++)
	{
		vertsX[i] = temp[i];
	}
}

void ClosestPointLineLine( 
		const SAM::TVector<float, 3> *verts0,	
		const SAM::TVector<float, 3> *verts1,
		SAM::TVector<float, 3> *vertsX,
		int *numVertX
	)
{
	SAM::TVector<float, 3> p1 = verts0[0];
	SAM::TVector<float, 3> q1 = verts0[1];
	SAM::TVector<float, 3> p2 = verts1[0];
	SAM::TVector<float, 3> q2 = verts1[1];

	SAM::TVector<float, 3> d1 = q1 - p1;
	SAM::TVector<float, 3> d2 = q2 - p2;
	SAM::TVector<float, 3> r  = p1 - p2;
	float a = d1.Dot(d1);
	float e = d2.Dot(d2);
	float f = d2.Dot(r);

	const float epsilon = 0.00001f;

	float s, t;
	SAM::TVector<float, 3> c1, c2;

	if (a <= epsilon && e <= epsilon)
	{
		s = t = 0.0f;
		c1 = p1;
		c2 = p2;

		vertsX[0] = c1;
		*numVertX = 1;
		return;
	}

	if (a <= epsilon)
	{
		s = 0.0f;
		t = f / e;
		t = SAM::Clamp(t, 0.0f, 1.0f);
	}
	else
	{
		float c = d1.Dot(r);
		if (e <= epsilon)
		{
			t = 0.0f;
			s = SAM::Clamp(-c/a, 0.0f, 1.0f);
		}
		else
		{
			float b = d1.Dot(d2);
			float denom = a*e - b*b;

			if (denom != 0.0f)
			{
				s = SAM::Clamp((b*f - c*e) / denom, 0.0f, 1.0f);
			}
			else
			{
				s = 0.0f;
			}

			t = (b*s + f) / e;

			if (t < 0.0f)
			{
				t = 0.0f;
				s = SAM::Clamp(-c / a , 0.0f, 1.0f);
			}
			else if (t > 1.0f)
			{
				t = 1.0f;
				s = SAM::Clamp((b-c) / a, 0.0f, 1.0f);
			}
		}
	}

	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;

	vertsX[0] = (c1 + c2) * 0.5f;
	*numVertX = 1;
}

void ClosestPtPointOBB(
		const SAM::TVector<float, 3> &point, 
		const CSpaghettiBoundsBox *box0,
		SAM::TVector<float, 3> *closestP
	)
{
	SAM::TVector<float, 3> d = point - box0->GetTransform();
	SAM::TVector<float, 3> q = box0->GetTransform();

	SAM::TVector<float, 3> bme = box0->m_e;

	for (int i = 0; i < 3; ++i)
	{
		float dist = d.Dot(box0->GetAxis(i));

		if (dist > bme[i]) dist = bme[i];
		if (dist < -bme[i]) dist = -bme[i];

		q = q + (box0->GetAxis(i) * dist);
	}

	*closestP = q;
}

void ClipLinePlane(	
		const SAM::TVector<float, 3> *verts0,
		const int *vertIndexs0,
		const CSpaghettiBoundsBox *box0,
		const SAM::TVector<float, 3> *verts1, 
		int *vertIndexs1,
		const CSpaghettiBoundsBox *box1,
		SAM::TVector<float, 3> *vertsX,
		int *numVertX
	)
{
	ClosestPtPointOBB(verts0[0], box1, &vertsX[0]);
	ClosestPtPointOBB(verts0[1], box1, &vertsX[1]);
	*numVertX = 2;
}

void CSpaghettiBoundsBox::CalculateHitPoint(
		const CSpaghettiBoundsBox *box0, 
		const CSpaghettiBoundsBox *box1,
		SAM::TVector<float, 3> &hitNormal,
		float &penetration
	)
{
	SAM::TVector<float, 3> verts0[8];
	int vertIndex0[8];
	SAM::TVector<float, 3> norm0 = hitNormal;
	int numVerts0 = GetNumHitPoints(box0, norm0, penetration, verts0, vertIndex0);

	SAM::TVector<float, 3> verts1[8];
	int vertIndex1[8];
	SAM::TVector<float, 3> norm1 = hitNormal * -1.0f;
	int numVerts1 = GetNumHitPoints(box1, norm1, penetration, verts1, vertIndex1);

	// Sanity check, should be impossible
	if (numVerts0==0 || numVerts1==0)
		return;

	int numVertsX = numVerts0;
	SAM::TVector<float, 3> *vertsX  = verts0;

	if (numVerts0 >= 4 && numVerts1 >= 4)
	{
		static SAM::TVector<float, 3> clipVerts[50];
		ClipFaceFaceVerts(verts0, vertIndex0, verts1, vertIndex1, clipVerts, &numVertsX);
		vertsX = clipVerts;
	}

	if (numVerts1 < numVerts0)
	{
		numVertsX = numVerts1;
		vertsX    = verts1;
		hitNormal = norm1 * 1.0f;
	}

	if (numVerts1 == 2 && numVerts0 == 2)
	{
		static SAM::TVector<float, 3> V[2];
		static int numV = 0;
		ClosestPointLineLine(verts0, verts1, V,	&numV);
		vertsX = V;
		numVertsX = numV;
	}

	if (numVerts0 == 2 && numVerts1 == 4)
	{
		ClipLinePlane(verts0, vertIndex0, box0, verts1, vertIndex1, box1, vertsX, &numVertsX);
	}

	if (numVerts0 == 4 && numVerts1 == 2)
	{
		ClipLinePlane(verts1, vertIndex1, box1, verts0, vertIndex0, box0, vertsX, &numVertsX);
	}

	m_hitPoint.resize(numVertsX);
	for (int hitPoint = 0; hitPoint < numVertsX; ++hitPoint)
	{
		m_hitPoint[hitPoint] = vertsX[hitPoint];
	}

}

/*
*	\brief	Does this bounding box intersect with another
*/
const bool CSpaghettiBoundsBox::Intersects( 
		CSpaghettiBounds *other								//! The bounding box to test against
	)
{
	if (other->GetType() == BoundsType::Box)
	{
		CSpaghettiBoundsBox *const otherBox = static_cast<CSpaghettiBoundsBox*>(other);

		bool hit = true;

		m_penetration = 10000.0f;
		m_hitNormal.Set(0, 0, 0);

		hit &= SpanIntersect(this, otherBox, m_axis[0], m_hitNormal, m_penetration);
		hit &= SpanIntersect(this, otherBox, m_axis[1], m_hitNormal, m_penetration);
		hit &= SpanIntersect(this, otherBox, m_axis[2], m_hitNormal, m_penetration);

		hit &= SpanIntersect(this, otherBox, otherBox->GetAxis(0), m_hitNormal, m_penetration);
		hit &= SpanIntersect(this, otherBox, otherBox->GetAxis(1), m_hitNormal, m_penetration);
		hit &= SpanIntersect(this, otherBox, otherBox->GetAxis(2), m_hitNormal, m_penetration);

		hit &= SpanIntersect(this, otherBox, m_axis[0].Cross(other->GetAxis(0)), m_hitNormal, m_penetration);
		hit &= SpanIntersect(this, otherBox, m_axis[0].Cross(other->GetAxis(1)), m_hitNormal, m_penetration);
		hit &= SpanIntersect(this, otherBox, m_axis[0].Cross(other->GetAxis(2)), m_hitNormal, m_penetration);

		hit &= SpanIntersect(this, otherBox, m_axis[1].Cross(other->GetAxis(0)), m_hitNormal, m_penetration);
		hit &= SpanIntersect(this, otherBox, m_axis[1].Cross(other->GetAxis(1)), m_hitNormal, m_penetration);
		hit &= SpanIntersect(this, otherBox, m_axis[1].Cross(other->GetAxis(2)), m_hitNormal, m_penetration);

		hit &= SpanIntersect(this, otherBox, m_axis[2].Cross(other->GetAxis(0)), m_hitNormal, m_penetration);
		hit &= SpanIntersect(this, otherBox, m_axis[2].Cross(other->GetAxis(1)), m_hitNormal, m_penetration);
		hit &= SpanIntersect(this, otherBox, m_axis[2].Cross(other->GetAxis(2)), m_hitNormal, m_penetration);

		if (!hit) return false;

		m_hitPoint.clear();
		CalculateHitPoint(this, otherBox, m_hitNormal, m_penetration);

		return true;
	}
	else if (other->GetType() == BoundsType::Sphere)
	{
		CSpaghettiBoundsSphere *const otherSphere = static_cast<CSpaghettiBoundsSphere*>(other);

		if (m_position.X() + m_max.X() < otherSphere->GetTransform().X() - otherSphere->GetRadius()) return false;
		if (m_position.X() + m_min.X() > otherSphere->GetTransform().X() + otherSphere->GetRadius()) return false;
		if (m_position.Y() + m_max.Y() < otherSphere->GetTransform().Y() - otherSphere->GetRadius()) return false;
		if (m_position.Y() + m_min.Y() > otherSphere->GetTransform().Y() + otherSphere->GetRadius()) return false;
		if (m_position.Z() + m_max.Z() < otherSphere->GetTransform().Z() - otherSphere->GetRadius()) return false;
		if (m_position.Z() + m_min.Z() > otherSphere->GetTransform().Z() + otherSphere->GetRadius()) return false;

		return true;
	}
	
	return false;
}