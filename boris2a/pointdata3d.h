//================================================================================================================================
// pointdata3d.h
// -------------
//			
//================================================================================================================================

#ifndef _BS_pointdata3d
#define _BS_pointdata3d

class pointdata3d
{
	friend class engine;
	friend class triangle;
	friend class world;

	public:
		pointdata3d() {};
		~pointdata3d() {};

		vertex  *v;
		nongeodata nv;
	
	private:

};


#endif	// _BS_pointdata3d

//================================================================================================================================
//END OF FILE
//================================================================================================================================
