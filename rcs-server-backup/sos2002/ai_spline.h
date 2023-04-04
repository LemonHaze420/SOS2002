#ifndef _SPLINE_H_
#define _SPLINE_H_

#include "Templates.h"
#include "bs2all.h"

typedef long SplineID;
typedef long AI_SplineInterfaceID;

struct AI_MonoSpline;

struct AI_Spline // catmull-rom spline
{
	char *name;
	long numPoints; // at least 4 normally
	bool circular; // if true, then last vert is one before the first...
	bool backwards; // if true, then spline is backwards...
	vector3 *points;
	float *lengths;
	AI_MonoSpline *width;
};

struct AI_MonoSpline // catmull-rom spline
{
	char *name;
	long numVals; // at least 4 normally
	bool circular; // if true, then last val is one before the first...
	float *vals;
};

struct AI_SplineRef
{
	long seg;
	float t;
};

inline vector3 &AI_SplineGetPoint( const AI_Spline &inSpline, const long &index ) { return inSpline.points[ ( index + inSpline.numPoints ) % inSpline.numPoints ]; }
void AI_SplineGetPos( vector3 *out, const AI_Spline &inSpline, const AI_SplineRef &ref ); // gets the position at that point on the spline
void AI_SplineGetDelta( vector3 *out, const AI_Spline &inSpline, const AI_SplineRef &ref ); // gets the delta position at that point on the spline.
float AI_SplineGetSegLength( const AI_Spline &inSpline, const long seg ); // gets the length of the segment
float AI_SplineGetSegSectionLength( const AI_Spline &inSpline, const AI_SplineRef &ref ); // gets the length of the section up to t of the segment

float AI_MonoSplineGetPos( const AI_MonoSpline &inSpline, const AI_SplineRef &ref ); // gets the value at that point on the spline
float AI_MonoSplineGetDelta( const AI_MonoSpline &inSpline, const AI_SplineRef &ref ); // gets the delta value at that point on the spline.

class AI_SplineInterface
{
private:
	SplineID mySpline;
	AI_SplineInterfaceID myID;

	AI_SplineRef myRef;
	bool refAccurate; // if false, then need to find the new point before returning position.

	vector3 closestPoint;
	vector3 oldPosition;
	vector3 newPosition;

public:
	AI_SplineInterface( SplineID ID, vector3 pos ); // creates interface and attaches to the spline.
	~AI_SplineInterface();

	void SetNewPosition( vector3 newPos );
	friend class AI_SplineManager;
};

class AI_SplineManager
{
private:
	Warehouse<AI_Spline> splineList;
	Warehouse<AI_SplineInterface> interfaceList;

public:
	SplineID LoadSpline( char *name, double *list, long length );
	void AddWidthDataToSpline( SplineID ID, SplineID widthSpline ); // basically takes a second spline and find the distance to it at all points on the first spline.
	void SetDirectionOfSpline( SplineID ID, vector3 pos, vector3 direction ); // uses a point in space and a direction to show the spline manager which way is forward on a spline
	void UnloadSpline( SplineID ID );

	SplineID FindSpline( char *name );
	

	AI_SplineInterfaceID AddInterface( SplineID ID, vector3 pos );
	void RemoveInterface( AI_SplineInterfaceID ID );
	void MoveTo( AI_SplineInterfaceID ID, vector3 newPos );
	vector3 ClosestSplinePoint( AI_SplineInterfaceID ID ); // closest position on spline to current point in interface
	vector3 PointAhead( AI_SplineInterfaceID ID, vector3 direction, float distanceAhead, AI_SplineInterfaceID outputID = WAREHOUSE_NULL_ID ); // position of spline distance world units away.
	vector3 PointForward( AI_SplineInterfaceID ID, float distanceForward, AI_SplineInterfaceID outputID = WAREHOUSE_NULL_ID ); // position of spline distance world units forward (or backward if negative.
	float WidthAtPoint( AI_SplineInterfaceID ID ); // returns the width of the road at the given interface.
	vector3 LeftEdge( AI_SplineInterfaceID ID ); // returns the point of the left edge of the road...
	vector3 RightEdge( AI_SplineInterfaceID ID ); // returns the point of the left edge of the road...
	void ReLocate( AI_SplineInterfaceID ID ); // dumps old info and finds REAL closest point on spline...
	// possibly using an axis aligned bounding box of the voronoi regions created by the inverse of the spline points stored in a quadtree with slices (not leafy)
};

#endif