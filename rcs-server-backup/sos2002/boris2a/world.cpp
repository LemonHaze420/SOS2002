// $Header$

// $Log$
// Revision 1.1  2002-07-22 10:06:36+01  jjs
// Test version of SOS2002
//
// Revision 1.1  2000-02-18 09:58:52+00  jjs
// vertex buffers working for world and cars.
//

//================================================================================================================================
// world.cpp
// ---------
//
//================================================================================================================================

#include "BS2all.h"

extern void dprintf(char *,...);
#define	dprintf(x)

float mz = 0;

world::world( char * filename, float scale,
			  float hunksizex, float hunksizez  )
{
	hunk * hunkPtr;
	slong i;
	slong loHunkX, hiHunkX,  loHunkZ, hiHunkZ; 

	slong xx, zz;
	hunk  * hunkPtrX,  * hunkPtrZ;

	float hx, hz;
	vector3 * pcoord, * qcoord, * rcoord;
//	overlaytriangle ** OvlPtrX, ** OvlPtrZ;

	triangle * thisTrianglePtr;


	xhunksize = hunksizex * scale; 
	zhunksize = hunksizez * scale;
	oneoverxhunksize = RECIPROCAL( xhunksize );
	oneoverzhunksize = RECIPROCAL( zhunksize );

	//loadMulderFile( filename, &meshData, scale );
	worldModel = getModelPtr( filename, scale, false, "" );

	if(boris2->useRacingline)
	{
		loadRacingLineSet(filename,scale);
	}

		//
		// each overlay point (px,pz) has a coord given by:
		//
		//				worldMinCoordX +  px * mapXsize,
		//				worldMinCoordZ +  pz * mapZsize;
		// 
		// and a pointer to the appropriate triangle by:
		//
		// overlayMap[pz * mapXcount + px] 
		//
		//
		// in overlaytriangle:  Avalue = -A/B   
		//						Cvalue = -C/B
		//						Dvalue = -D/B




		//
		// create the hunk lists of triangles
		//

	float minx, maxx, minz, maxz;

		// allocate a sufficient number of hunks for the world

		// calculate number of XZ hunks required to cover the world
		// (looking at mesh 0 - the most detailed level)

	mesh * meshData = worldModel->meshData;

	xhunks = (slong)((meshData->MaxCoordX - meshData->MinCoordX) * oneoverxhunksize) + 1;
	zhunks = (slong)((meshData->MaxCoordZ - meshData->MinCoordZ) * oneoverzhunksize) + 1;
	
	hunkPtr = hunkList = new hunk[ totalhunks = xhunks * zhunks + 1];	// +1 for the mountain
					// clear the counts in all hunks to 0 initially

	dprintf("hunks %d %d",xhunks,zhunks);

	for( i = totalhunks; i--; )
	{
			hunkPtr->tcount_W = hunkPtr->tcount_N = 0;
			hunkPtr++;
	}

	mountainHunk = &hunkList[totalhunks - 1];
		//
		// PASS 1:
		// examine each triangle, increment the count of every appropriate hunk list
		//

	thisTrianglePtr = meshData->tlistPtr;
	for (i = meshData->tcount; i--; )
	{
		bool wallTriangle = thisTrianglePtr->mtl->bm.flags & MAT_WALL;
		bool mountainTriangle = thisTrianglePtr->mtl->bm.flags & (MAT_MAXZM1 | MAT_MAXZ);

		if(mountainTriangle)
			mountainHunk->tcount_N++;
		else
		{
			pcoord = thisTrianglePtr->p.v->geoCoord;
			qcoord = thisTrianglePtr->q.v->geoCoord;
			rcoord = thisTrianglePtr->r.v->geoCoord;
			
			// Find the XZ bound of this triangle
			
			minmax3( pcoord->x,
				qcoord->x,
				rcoord->x, &minx, &maxx );
			
			minmax3( pcoord->z,
				qcoord->z,
				rcoord->z, &minz, &maxz );
			
			// get range of hunks whuch encompass this triangles bounds
			
			
			FASTFTOL(&loHunkX , (minx - meshData->MinCoordX) * oneoverxhunksize - 0.5f);
			FASTFTOL(&hiHunkX , (maxx - meshData->MinCoordX) * oneoverxhunksize + 0.5f);
			
			FASTFTOL(&loHunkZ , (minz - meshData->MinCoordZ) * oneoverzhunksize - 0.5f) ;
			FASTFTOL(&hiHunkZ , (maxz - meshData->MinCoordZ) * oneoverzhunksize + 0.5f) ;
			
			if( hiHunkX >= xhunks) hiHunkX = xhunks-1;
			if( hiHunkZ >= zhunks) hiHunkZ = zhunks-1;
			
			
			// test current triangle against every hunk in its bounding rectangle, if it is visible then
			// simply increment the count here....
			
			hx = loHunkX * xhunksize + meshData->MinCoordX ; //minx;
			hunkPtrX = hunkList + loHunkX*zhunks;
			for( xx = 1 + hiHunkX - loHunkX; xx--; )
			{
				
				if((((pcoord->x < hx) ? 1 : ((pcoord->x >= (hx+xhunksize)) ? 2 : 0 )) &
					((qcoord->x < hx) ? 1 : ((qcoord->x >= (hx+xhunksize)) ? 2 : 0 )) &
					((rcoord->x < hx) ? 1 : ((rcoord->x >= (hx+xhunksize)) ? 2 : 0 )) ) == 0)
				{
					
					hz = loHunkZ * zhunksize + meshData->MinCoordZ; //minz;
					hunkPtrZ = hunkPtrX + loHunkZ;
					for(  zz = 1 + hiHunkZ - loHunkZ; zz--; )
					{
						if((((pcoord->z < hz) ? 1 : ((pcoord->z >= (hz+zhunksize)) ? 2 : 0 )) &
							((qcoord->z < hz) ? 1 : ((qcoord->z >= (hz+zhunksize)) ? 2 : 0 )) &
							((rcoord->z < hz) ? 1 : ((rcoord->z >= (hz+zhunksize)) ? 2 : 0 )) ) == 0)	
						{
							// hunkPtrZ = pointer to hunk which (partially) contains the current triangle
							// so increment its counter.
							
							if( wallTriangle )
								hunkPtrZ->tcount_W++;
							else
								hunkPtrZ->tcount_N++;
						}
						hz += zhunksize;
						hunkPtrZ++;
					}
				}
				hx += xhunksize;
				hunkPtrX += zhunks;
			}
		}
		thisTrianglePtr++;
	}


		//
		//	Allocate local hunk lists
		//

	hunkPtr = hunkList;
	for( i = totalhunks; i--; )
	{
		//if (hunkPtr->tcount)
			//DBGR.Dprintf("%f ... %f ",i, (float)hunkPtr->tcount);

		hunkPtr->tlistPtr_N = new triangle *[hunkPtr->tcount_N];
		hunkPtr->tlistPtr_W = new triangle *[hunkPtr->tcount_W];

		hunkPtr->nextTriangle_N = hunkPtr->tlistPtr_N;// point one after end (for loop algorithms)
		hunkPtr->nextTriangle_W = hunkPtr->tlistPtr_W;// point one after end (for loop algorithms)
		hunkPtr++;
	}


		//
		// PASS2:
		//			As PASS1 but actually attach the triangle
		//
	

	thisTrianglePtr = meshData->tlistPtr;
	for (i = meshData->tcount; i--; )
	{

		bool wallTriangle = thisTrianglePtr->mtl->bm.flags & MAT_WALL;
		bool mountainTriangle = thisTrianglePtr->mtl->bm.flags & (MAT_MAXZM1 | MAT_MAXZ);

		if(mountainTriangle)
			*(mountainHunk->nextTriangle_N++) = thisTrianglePtr;
		else
		{
			pcoord = thisTrianglePtr->p.v->geoCoord;
			qcoord = thisTrianglePtr->q.v->geoCoord;
			rcoord = thisTrianglePtr->r.v->geoCoord;
			
			// Find the XZ bound of this triangle
			
			minmax3( pcoord->x,
				qcoord->x,
				rcoord->x, &minx, &maxx );
			
			minmax3( pcoord->z,
				qcoord->z,
				rcoord->z, &minz, &maxz );
			
			// get range of hunks whuch encompass this triangles bounds
			
			FASTFTOL(&loHunkX , (minx - meshData->MinCoordX) * oneoverxhunksize - 0.5f) ;
			FASTFTOL(&hiHunkX , (maxx - meshData->MinCoordX) * oneoverxhunksize + 0.5f);
			
			FASTFTOL(&loHunkZ , (minz - meshData->MinCoordZ) * oneoverzhunksize - 0.5f) ;
			FASTFTOL(&hiHunkZ , (maxz - meshData->MinCoordZ) * oneoverzhunksize + 0.5f) ;
			
			if( hiHunkX >= xhunks) hiHunkX = xhunks-1;
			if( hiHunkZ >= zhunks) hiHunkZ = zhunks-1;
			
			
			// test current triangle against every hunk in its bounding rectangle, if it is visible then
			// simply increment the count here....
			
			hx = loHunkX*xhunksize + meshData->MinCoordX; // minx
			hunkPtrX = hunkList + loHunkX*zhunks;
			for( xx = 1 + hiHunkX - loHunkX; xx--; )
			{
				
				if((((pcoord->x < hx) ? 1 : ((pcoord->x >= (hx+xhunksize)) ? 2 : 0 )) &
					((qcoord->x < hx) ? 1 : ((qcoord->x >= (hx+xhunksize)) ? 2 : 0 )) &
					((rcoord->x < hx) ? 1 : ((rcoord->x >= (hx+xhunksize)) ? 2 : 0 )) ) == 0)
				{
					
					hz = loHunkZ*zhunksize + meshData->MinCoordZ; //minz;
					hunkPtrZ = hunkPtrX + loHunkZ;
					for( zz = 1 + hiHunkZ - loHunkZ; zz--; )
					{
						if((((pcoord->z < hz) ? 1 : ((pcoord->z >= (hz+zhunksize)) ? 2 : 0 )) &
							((qcoord->z < hz) ? 1 : ((qcoord->z >= (hz+zhunksize)) ? 2 : 0 )) &
							((rcoord->z < hz) ? 1 : ((rcoord->z >= (hz+zhunksize)) ? 2 : 0 )) ) == 0)	
						{
							
							// hunkPtrZ = pointer to hunk which (partially) contains the current triangle
							// so place the pointer to the triangle in the hunks list of pointers to triangles
							
							if(wallTriangle)
								*(hunkPtrZ->nextTriangle_W++) = thisTrianglePtr;
							else
								*(hunkPtrZ->nextTriangle_N++) = thisTrianglePtr;
						}
						hz += zhunksize;
						hunkPtrZ++;
					}
				}
				hx += xhunksize;
				hunkPtrX += zhunks;
			}
		}
			//
			// Phew..
			//
		thisTrianglePtr++;
	}

	makeVertexBuffers();

	sunOn = false;

}

extern LPD3DVERTEXC jjsVert;
extern int numjjsVert;
WORD Addvertex(pointdata3d &p);

int world::getMat(material *m, material *p)
{
	for(int i=0;;++i)
		if(m == p++)
			return i;

}


void world::makeVertexBuffers(void)
{
	LPWORD *indBuf;
	WORD* indCount;
	WORD ind0, ind1, ind2;
	int i;

	mesh * meshData = worldModel->meshData;
	hunk* hunkPtr = hunkList;

	// Allocate temporary vertex buffers
	jjsVert = new D3DVERTEXC[meshData->tcount * 3];
	numjjsVert = 0;
	
	indBuf = new LPWORD[meshData->mcount];
	for(i=0; i<meshData->mcount; ++i)
		indBuf[i] = new WORD[meshData->tcount * 3];
	indCount = new WORD[meshData->mcount];

	for(i = 0 ; i < totalhunks ; ++i, ++hunkPtr)
	{
		int j,k,mat;

		if(hunkPtr->tcount_N + hunkPtr->tcount_W)
		{
			memset(indCount,0,sizeof(WORD) * meshData->mcount);

			triangle** ground = hunkPtr->tlistPtr_N;
			triangle** wall = hunkPtr->tlistPtr_W;

			hunkPtr->mlistPtr = meshData->mlistPtr;
			hunkPtr->mcount = meshData->mcount;
			numjjsVert = 0;

			for(j = 0 ; j < hunkPtr->tcount_N; ++j, ++ground)
			{
				ind0 = Addvertex((*ground)->p);
				ind1 = Addvertex((*ground)->q);
				ind2 = Addvertex((*ground)->r);

				mat = getMat((*ground)->mtl, hunkPtr->mlistPtr);

				indBuf[mat][indCount[mat]++] = ind0;
				indBuf[mat][indCount[mat]++] = ind1;
				indBuf[mat][indCount[mat]++] = ind2;
			}

			for(j = 0 ; j < hunkPtr->tcount_W; ++j, ++wall)
			{
				ind0 = Addvertex((*wall)->p);
				ind1 = Addvertex((*wall)->q);
				ind2 = Addvertex((*wall)->r);

				mat = getMat((*wall)->mtl, hunkPtr->mlistPtr);

				indBuf[mat][indCount[mat]++] = ind0;
				indBuf[mat][indCount[mat]++] = ind1;
				indBuf[mat][indCount[mat]++] = ind2;
			}

			//LPWORD[meshData->mcount];
			hunkPtr->indices = new IDirect3DIndexBuffer8 *[meshData->mcount];
				
			hunkPtr->indcount = new WORD[meshData->mcount];
			
			HRESULT hr;
			unsigned char *ptr;
		/*	D3DVERTEXBUFFERDESC vbdesc;
			ZeroMemory(&vbdesc, sizeof(D3DVERTEXBUFFERDESC));
			vbdesc.dwSize= sizeof(D3DVERTEXBUFFERDESC);
			vbdesc.dwCaps        = D3DVBCAPS_WRITEONLY;
			vbdesc.dwFVF         = D3DFVF_VERTEXC;
			vbdesc.dwNumVertices = numjjsVert;
			
			// Create a clipping-capable vertex buffer.
			if(FAILED(g_pD3D->CreateVertexBuffer(&vbdesc, &hunkPtr->vertBuf, 0L)))
				dprintf("No vbuff\n");*/
			
			//IDirect3DVertexBuffer8 *vbdesc;
			if (FAILED(g_pd3dDevice->CreateVertexBuffer(numjjsVert * sizeof(D3DVERTEXC), D3DUSAGE_WRITEONLY, D3DFVF_VERTEXC, D3DPOOL_DEFAULT, &hunkPtr->vertBuf)))
			{
				dprintf("No vbuff\n");
				return;
			}
			
		
			hr = hunkPtr->vertBuf->Lock(0, 0, &ptr, 0);
			if (FAILED(hr))
				dprintf("Failed vb lock\n");

			memcpy(ptr,jjsVert,sizeof(D3DVERTEXC) * numjjsVert);
			
			
			hunkPtr->vertBuf->Unlock();

			hunkPtr->vertStride = sizeof(D3DVERTEXC);
			hunkPtr->vbcount = numjjsVert;

			LPDIRECT3DINDEXBUFFER8 tmp;

			for(k=0;k<hunkPtr->mcount; ++k)
			{
				void *m_indPtr;

				if (indCount[k] > 0) {
					if (FAILED(g_pd3dDevice->CreateIndexBuffer(indCount[k] * sizeof(WORD), D3DUSAGE_DYNAMIC |D3DUSAGE_WRITEONLY , D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM /*D3DPOOL_DEFAULT*/, &hunkPtr->indices[k]))) {
						dprintf("failed CreateIndexBuffer\n"); continue;	
					}
				
					if (FAILED(hunkPtr->indices[k]->Lock(0, indCount[k] * sizeof(WORD), (BYTE **)&m_indPtr, 0 ))) {
						dprintf("index buffer lock\n"); continue;
					}

					memcpy(m_indPtr, indBuf[k], indCount[k] * sizeof(WORD));
					hunkPtr->indcount[k] = indCount[k];
					hunkPtr->indices[k]->Unlock();
				} else {
					hunkPtr->indices[k] = NULL;
					hunkPtr->indcount[k] = indCount[k];
				}
			}	
		}
	}
	// Clear up temporary buffers
	for(i=0;i<meshData->mcount; ++i)
		delete[] indBuf[i];
	delete[] jjsVert;
	delete[] indBuf;
	delete[] indCount;
}

//
//
//

bool  yfound;
float bestycoord;
uchar bestyside;
triangle * bestyTri;


#define UPTOLERANCE 1.0f

#define	OLDPLONK 0

// scans the triangle, looking for the flags described in the scanset. If a flag is set, it sets the corresponding
// boolean and returns true or false depending on whether the 'considerasground' bool in the scanset is set. This
// is set when a triangle with that flag should still be considered to be a ground polygon.
// Otherwise true is returned, since an unflagged triangle should always be considered as ground

bool __inline scanTrisForFlags(triangle *tri,scanset *scan,int numscans)
{
	int i;
#if OLDPLONK

	bool rv = !(tri->mtl->bm.flags & (MAT_DECAL | MAT_REVERBLO | MAT_REVERBHI | MAT_SHADOW |
			MAT_BLACKFLAG | MAT_REVLIMIT ));
	for(i=0;i<numscans;i++)
	{
		if(scan->flag & tri->mtl->bm.flags)
		{
			*(scan->ptr) = true;
			if(!scan->considerasground) rv=false;
		}
		scan++;
	}
#else

	bool rv = (tri->mtl->bm.flags & MAT_GROUND);	// only consider GROUND as ground

	for(i=0;i<numscans;i++)
	{
		if(scan->flag & tri->mtl->bm.flags)
		{
			*(scan->ptr) = true;
			if(!scan->considerasground) rv=false;
		}
		scan++;
	}
#endif
	return rv;
}



bool world::scanTrianglesForXZ( triangle **thistlist, ulong total, vector3 * v, float * yresult, triangle ** resulttriPtr,scanset *scan,int numscans)
{
	//static   vertex   * pvPtr, * qvPtr, * rvPtr;
	long kk;

	triangle * triPtr;
	vector3 * pcoord, *qcoord, * rcoord;
	bool  tcondn;
	float ycoord;
	uchar yside;

	if(scan)
	{
		for(int i=0;i<numscans;i++)	*(scan[i].ptr) = false;
	}

	if(total)
		for (kk = total; kk--; thistlist++)
		{
			triPtr = *thistlist;

			if(  (triPtr->minLimit.x <= v->x) && (triPtr->maxLimit.x >= v->x) &&
				 (triPtr->minLimit.z <= v->z) && (triPtr->maxLimit.z >= v->z)   )
			{
				if( (triPtr->faceNormal.y )	&& ( (triPtr->mtl->bm.flags & (
					MAT_WATER | MAT_DRIVINGAIDS ) ) == 0 )) // triangle plane is NOT vertical, nor water
				{
					// point is within bounding box of triangle so now lets test with 2x2 determinants to see if the
					// XZ point is within the droppped XZ triangle.

					pcoord = triPtr->p.v->geoCoord;
					qcoord = triPtr->q.v->geoCoord;
					rcoord = triPtr->r.v->geoCoord;

					float Xtp = pcoord->x - v->x;
					float Ztp = pcoord->z - v->z;
					float Xup = qcoord->x - v->x;
					float Zup = qcoord->z - v->z;
					float Xvp = rcoord->x - v->x;
					float Zvp = rcoord->z - v->z;
			
					if( Xtp*Zup  >  Xup*Ztp )
					{
						tcondn = ( ( Xup*Zvp >= Xvp*Zup) &&  ( Xvp*Ztp >= Xtp*Zvp) );
					}
					else
					{
						tcondn = ( ( Xup*Zvp <= Xvp*Zup) &&  ( Xvp*Ztp <= Xtp*Zvp) );
					}

					if(tcondn)
					{
						ycoord    = (float)(triPtr->minusOneOverB * (
								    triPtr->A * v->x +
								    triPtr->C * v->z +
								    triPtr->planeD ));

						yside = ( (v->y + UPTOLERANCE) >= ycoord ) ? 1 : 2;
						if( yfound )
						{			
										// .. we already have a value held.
							if( yside == bestyside )
							{
										// both on same side 
										
								if( yside == 1)
								{
									if(scanTrisForFlags(triPtr,scan,numscans) && 
										(  ycoord > bestycoord ))
									{
										bestycoord = ycoord;
										bestyside  = yside;
										bestyTri   = triPtr;
										yfound     = true;		
									}
								}
								else
								{
									if(scanTrisForFlags(triPtr,scan,numscans) && 
										(  ycoord < bestycoord ))
									{
										bestycoord = ycoord;
										bestyside  = yside;
										bestyTri   = triPtr;
										yfound     = true;		

									}
								}
							} 
							else
							{
								if( yside == 1 )		// new coord ABOVE, ( and old coord below)
								{
									if(scanTrisForFlags(triPtr,scan,numscans))
									{
										bestycoord = ycoord;
										bestyside  = yside;
										bestyTri   = triPtr;
										yfound     = true;
									}
								}		// else (old coord ABOVE new below so no change
							}
						} 
						else
						{
							if(scanTrisForFlags(triPtr,scan,numscans))
							{
								bestycoord = ycoord;
								bestyside  = yside;
								bestyTri   = triPtr;
								yfound = true;
							}
						}
					}
				}
			}
		}

	if(yfound)
	{

		*resulttriPtr = bestyTri;//->mtl->bm.flags;
		*yresult = bestycoord;
		return( true );
	}


	return( false );
}

//================================================================================================================================
//================================================================================================================================

bool world::isPointUnderwater( vector3 * v, float * depth, triangle ** resulttriPtr )
{
//return(true);

	slong hunkX, hunkZ;
	hunk * hunkPtr;
	
		// Find the hunk which contains this XZ point..

	mesh * meshData = worldModel->meshData;	// using most detailed lod

	FASTFTOL(&hunkX , (v->x - meshData->MinCoordX) * oneoverxhunksize - 0.5f);
	FASTFTOL(&hunkZ , (v->z - meshData->MinCoordZ) * oneoverzhunksize - 0.5f);
		
	if( hunkX >= xhunks) hunkX = xhunks-1;
	if( hunkZ >= zhunks) hunkZ = zhunks-1;

	hunkPtr = hunkList + hunkX*zhunks + hunkZ;


	long kk;

	triangle * triPtr;
	vector3 * pcoord, *qcoord, * rcoord;
	bool  tcondn;
	float ycoord;

	triangle ** thistlist = hunkPtr->tlistPtr_N;

	if(hunkPtr->tcount_N)
	for (kk = hunkPtr->tcount_N; kk--; thistlist++)
	{
	    triPtr = *thistlist;

		if(  (triPtr->minLimit.x <= v->x) && (triPtr->maxLimit.x >= v->x) &&
			 (triPtr->minLimit.z <= v->z) && (triPtr->maxLimit.z >= v->z) )
		{

			if( (triPtr->mtl->bm.flags & MAT_WATER) != 0 )	// triangle is water!!!!
			{
				// point is within bounding box of triangle so now lets test with 2x2 determinants to see if the
				// XZ point is within the droppped XZ triangle.

				pcoord = triPtr->p.v->geoCoord;
				qcoord = triPtr->q.v->geoCoord;
				rcoord = triPtr->r.v->geoCoord;

				float Xtp = pcoord->x - v->x;
				float Ztp = pcoord->z - v->z;
				float Xup = qcoord->x - v->x;
				float Zup = qcoord->z - v->z;
				float Xvp = rcoord->x - v->x;
				float Zvp = rcoord->z - v->z;
			
				if( Xtp*Zup  >  Xup*Ztp )
				{
					tcondn = ( ( Xup*Zvp >= Xvp*Zup) &&  ( Xvp*Ztp >= Xtp*Zvp) );
				} 
				else
				{
					tcondn = ( ( Xup*Zvp <= Xvp*Zup) &&  ( Xvp*Ztp <= Xtp*Zvp) );
				}

				if(tcondn)
				{
					ycoord = (float)(triPtr->minusOneOverB * (
							    triPtr->A * v->x +
							    triPtr->C * v->z +
							    triPtr->planeD ) - v->y);
														
					if	( ycoord >= 0.0f )
					{
						*depth = ycoord;
						*resulttriPtr = triPtr;
						return( true );
					}

				}
			}
		}
	}

	return( false );
}

//================================================================================================================================
//================================================================================================================================

bool world::getShadowCoord( vector3 * v, vector3 * dirn, vector3 * shadowCoord, vector3 * triNormal )
{

	float yresult;
	triangle * resulttriPtr;
	getY( v,  &yresult, &resulttriPtr ,false); 
	float vlen = yresult - v->y;			// length of direction vector
	//vlen*=3;
	v->x = v->x + vlen*dirn->x;
	v->z = v->z + vlen*dirn->z;
	//return(getY( v,  &yresult, &resulttriPtr ,false)); 
	yresult += 2;

	bool foundit = false;
	//#define vlen 0.5f					// distance to consider for shadow		


	static vector3 r;
	triangle * triPtr;

	static vector3 pulledDirn;

	pulledDirn = *dirn;
	pulledDirn.y = -0.99f;//*= 100
	//pulledDirn.x *= 0.1f;
	pulledDirn.Normalise();
	pulledDirn = vector3(0,-1,0);
	float f = vlen * pulledDirn.x;
	float g = vlen * pulledDirn.y;
	float h = vlen * pulledDirn.z;
	
	float x0 = v->x;
	float y0 = v->y + 1;
	float z0 = v->z;

	{
		v->x += f;
		v->y += g;
		v->z += h;

		float yresult;
		static float lastval = 0.0;

		if(getY( v, &yresult, &triPtr, false ))
			lastval = yresult;
		else
			yresult = lastval;
	
		shadowCoord->x = v->x;
		shadowCoord->y = yresult + 0.02f;//
		shadowCoord->z = v->z;
	}
	return(true);
	
}


//================================================================================================================================
// getY
// ----
//		get height at point (or close to). 
//
//		- Used to retrieve the height of an XZ point above the world.
//
//		returns 'true' iff the point(XZ) is on the world.
//
//================================================================================================================================
//debugger DBGR;
double sum = 0;
double cnt = 0;

extern void dprintf(char *,...);

// if shadptr!=NULL then it's written with true if we're over a shadow poly, otherwise false.
bool world::getY( vector3 * v, float * yresult, triangle ** resulttriPtr, bool wallsallowed,
				 scanset *scans,int numscans)     // Assumes v->x, v->z inrange (above world)
{
	slong hunkX, hunkZ;
	hunk * hunkPtr;

	
	// Find the hunk which contains this XZ point..

	mesh * meshData = worldModel->meshData;	// using most detailed lod

	FASTFTOL(&hunkX , (v->x - meshData->MinCoordX) * oneoverxhunksize - 0.5f);
	FASTFTOL(&hunkZ , (v->z - meshData->MinCoordZ) * oneoverzhunksize - 0.5f);
		
	if( hunkX >= xhunks) hunkX = xhunks-1;
	if( hunkZ >= zhunks) hunkZ = zhunks-1;

	if( hunkX < 0) hunkX = 0;
	if( hunkZ < 0) hunkZ = 0;

	hunkPtr = hunkList + hunkX*zhunks + hunkZ;


	//
	// Now examine every triangle in the hunk to find the triangle which this point sits above.
	//		- examine both Non-Wall triangles, and wall triangles
	//

	yfound = false;
	if( scanTrianglesForXZ( hunkPtr->tlistPtr_N, hunkPtr->tcount_N, v, yresult, resulttriPtr,scans,numscans) )
	{
		return(true) ;
	}
	if(wallsallowed)
	{
		if(scanTrianglesForXZ( hunkPtr->tlistPtr_W, hunkPtr->tcount_W, v, yresult, resulttriPtr,scans,numscans) )
		{
			return true;
		}
		return false;
	}
	else
		return(false);

}



world::~world()
{
//	delete worldModel;
	for (int i=0; i<totalhunks; ++i)
	{
		delete[] hunkList[i].tlistPtr_N;
		delete[] hunkList[i].tlistPtr_W;
	}
	delete[] hunkList;
}

//
//  for all the world...
//	find the triangle which is intersected by vector from 'p' in direction 'd' for which
//  the point of intersection is the closest.
//  This involves firstly finding the hunks along the direction vector, and testing the
//  triangles in each hunk WITHOUT RE-EXAMINING TRIANGLES occuring in more than one hunk.
//  ( returns true iff intersection found )

bool world::findClosestIntersectedTriangle( vertex * p, vertex * d, triangle * t, float * distance)
{
		
	return(true);
}

//
//  for all the world...
//  find the triangle which is closest (at nearest point) to a coordinate 'p'
//  ( involves examining firstly the hunk containing 'p' and then the hunks
//    surrounding it.  Note that because hunks are rectangular the closest triangle
//    is not necessarily in the same hunk as the point!
//  )
//

void world::findClosestTriangle( vertex * p, float * distance )
{
}
 

void world::updateLforvertex( vertex * v)
{

	v->l =  sunLightDirection.DotProduct( &v->vertexNormal ) ;
	if (v->l < 0 ) v->l = 0;
}

//-------------------------------------------------------------------------------------------
// initialise the solar light source
//-------------------------------------------------------------------------------------------

void world::setSun( vector3 * lightDirection,
	 			  uchar  ambientR, uchar ambientG, uchar ambientB,
				  uchar  incidentR, uchar incidentG, uchar incidentB)
{

	sunAmbientColour.red  = ambientR;
	sunAmbientColour.grn  = ambientG;
	sunAmbientColour.blu  = ambientB;
	sunIncidentColour.red = incidentR;
	sunIncidentColour.grn = incidentG;
	sunIncidentColour.blu = incidentB;

	//
	// Calculate 'l' for each WORLD co-ordinate now, since the world foes NOT move in world space!
	//

	sunLightDirection = *lightDirection;
	sunLightDirection.Normalise();
	updateWorldForSun();
}


void world::updateWorldForSun()
{
	ulong t;
	if( sunOn && ( sunLightDirection != vector3(0,0,0) ) )
	{
		for(int i=0;i<worldModel->meshct;i++)
		{
			mesh * meshData = worldModel->meshData + i;

			if( lastSunLightDirection != sunLightDirection)
			{
				vertex * vlist = meshData->vlistPtr;

				for( t = meshData->vcount; t--; )
				{
					updateLforvertex( vlist );
					vlist++;
				}

				lastSunLightDirection = sunLightDirection;
			}
		}
	}
}

void world::setSunDirection( vector3 * d )
{
	sunLightDirection = *d;
	sunLightDirection.Normalise();
	updateWorldForSun();
}

void world::turnSunOn()
{
	if( !sunOn )
	{
		sunOn = true;
		updateWorldForSun();
	}
}

//================================================================================================================================
// END OF FILE
//================================================================================================================================
