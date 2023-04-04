// $Header$

// $Log$
// Revision 1.0  2002-07-22 11:25:27+01  jjs
// Initial revision
//
// Revision 1.1  2000-01-07 16:09:27+00  jjs
// Modified hunk to add VertexBuffers.
//

//================================================================================================================================
// hunk.h
// ------
//			A hunk is a XZ 2d-box containing a list of triangles which occur within this list.
//================================================================================================================================

#ifndef _BS_hunk
#define _BS_hunk

class hunk
{
	friend class world;
	friend class engine;
	friend class SphereSet;
	friend class Sphere;

	public:
		hunk();
		~hunk();

	private:
		triangle ** tlistPtr_N,  ** nextTriangle_N,  ** tlistPtr_W, ** nextTriangle_W;		
		slong tcount_N, tcount_W;

		material  * mlistPtr;		// all materials in the mesh
		slong	  mcount;			// total materialse in the mesh

		LPDIRECT3DVERTEXBUFFER8 vertBuf;
		int vbcount;
		int vertStride;

		/*LPWORD*/IDirect3DIndexBuffer8 **indices;
		WORD *indcount;
};

#endif	// _BS_hunk

//================================================================================================================================
//END OF FILE
//================================================================================================================================
