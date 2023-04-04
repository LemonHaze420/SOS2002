//================================================================================================================================
// mesh.cpp
// ----------
//
//================================================================================================================================

#include "BS2all.h"

mesh::mesh()
{
	tlistPtr = NULL;
	vlistPtr = NULL;
	mlistPtr = NULL;
	geoDataPtr=NULL;
}
  
extern void dprintf(char *,...);
#define	dprintf(x)

mesh::~mesh()
{
	long i;

	dprintf("deleting triangles %lx",tlistPtr);
	if (tlistPtr) 
	{
		delete[] tlistPtr;
		tlistPtr=NULL;
	}
	dprintf("deleting vertices %lx",vlistPtr);
	if (vlistPtr) 
	{	
		delete[] vlistPtr;
		vlistPtr=NULL;
	}
	dprintf("deleting materials %lx",mlistPtr);
	if (mlistPtr) 
	{
		delete[] mlistPtr;
		mlistPtr=NULL;
	}
	dprintf("deleting geodata %lx",geoDataPtr);
	if( geoDataPtr )
	{
		delete[] geoDataPtr;
		geoDataPtr=NULL;
	}
}

void mesh::copyfrom(mesh *m)
{
	if(m->tlistPtr)
	{
		tcount = m->tcount;
		tlistPtr = new triangle [tcount];
		memcpy((void *)tlistPtr,(const void *)m->tlistPtr,sizeof(triangle)*tcount);
	}

	if(m->vlistPtr)
	{
		vcount = m->vcount;
		vlistPtr = new vertex[vcount];
		memcpy((void *)vlistPtr,(const void *)m->vlistPtr,sizeof(vertex)*vcount);
	}

	if(m->mlistPtr)
	{
		mcount = m->mcount;
		mlistPtr = new material [mcount];
		memcpy((void *)mlistPtr,(const void *)m->mlistPtr,sizeof(material)*mcount);
	}


	// v3list copy todo 
}

bool mesh::hasTexture(char *name)
{
	char buf[256];
	for(int i=0;i<mcount;i++)
	{
		if(mlistPtr[i].textureDataPtr)
		{
			sprintf(buf,"checking %s = %s\n",mlistPtr[i].textureDataPtr->origfilename,name);OutputDebugString(buf);
			if(!strcmp(mlistPtr[i].textureDataPtr->origfilename,name))
				return true;
		}
	}
	OutputDebugString("NO MATCH\n");
	return false;
}

//================================================================================================================================
// END OF FILE
//================================================================================================================================
