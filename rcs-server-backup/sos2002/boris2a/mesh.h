// $Header$

// $Log$
// Revision 1.1  2002-07-22 10:06:39+01  jjs
// Test version of SOS2002
//
// Revision 1.1  2000-01-07 16:13:53+00  jjs
// Now adds VertexBuffers.
//

//================================================================================================================================
// mesh.h
// ------
//
//================================================================================================================================

#ifndef _BS_mesh
#define _BS_mesh


class mesh {

	friend class engine;
	friend class world;
	friend class edge;

public:
		mesh();
		~mesh();

		float MinCoordX, MaxCoordX;
		float MinCoordY, MaxCoordY;
		float MinCoordZ, MaxCoordZ;

		triangle  * tlistPtr;		// all triangles in the mesh
		vertex    * vlistPtr;		// all vertices in the mesh
		slong     tcount;			// total triangles in mesh
		slong     vcount;			// total vertices in mesh
	
		material  * mlistPtr;		// all materials in the mesh
		slong	  mcount;			// total materialse in the mesh

		SphereSet * spheres;		// holds collision bounding information IFF it exists.

		vector3 * geoDataPtr;

		bool hasTexture(char *name);
		void copyfrom(mesh *m);

		LPDIRECT3DVERTEXBUFFER8 vertBuf;
		int vbcount;
		int vertStride;

		IDirect3DIndexBuffer8 **indices;
		WORD *indcount;

private:
	
};

#endif	// _BS_mesh

//================================================================================================================================
//END OF FILE
//================================================================================================================================
