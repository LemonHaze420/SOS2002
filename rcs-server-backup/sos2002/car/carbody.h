/*
	$Header$
	$Log$
	Revision 1.0  2000-02-16 14:51:20+00  jjs
	Initial revision

	Revision 1.2  1999/01/12 11:45:29  jcf
	This version seems to work

*/

//================================================================================================================================
// carbody.h
// ---------
//
//================================================================================================================================

#ifndef _BS_carbody
#define _BS_carbody

class carbody {
	
	public:	
		//carbody(){};
		~carbody(){};

		carbody( object * whichObject );


	//private:
		vector3 exhaustCloudPoint;				// body-relative co-ordinate of exahust cloud
		vector3 transformedexhaustCloudPoint;

		object * bodyObject;


		vector3 enginePosition;		// offset of engine centre in body

		bool getWorldHeight( float * yvalue, triangle ** resulttriPtr);

		void transformDirectionVectorToWorld( vector3 * r, vector3 * v );	
		void transformPositionVectorToWorld( vector3 * r, vector3 * v );

		vector3 * getPositionInWorld();		// get position of body centroid in world co-ordinates


};

#endif	// _BS_carbody

//================================================================================================================================
//END OF FILE
//================================================================================================================================
