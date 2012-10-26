#ifndef TYPES_H
#define TYPES_H

//-------------------------------------------------------------------------------------------//
//
//	../res/Types.h header provides basics stuctures, mainly for Math or storing purpose (PL)
//
//-------------------------------------------------------------------------------------------//

//GL Matrices
//typedef GLdouble GLMatrix[16];
//typedef double GLMatrix3[9];

#include <iostream>
#include <fstream>
#include <vector>

//GL Matrices
typedef double GLMatrix16[16];
typedef double GLMatrix[9];

using namespace std;

//utility data structure
struct ColorSum { 
	float x,y; 
	vector<float> itensitySum;

};

//-------------------------------------------------------------//
//	Misc. Structures
//-------------------------------------------------------------//
struct ColorInt { int x,y; };
struct ColorFloat { float x,y; };
struct PointFloat { float x,y; };
struct SmallBox { ColorInt maxPoint, minPoint; };
struct PositionData { ColorInt before, previous, current; };
struct Vector { float x, y; };
struct Vector3f { float x, y, z; };
struct ColorCount { int x,y; };
struct Color { int r,g,b,w; };
struct Box { int minX, maxX, minY, maxY; };
struct TriangleOrder {	float p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10; };

//-------------------------------------------------------------//
//	Structure Point2Df
//-------------------------------------------------------------//
struct Point2Df 
{ 
	Point2Df(){}

	Point2Df(float _x, float _y) 
	{
		x = _x; 
		y = _y; 
	}
	
	float x, y; 

	void Normalize(void)
	{
		float norm = sqrtf(powf(x, 2) + powf(y, 2));

		if(norm > 0)
		{
			x = x/norm;
			y = y/norm;
		}
		else
		{
			x = 0;
			y = 0;
		}
	}

	Point2Df operator-(const Point2Df &p1)
	{
		return Point2Df(x-p1.x, y-p1.y);
	}

};

//-------------------------------------------------------------//
//	Structure Point3D
//-------------------------------------------------------------//
struct Point3D { 

	float x,y,z; 
	float radius;

	void Normalize(void)
	{
		float norm = sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
		x = x/norm;
		y = y/norm;
		z = z/norm;
	}

	float Distance3D(Point3D p)
	{
		return sqrtf(powf(x-p.x, 2) + powf(y-p.y, 2) + powf(z-p.z, 2));
	}

	float Distance2D(Point3D p)
	{
		return sqrtf(powf(x-p.x, 2) + powf(z-p.z, 2));
	}

	//initializes point to (0, 0, 0)
	void Zero(void)
	{
		x = y = z = 0.0f;
	}
};

//-------------------------------------------------------------//
//	Structure Point3Df (additional const-ness added by Simon)
//-------------------------------------------------------------//
struct Point3Df
{
	float x, y, z;

	//default constructor
	Point3Df(void)
	{
		x = y = z = 0.0f;
	}

	//constructor
	Point3Df(float vx, float vy, float vz)
	{
		x = vx;
		y = vy;
		z = vz;
	}

	//initializes point to (0, 0, 0)
	void Zero(void)
	{
		x = y = z = 0.0f;
	}

	//normalizes point (useful if Point3Df is used as vector)
	void Normalize(void)
	{
		float norm = sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
		x = x/norm;
		y = y/norm;
		z = z/norm;
	}

	//returns the norm of the vector
	float Norm(void) const
	{
		return (sqrtf(powf(x, 2)+powf(y, 2)+powf(z, 2)));
	}

	//returns the norm^2 of the vector (added by Simon)
	float Norm2(void) const
	{
		return ( powf(x, 2)+powf(y, 2)+powf(z, 2) );
	}
	
	// set individual components (added by Simon)
	void set_x(float _x) {x = _x;}
	void set_y(float _y) {y = _y;}
	void set_z(float _z) {z = _z;}

	// return individual components (added by Simon)
	float get_x(void) {return x;}
	float get_y(void) {return y;}
	float get_z(void) {return z;}

	//sets the point coordinates to the specified values
	void SetPosition(float px, float py, float pz)
	{
		x = px;
		y = py;
		z = pz;
	}

	//sets the point's coordinates to the same as the parameter point
	void SetPosition(Point3Df p)
	{
		x = p.x;
		y = p.y;
		z = p.z;
	}

	//operator + (e.g. Point1 + Point2)
	Point3Df operator+(const Point3Df &p1) const
	{
		return Point3Df(p1.x+x, p1.y+y, p1.z+z);
	}

	//operator - (e.g. Point1 - Point2)
	Point3Df operator-(const Point3Df &p1) const
	{
		return Point3Df(x-p1.x, y-p1.y, z-p1.z);
	}

	//cross product
	Point3Df operator*(const Point3Df &p1) const
	{
		return Point3Df(y*p1.z - z*p1.y, z*p1.x - x*p1.z, x*p1.y - y*p1.x);
	}

	//scalar product
	Point3Df operator*(const float &n) const
	{
		return Point3Df(x*n, y*n, z*n);
	}

	//equality
	void operator=(const Point3Df &p1)
	{
		x = p1.x;
		y = p1.y;
		z = p1.z;
	}

	//equality (pointer and non const, added by Simon)
	void operator=( Point3Df* p1)
	{
		x = p1->x;
		y = p1->y;
		z = p1->z;
	}

	//returns mirror point
	Point3Df operator-(void) const
	{
		return Point3Df(-x, -y, -z);
	}

	//dot product 
	float operator^(const Point3Df &p1) const
	{
		return (x*p1.x + y*p1.y + z*p1.z);
	}

	//quick display of the point's coordinates
	void Cout(void) const
	{
		std::cout << "(" << x << ", " << y << ", " << z << ")" << endl;
	}
};

//-------------------------------------------------------------//
//	Structure Surface Point
//-------------------------------------------------------------//
struct SurfacePoint
{
	Point3Df point;
	Point3Df vector;
	Point3Df normal;
	float value;
};

struct outputData
{
	float time;
	Point3Df lHand;
	Point3Df rHand;
	Point3Df head;	
};

struct outputDataPosition
{
	int time;
	Point3Df wii;
	Point3Df led;
	Point3Df fudge;
};

//-------------------------------------------------------------//
//	Structure Target (was used for target generation)
//-------------------------------------------------------------//
struct Target
{
	Point3Df p;
	bool active;
	bool collided;
	bool timer;
	bool moving;
	float fading;
	float time_limit;
	float time_elapsed;
	float time_fading;
	bool time_out;
};

//-------------------------------------------------------------//
//	Structure Point3Di = INT
//-------------------------------------------------------------//
struct Point3Di
{
	int x, y, z;
};

//-------------------------------------------------------------//
//	Structure Matrix33f
//-------------------------------------------------------------//
struct Matrix33f
{
	float a, b, c;
	float d, e, f;
	float g, h, i;

	//sets values to 0
	void initialize(void)
	{
		a = b = c = d = e = f = g = h = i = 0.0f;
	}

	//returns determinant
	float determinant(void)
	{
		return(a*e*i-a*f*h+b*f*g-b*d*i+c*d*h-c*e*g);
	}

	//initializes matrix from pitch, roll and yaw values
	void set(float pitch, float roll, float yaw)
	{
		a = cosf(pitch)*cosf(yaw);
		b = sinf(roll)*sinf(pitch)*cosf(yaw) - cosf(roll)*sinf(yaw);
		c = cosf(roll)*sinf(pitch)*cosf(yaw) + sinf(roll)*sinf(yaw);
		d = cosf(pitch)*sinf(yaw);
		e = sinf(roll)*sinf(pitch)*sinf(yaw) + cosf(roll)*cosf(yaw);
		f = cosf(roll)*sinf(pitch)*sinf(yaw) - sinf(roll)*cosf(yaw);
		g = -sinf(pitch);
		h = sinf(roll)*cosf(pitch);
		i = cosf(roll)*cosf(pitch);		
	}

	//quick display
	void display(void)
	{
		cout << "[" << a << ", " << b << ", " << c << "]" << endl;
		cout << "[" << d << ", " << e << ", " << f << "]" << endl;
		cout << "[" << g << ", " << h << ", " << i << "]" << endl << endl;
	}
};

//-------------------------------------------------------------//
//	Structure ApplicationSettings for MotionCapture
//-------------------------------------------------------------//
struct ApplicationSettings
{
	int S_RUN_TRACKER_GREEN;		
	int S_RUN_TRACKER_RED;	
	int S_RUN_WIIMOTE_LEFT;	
	int S_RUN_WIIMOTE_RIGHT;	
	int S_RUN_WIIMOTE_HEAD;
	int S_DISPLAY_GL_DATA;
	int S_DISPLAY_TRACKER_DATA;
	int S_DISPLAY_REAL_DATA;
	int S_DISPLAY_RING;
	int S_DISPLAY_BODY_MODEL;
	int S_DISPLAY_BODY_LINES;
	int S_DISPLAY_GRID;
	int S_RUN_TARGET_SYSTEM;
	int S_DISPLAY_GUI;
	int S_LEVEL_LIMIT_WIN;
	int S_LEVEL_LIMIT_FAIL;
	int S_LEVEL_LIMIT_SPEED;
	int S_LEVEL_LIMIT_TIME;

	void initialize(void)
	{
		S_RUN_TRACKER_GREEN		= 0;
		S_RUN_TRACKER_RED		= 0;
		S_RUN_WIIMOTE_LEFT		= 0;
		S_RUN_WIIMOTE_RIGHT		= 0;
		S_RUN_WIIMOTE_HEAD		= 0;
		S_DISPLAY_GL_DATA		= 0;
		S_DISPLAY_TRACKER_DATA	= 0;
		S_DISPLAY_REAL_DATA		= 0;
		S_DISPLAY_RING			= 0;
		S_DISPLAY_BODY_MODEL	= 0;
		S_DISPLAY_BODY_LINES	= 0;
		S_DISPLAY_GRID			= 0;
		S_RUN_TARGET_SYSTEM		= 0;
		S_DISPLAY_GUI			= 0;
		S_LEVEL_LIMIT_WIN		= 5;
		S_LEVEL_LIMIT_FAIL		= 3;
		S_LEVEL_LIMIT_SPEED		= 25;
		S_LEVEL_LIMIT_TIME		= 10;
	}
};


// added by Simon

//--------------------------------------------------------------//
//		struct to hold local-to-world transformation data		//
//--------------------------------------------------------------//

struct worldTrans
{
	// translation in x,y,z axes
	// this maps the origin (in local space) to point (trans_x, trans_y, trans_z) in world space
	float trans_x;
	float trans_y;
	float trans_z;

	// scaling in x,y,z axes
	// this maps (1,1,1) (in local space) to point (scale_x, scale_y, scale_z) in world space
	float scale_x;
	float scale_y;
	float scale_z;

	// rotation about x,y,z axes
	// this maps (1,1,1,) (in local space) to point ( sin(rot_y).sin(rot_z), sin(rot_x).cos(rot_z), cos(rot_x).cos(rot_y) ) in world space (** need to check this **)
	float rot_x;
	float rot_y;
	float rot_z;

	void setIdentity()
	{
		trans_x = trans_y = trans_z = 0;
		scale_x = scale_y = scale_z = 1;
		rot_x = rot_y = rot_z = 0;
	}
};

#endif

//-------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------//