#include "ai_spline.h"

#include "BAssert.h"

// catmull-rom is
//
// t2 = t*t;
// t3 = t2*t3;

// q(t) = 0.5 * (1.0f,t,t2,t3)  *	[  0  2  0  0 ]   [P1]
//									[ -1  0  1  0 ] * [P2]
//									[  2 -5  4 -1 ]   [P3]
//									[ -1  3 -3  1 ]   [P4] 

//	q(t) =	0.5 *(	(2 * P1) + 
//					(-P1 + P3) * t +
//					(2*P1 - 5*P2 + 4*P3 - P4) * t2 +
//					(-P1 + 3*P2 - 3*P3 + P4) * t3
//					) 

//	q(t) =	P2 + 
//			(P3 - P1) * 0.5 * t +
//			(P1 - 2.5*P2 + 2*P3 - 0.5*P4) * t2 +
//			(3*P2 - P1 - 3*P3 + P4)*0.5 * t3
//			

// the first derivative of the spline is:
//	q(t) =	(-P1 + P3) * 0.25 +
//			(P1 - 2.5*P2 + 2*P3 - 0.5*P4) * t2 +
//			(-P1 + 3*P2 - 3*P3 + P4)*0.5 * t3
//			

//    static double[][] M = {             // Catmull-Rom basis matrix
//      {-0.5,  1.5,  -1.5, 0.5},* t3
//      { 1  , -2.5,   2  ,-0.5},* t2
//      {-0.5,  0  ,   0.5, 0  },* t
//      { 0  ,  1  ,   0  , 0  } * 1
//       p[0], p[1], p[2] ,p[3]
//   };
// t = 0 { 0, 1, 0, 0 }
// t = 1 { 0, 0, 1, 0 }

//CCMatrix *romMat

void AI_SplineGetPos( vector3 *out, const AI_Spline &inSpline, const AI_SplineRef &ref ) // gets the position at that point on the spline
{
	float t = ref.t;
	long seg = ref.seg;
	Assert( t >= 0.0f && t <= 1.0f );
	float t2, t3;
	t2 = t * t;
	t3 = t2 * t;
	
	vector3 *p1, *p2, *p3, *p4;
	
	if( inSpline.circular )
	{
		Assert( seg >= 0 && seg <= inSpline.numPoints );
		p1 = &( inSpline.points[ ( seg - 1 + inSpline.numPoints ) % inSpline.numPoints ] );
		p2 = &( inSpline.points[ ( seg + 0 + inSpline.numPoints ) % inSpline.numPoints ] );
		p3 = &( inSpline.points[ ( seg + 1 + inSpline.numPoints ) % inSpline.numPoints ] );
		p4 = &( inSpline.points[ ( seg + 2 + inSpline.numPoints ) % inSpline.numPoints ] );
	}
	else
	{
		Assert( seg > 0 && seg < inSpline.numPoints - 2 ); // np = 4 seg > 0 == 1 seg < np-2 == 1
		p1 = &( inSpline.points[ seg - 1 ] );
		p2 = &( inSpline.points[ seg + 0 ] );
		p3 = &( inSpline.points[ seg + 1 ] );
		p4 = &( inSpline.points[ seg + 2 ] );
	}
	// n.b. numSegs == numPoints - 3
	out->x =
		p2->x // * 1.0f
		+	( p3->x - p1->x ) * 0.5f * t
		+	( p1->x - 2.5f * p2->x + 2 * p3->x - 0.5f * p4->x ) * t2
		+	( 3.0f * ( p2->x - p3->x ) - p1->x + p4->x ) * 0.5f * t3;
	out->y =
		p2->y // * 1.0f
		+	( p3->y - p1->y ) * 0.5f * t
		+	( p1->y - 2.5f * p2->y + 2 * p3->y - 0.5f * p4->y ) * t2
		+	( 3.0f * ( p2->y - p3->y ) - p1->y + p4->y ) * 0.5f * t3;
	out->z =
		p2->z // * 1.0f
		+	( p3->z - p1->z ) * 0.5f * t
		+	( p1->z - 2.5f * p2->z + 2 * p3->z - 0.5f * p4->z ) * t2
		+	( 3.0f * ( p2->z - p3->z ) - p1->z + p4->z ) * 0.5f * t3;
}

void AI_SplineGetDelta( vector3 *out, const AI_Spline &inSpline, const AI_SplineRef &ref ) // gets the delta position at that point on the spline.
{
	float t = ref.t;
	long seg = ref.seg;
	Assert( t >= 0.0f && t <= 1.0f );
	float t2;//, t3;
	t2 = t * t;
	//t3 = t2 * t3;
	
	vector3 *p1, *p2, *p3, *p4;
	
	if( inSpline.circular )
	{
		Assert( seg >= 0 && seg <= inSpline.numPoints );
		p1 = &( inSpline.points[ ( seg - 1 + inSpline.numPoints ) % inSpline.numPoints ] );
		p2 = &( inSpline.points[ ( seg + 0 + inSpline.numPoints ) % inSpline.numPoints ] );
		p3 = &( inSpline.points[ ( seg + 1 + inSpline.numPoints ) % inSpline.numPoints ] );
		p4 = &( inSpline.points[ ( seg + 2 + inSpline.numPoints ) % inSpline.numPoints ] );
	}
	else
	{
		Assert( seg > 0 && seg < inSpline.numPoints - 2 ); // np = 4 seg > 0 == 1 seg < np-2 == 1
		p1 = &( inSpline.points[ seg - 1 ] );
		p2 = &( inSpline.points[ seg + 0 ] );
		p3 = &( inSpline.points[ seg + 1 ] );
		p4 = &( inSpline.points[ seg + 2 ] );
	}
	// n.b. numSegs == numPoints - 3
	out->x =
		0.0f //p2->x // * 1.0f
		+	( p3->x - p1->x ) * 0.5f * 1.0f
		+	( p1->x - 2.5f * p2->x + 2 * p3->x - 0.5f * p4->x ) * 2.0f * t
		+	( 3.0f * ( p2->x - p3->x ) - p1->x + p4->x ) * 0.5f * 3.0f * t2;
	out->y =
		0.0f //p2->y // * 1.0f
		+	( p3->y - p1->y ) * 0.5f * 1.0f
		+	( p1->y - 2.5f * p2->y + 2 * p3->y - 0.5f * p4->y ) * 2.0f * t
		+	( 3.0f * ( p2->y - p3->y ) - p1->y + p4->y ) * 0.5f * 3.0f * t2;
	out->z =
		0.0f // p2->z // * 1.0f
		+	( p3->z - p1->z ) * 0.5f * 1.0f
		+	( p1->z - 2.5f * p2->z + 2 * p3->z - 0.5f * p4->z ) * 2.0f * t
		+	( 3.0f * ( p2->z - p3->z ) - p1->z + p4->z ) * 0.5f * 3.0f * t2;
}

#define SPLINE_LENGTH_ERROR_TOLERANCE 0.001f

float AI_SplineGetSegLength( const AI_Spline &inSpline, const long seg ) // gets the length of the segment
{
	if( inSpline.lengths[ seg ] < 0.0f )
	{
		float error;
		float maxError = SPLINE_LENGTH_ERROR_TOLERANCE * ( AI_SplineGetPoint( inSpline, seg + 1 ) - AI_SplineGetPoint( inSpline, seg ) ).Mag();
		float oldLength, newLength;
		long loop, maxLoop;
		float delta;
		vector3 p1, p2;

		AI_SplineRef tempRef;
		tempRef.seg = seg;

		maxLoop = 4;
		delta = 0.25f;
		error = 1000.0f;

		while( error > SPLINE_LENGTH_ERROR_TOLERANCE )
		{
			newLength = 0.0f;

			p1 = inSpline.points[ seg ]; // get first point
			for( loop=0, tempRef.t = delta; tempRef.t <= 1.0f; tempRef.t += delta, loop++ )
			{
				AI_SplineGetPos( &p2, inSpline, tempRef );
				newLength += ( p2 - p1 ).Mag();
				p1 = p2;
			}
			Assert( loop == maxLoop );

			// get length of spline using new stuff....
			error = (float)fabs( oldLength - newLength );
			oldLength = newLength;
			maxLoop *= 2;
			delta /= 2;
		}
		inSpline.lengths[ seg ] = newLength;
	}
	return inSpline.lengths[ seg ];
}

float AI_SplineGetSegSectionLength( const AI_Spline &inSpline, const AI_SplineRef &ref ) // gets the length of the section up to t of the segment
{
	//DynamicArray<vector3> posTree;
	vector3 start = AI_SplineGetPoint( inSpline, ref.seg );
	vector3 end; AI_SplineGetPos( &end, inSpline, ref );

	return ( end - start ).Mag();
}

float AI_SplineGetSegLengthT( const AI_Spline &inSpline, const long seg, const float length ) // gets the t for the length of section of the segment
{
	//DynamicArray<vector3> posTree;
	vector3 start = AI_SplineGetPoint( inSpline, seg );
	vector3 end = AI_SplineGetPoint( inSpline, seg + 1 );

	float newT = length / ( end - start ).Mag();

	return newT < 1.0f ? newT : 1.0f;
}

float AI_MonoSplineGetPos( const AI_MonoSpline &inSpline, const AI_SplineRef &ref ) // gets the value at that point on the spline
{
	float t = ref.t;
	long seg = ref.seg;
	Assert( t >= 0.0f && t <= 1.0f );
	float t2, t3;
	t2 = t * t;
	t3 = t2 * t;
	
	float p1, p2, p3, p4;
	
	if( inSpline.circular )
	{
		Assert( seg >= 0 && seg <= inSpline.numVals );
		p1 = ( inSpline.vals[ ( seg - 1 + inSpline.numVals ) % inSpline.numVals ] );
		p2 = ( inSpline.vals[ ( seg + 0 + inSpline.numVals ) % inSpline.numVals ] );
		p3 = ( inSpline.vals[ ( seg + 1 + inSpline.numVals ) % inSpline.numVals ] );
		p4 = ( inSpline.vals[ ( seg + 2 + inSpline.numVals ) % inSpline.numVals ] );
	}
	else
	{
		Assert( seg > 0 && seg < inSpline.numVals - 2 ); // np = 4 seg > 0 == 1 seg < np-2 == 1
		p1 = ( inSpline.vals[ seg - 1 ] );
		p2 = ( inSpline.vals[ seg + 0 ] );
		p3 = ( inSpline.vals[ seg + 1 ] );
		p4 = ( inSpline.vals[ seg + 2 ] );
	}
	// n.b. numSegs == numVals - 3
	return
		p2 // * 1.0f
		+	( p3 - p1 ) * 0.5f * t
		+	( p1 - 2.5f * p2 + 2 * p3 - 0.5f * p4 ) * t2
		+	( 3.0f * ( p2 - p3 ) - p1 + p4 ) * 0.5f * t3;
}

float AI_MonoSplineGetDelta( const AI_MonoSpline &inSpline, const AI_SplineRef &ref ) // gets the delta value at that point on the spline.
{
	float t = ref.t;
	long seg = ref.seg;
	Assert( t >= 0.0f && t <= 1.0f );
	float t2;//, t3;
	t2 = t * t;
	//t3 = t2 * t3;
	
	float p1, p2, p3, p4;
	
	if( inSpline.circular )
	{
		Assert( seg >= 0 && seg <= inSpline.numVals );
		p1 = ( inSpline.vals[ ( seg - 1 + inSpline.numVals ) % inSpline.numVals ] );
		p2 = ( inSpline.vals[ ( seg + 0 + inSpline.numVals ) % inSpline.numVals ] );
		p3 = ( inSpline.vals[ ( seg + 1 + inSpline.numVals ) % inSpline.numVals ] );
		p4 = ( inSpline.vals[ ( seg + 2 + inSpline.numVals ) % inSpline.numVals ] );
	}
	else
	{
		Assert( seg > 0 && seg < inSpline.numVals - 2 ); // np = 4 seg > 0 == 1 seg < np-2 == 1
		p1 = ( inSpline.vals[ seg - 1 ] );
		p2 = ( inSpline.vals[ seg + 0 ] );
		p3 = ( inSpline.vals[ seg + 1 ] );
		p4 = ( inSpline.vals[ seg + 2 ] );
	}
	// n.b. numSegs == numPoints - 3
	return
		0.0f //*p2 // * 1.0f
		+	( p3 - p1 ) * 0.5f * 1.0f
		+	( p1 - 2.5f * p2 + 2 * p3 - 0.5f * p4 ) * 2.0f * t
		+	( 3.0f * ( p2 - p3 ) - p1 + p4 ) * 0.5f * 3.0f * t2;
}

AI_SplineInterface::AI_SplineInterface( SplineID ID, vector3 pos ) // creates interface and attaches to the spline.
{
	mySpline = ID; // ooh my spline!
	closestPoint = oldPosition = newPosition = pos; // same for both
	refAccurate = false; // course its not accurate
	myRef.seg = 1; // default seg value
	myRef.t = 0.0f; // default t value
}

AI_SplineInterface::~AI_SplineInterface()
{
	// nothing needs doing on exit
}


void AI_SplineInterface::SetNewPosition( vector3 newPos )
{
	newPosition = newPos;
	refAccurate = false;
}

SplineID AI_SplineManager::LoadSpline( char *name, double *list, long length )
{
	AI_Spline *s = new AI_Spline;
	SplineID sID = splineList.AddItem( s );
	s->name = new char[ strlen( name ) + 1 ];
	strcpy( s->name, name );
	s->circular = true;
	s->numPoints = length;
	s->points = new vector3[ length ];
	s->lengths = new float[ length ];
	s->width = NULL;
	long loop;
	for( loop = 0; loop < length; ++loop )
	{
		s->points[ loop ].x = (float)( list[ loop * 3 + 0 ] );
		s->points[ loop ].y = (float)( list[ loop * 3 + 2 ] ); // fix y,z translation thing....
		s->points[ loop ].z = (float)( list[ loop * 3 + 1 ] );
		s->lengths[ loop ] = -1.0f;
	}
	return sID;
}

#define MAXIMUM_POSSIBLE_WIDTH 100.0f

void AI_SplineManager::AddWidthDataToSpline( SplineID ID, SplineID widthSpline )
{
	AI_Spline &s = *splineList[ ID ];
	AI_Spline &w = *splineList[ widthSpline ];

	AI_MonoSpline *ms = new AI_MonoSpline;

	AI_SplineInterfaceID iID = AddInterface( widthSpline, w.points[ 0 ] );

	s.width = ms;

	ms->circular = s.circular;
	ms->name = NULL;
	ms->numVals = s.numPoints;
	ms->vals = new float[ ms->numVals ];
	
	vector3 tempPos;
	long loop;
	for( loop = 0; loop < s.numPoints; ++loop )
	{
		MoveTo( iID, s.points[ loop ] );
		ReLocate( iID );
		tempPos = ClosestSplinePoint( iID );
		ms->vals[ loop ] = ( tempPos - s.points[ loop ] ).Mag();
		Assert( ms->vals[ loop ] < MAXIMUM_POSSIBLE_WIDTH );
	}
	RemoveInterface( iID );
}

void AI_SplineManager::SetDirectionOfSpline( SplineID ID, vector3 pos, vector3 direction ) // uses a point in space and a direction to show the spline manager which way is forward on a spline
{
	AI_SplineInterfaceID iID = AddInterface( ID, pos );
	AI_SplineInterface &i = *interfaceList[ iID ];
	AI_Spline &s = *splineList[ ID ];
	
	//ReLocate( iID ); // this is unneccessary, cause it is in add interface
	
	vector3 trackDir;
	AI_SplineGetDelta( &trackDir, s, i.myRef );

	if( trackDir.DotProduct( &direction ) < 0.0f )
	{
		s.backwards = true;
	}
	else
	{
		s.backwards = false;
	}
	//asdf
}

void AI_SplineManager::UnloadSpline( SplineID ID )
{
	Assert( splineList[ ID ] != NULL );
	delete splineList[ ID ]->points;
	delete splineList[ ID ]->name;
	delete splineList[ ID ];
}

SplineID AI_SplineManager::FindSpline( char *name )
{
	SplineID sID = splineList.GetFirst();
	while( sID != WAREHOUSE_NULL_ID )
	{
		if( !strcmp( splineList[ sID ]->name, name ) )
		{
			return sID;
		}
	}
	return WAREHOUSE_NULL_ID;
}

AI_SplineInterfaceID AI_SplineManager::AddInterface( SplineID ID, vector3 pos )
{
	Assert( splineList[ ID ] != NULL );
	AI_SplineInterfaceID iID = interfaceList.AddItem( new AI_SplineInterface( ID, pos ) );
	ReLocate( iID );
	return iID;
}

void AI_SplineManager::RemoveInterface( AI_SplineInterfaceID ID )
{
	Assert( interfaceList[ ID ] != NULL );
	delete interfaceList[ ID ];
	interfaceList.RemoveItem( ID );
}

void AI_SplineManager::MoveTo( AI_SplineInterfaceID ID, vector3 newPos )
{
	interfaceList[ ID ]->SetNewPosition( newPos );
}

#define splineFindDeltaMin 0.001f

vector3 AI_SplineManager::ClosestSplinePoint( AI_SplineInterfaceID ID ) // closest position on spline to current point in interface
{
	AI_SplineInterface &i = *interfaceList[ ID ];
	AI_Spline &s = *splineList[ i.mySpline ];
	
	vector3 newRefPos, oldRefPos, tempDir, myDir;
	float oldDist, newDist;
	AI_SplineRef oldRef, newRef;

	if( i.refAccurate )
		return i.closestPoint;
	
	myDir = i.newPosition - i.oldPosition;
	//AI_SplineGetPos( &temp, *splineList[ i.mySpline ], i.myRef );
	AI_SplineGetDelta( &tempDir, s, i.myRef );

	bool forward;
	float delta = 1.0f;

	if( myDir.DotProduct( &tempDir ) > 0.0f )
	{
		// forward
		forward = true;
	}
	else
	{
		// backward
		forward = false;
		delta = -delta;
	}
	
	oldRef = newRef = i.myRef;			
	AI_SplineGetPos( &i.closestPoint, s, newRef );
	oldRefPos = i.closestPoint;
	oldDist = ( i.newPosition - i.closestPoint ).Mag();
	while( fabs( delta ) > splineFindDeltaMin )
	{
		newRef.t += delta;
		while( newRef.t < 0.0f )
		{
			newRef.t += 1.0f;
			newRef.seg -= 1;
			if( newRef.seg < 0 )
				newRef.seg += s.numPoints;
		}
		while( newRef.t > 1.0f )
		{
			newRef.t -= 1.0f;
			newRef.seg += 1;
			if( newRef.seg >= s.numPoints )
				newRef.seg -= s.numPoints;
		}

		AI_SplineGetPos( &newRefPos, s, newRef );
		newDist = ( i.newPosition - newRefPos ).Mag();
		if( newDist >= oldDist )
		{
			delta /= 2.0f;
			newRef = oldRef;
			continue;
		}
		else
		{ // success
			AI_SplineGetDelta( &tempDir, s, newRef );
			myDir = i.newPosition - newRefPos;
			if( myDir.DotProduct( &tempDir ) > 0.0f )
			{
				if( !forward )
				{
					// forward
					forward = true;
					delta = -delta;
				}
			}
			else
			{
				if( forward )
				{
					// backward
					forward = false;
					delta = -delta;
				}
			}
			oldDist = newDist;
			oldRef = newRef;
			oldRefPos = newRefPos;
		}
	}
	AI_SplineGetPos( &newRefPos, s, oldRef );
	i.myRef = oldRef;
	i.oldPosition = i.newPosition;
	i.closestPoint = newRefPos;
	i.refAccurate = true;
	return newRefPos;	
}

vector3 AI_SplineManager::PointAhead( AI_SplineInterfaceID ID, vector3 direction, float distanceAhead, AI_SplineInterfaceID outputID ) // position of spline distance world units away.
{
	AI_SplineInterface &i = *interfaceList[ ID ];
	AI_Spline &s = *splineList[ i.mySpline ];

	float decumulator = distanceAhead; // hehehe

	vector3 splineDir;
	AI_SplineRef newRef;
	AI_SplineGetDelta( &splineDir, s, i.myRef );
	if( splineDir.DotProduct( &direction ) < 0.0f )
	{
		decumulator = -decumulator;
	}

	newRef = i.myRef;

	decumulator += AI_SplineGetSegSectionLength( s, newRef );
	newRef.t = 0.0f;

	if( decumulator > 0 )
	{
		// keep going forward until we run out of length;
		while( decumulator > 0 )
		{
			decumulator -= AI_SplineGetSegLength( s, newRef.seg );
			newRef.seg++;
		}
		// jump back one,
		newRef.seg--;
		if( newRef.seg < 0 )
			newRef.seg += s.numPoints;
		decumulator += AI_SplineGetSegLength( s, newRef.seg );
		
		// find the t distance to the right length...
		// can do that outside the "if"				
	}
	else
	{
		// keep going backwards until we run out of length;
		while( decumulator < 0 )
		{
			newRef.seg--;
			if( newRef.seg < 0 )
				newRef.seg += s.numPoints;
			decumulator += AI_SplineGetSegLength( s, newRef.seg );
		}
		// find the t distance to bring the length back up to zero...
		// can do that outside the "if"
	}

	// find the t required to match the length.
	newRef.t = AI_SplineGetSegLengthT( s, newRef.seg, decumulator );

	vector3 temp;
	AI_SplineGetPos( &temp, s, newRef );
	if( outputID != WAREHOUSE_NULL_ID )
	{
		AI_SplineInterface &oi = *interfaceList[ outputID ];
		Assert( oi.mySpline == i.mySpline );
		oi.refAccurate = true;
		oi.closestPoint = temp;
		
		// shouldn't need to do the following but will in case some gimp does something silly...
		oi.myRef = newRef;
		oi.newPosition = temp;
		oi.oldPosition = temp;	
	}
	return temp;
}

vector3 AI_SplineManager::PointForward( AI_SplineInterfaceID ID, float distanceForward, AI_SplineInterfaceID outputID ) // position of spline distance world units away.
{
	AI_SplineInterface &i = *interfaceList[ ID ];
	AI_Spline &s = *splineList[ i.mySpline ];

	float decumulator = distanceForward; // hehehe

	//vector3 splineDir;
	AI_SplineRef newRef;
	//AI_SplineGetDelta( &splineDir, s, i.myRef );
	if( s.backwards )
	{
		decumulator = -decumulator;
	}

	newRef = i.myRef;

	decumulator += AI_SplineGetSegSectionLength( s, newRef );
	newRef.t = 0.0f;

	if( decumulator > 0 )
	{
		// keep going forward until we run out of length;
		while( decumulator > 0 )
		{
			decumulator -= AI_SplineGetSegLength( s, newRef.seg );
			newRef.seg++;
		}
		// jump back one,
		newRef.seg--;
		if( newRef.seg < 0 )
			newRef.seg += s.numPoints;
		decumulator += AI_SplineGetSegLength( s, newRef.seg );
		
		// find the t distance to the right length...
		// can do that outside the "if"				
	}
	else
	{
		// keep going backwards until we run out of length;
		while( decumulator < 0 )
		{
			newRef.seg--;
			if( newRef.seg < 0 )
				newRef.seg += s.numPoints;
			decumulator += AI_SplineGetSegLength( s, newRef.seg );
		}
		// find the t distance to bring the length back up to zero...
		// can do that outside the "if"
	}

	// find the t required to match the length.
	newRef.t = AI_SplineGetSegLengthT( s, newRef.seg, decumulator );

	vector3 temp;
	AI_SplineGetPos( &temp, s, newRef );
	if( outputID != WAREHOUSE_NULL_ID )
	{
		AI_SplineInterface &oi = *interfaceList[ outputID ];
		Assert( oi.mySpline == i.mySpline );
		oi.refAccurate = true;
		oi.closestPoint = temp;
		
		// shouldn't need to do the following but will in case some gimp does something silly...
		oi.myRef = newRef;
		oi.newPosition = temp;
		oi.oldPosition = temp;
	}
	return temp;
}

float AI_SplineManager::WidthAtPoint( AI_SplineInterfaceID ID ) // returns the width of the road at the given interface.
{
	if( ID != WAREHOUSE_NULL_ID )
	{
		Assert( splineList[ interfaceList[ ID ]->mySpline ]->width != NULL );
		return AI_MonoSplineGetPos( *(splineList[ interfaceList[ ID ]->mySpline ]->width), interfaceList[ ID ]->myRef );
	}
	return -1.0f;
}

vector3 AI_SplineManager::LeftEdge( AI_SplineInterfaceID ID ) // returns the point of the left edge of the road...
{
	float w = WidthAtPoint( ID );
	AI_SplineInterface &i = *interfaceList[ ID ];
	AI_Spline &s = *splineList[ i.mySpline ];

	vector3 direction, pos;

	AI_SplineGetDelta( &direction, s, i.myRef );

	direction.y = direction.x;
	direction.x = direction.z;
	direction.z = -direction.y;
	direction.y = 0;

	direction.Normalise();

	pos = i.closestPoint + direction * w;

	return pos;
}

vector3 AI_SplineManager::RightEdge( AI_SplineInterfaceID ID ) // returns the point of the left edge of the road...
{
	float w = WidthAtPoint( ID );
	AI_SplineInterface &i = *interfaceList[ ID ];
	AI_Spline &s = *splineList[ i.mySpline ];

	vector3 direction, pos;

	AI_SplineGetDelta( &direction, s, i.myRef );

	direction.y = direction.x;
	direction.x = -direction.z;
	direction.z = direction.y;
	direction.y = 0;

	direction.Normalise();

	pos = i.closestPoint + direction * w;

	return pos;
}

void AI_SplineManager::ReLocate( AI_SplineInterfaceID ID ) // dumps old info and finds REAL closest point on spline...
{
	AI_SplineInterface &i = *interfaceList[ ID ];
	AI_Spline &s = *splineList[ i.mySpline ];

	long loop;
	long closestID;
	float closestDist;

	float testDist;

	closestID = 0;
	closestDist = ( i.newPosition - AI_SplineGetPoint( s, closestID ) ).Mag();
	
	// find the closest segments by looping through all the segments and seeing which is closest
	for( loop = 0; loop < s.numPoints; ++loop )
	{
		testDist = ( i.newPosition - AI_SplineGetPoint( s, loop ) ).Mag();
		
		if( closestDist > testDist )
		{
			closestID = loop;
			closestDist = testDist;
		}		
	}
	
	i.myRef.seg = closestID;
	i.myRef.t = 0.0f;

	i.closestPoint = AI_SplineGetPoint( s, i.myRef.seg );
	i.oldPosition = i.closestPoint;
	i.refAccurate = false;
	ClosestSplinePoint( ID );
}

