/*
	Skidmark code. First version - single triangle pair.
*/

#include <windows.h>
#include <stdio.h>
#if !defined(UNDER_CE)
#include <process.h>
#include <time.h>
#include <new.h>
#include <crtdbg.h>
#endif
#include <stdlib.h>
#include <dinput.h>
#include <dsound.h>
	
#include "BS2all.h"
#include "particle.h"
#include "ALLcar.h"
#include "controller.h"
#include "carsound.h"
#include "param.h"
#include "PositionTable.h"
#include "Overlay.h"
#include "Camera.h"
#include "fx.h"
#include "frontend.h"

#define	NUMSKIDQUADS	1024
#define	YSPACING	0.06f	// height of skid above ground
#define	SKIDWIDTH	0.2f		// width of skid


extern gamesetup *game;
extern car *cararray[];

static material mat[8*3];	// 0 is tar, 1 is grass, 2 is dust


void RenderSkidTri(vector3 *points,int idx1,int idx2,int idx3,int matidx)
{
	vertex p,q,r;
	triangle t;


	t.mtl = mat+matidx;

	p.geoCoord = points+idx1;
	q.geoCoord = points+idx2;
	r.geoCoord = points+idx3;

	p.l = 1.0;
	p.specularl=0.0;
	q.l = 1.0;
	q.specularl=0.0;
	r.l = 1.0;
	r.specularl=0.0;

	memset(&t.p,0,sizeof(t.p));t.p.v = &p;
	memset(&t.q,0,sizeof(t.p));t.q.v = &q;
	memset(&t.r,0,sizeof(t.p));t.r.v = &r;

	boris2->renderTriangle(&t);
}


static void GetContactPatch(car *c,int w,vector3 *v1,vector3 *v2)
{
	// we get the coordinates of either side of the patch by putting (-x,0,0) and (x,0,0) through
	// the wheel's concatenated (i.e. 'to-world') matrix.

	vector3 vv1(-SKIDWIDTH/2,0,0),vv2(SKIDWIDTH/2,0,0);

	c->wheel[w]->wheelObject->concatenatedTransform.multiplyV3By3x4mat(v1,&vv1);
	c->wheel[w]->wheelObject->concatenatedTransform.multiplyV3By3x4mat(v2,&vv2);
}


	// keyed on [car][wheel]

static bool skidstate[16][4];

	// these two vectors define the two points which form each end of a contact patch
static vector3 skidstartpos1[16][4];
static vector3 skidstartpos2[16][4];

static vector3 skidquads[NUMSKIDQUADS*4];
static bool quadinuse[NUMSKIDQUADS];
static int skidmatidx[NUMSKIDQUADS];

static int curquad=0,skidsused=0;

void InitialiseSkids()
{
	static DWORD basiccol[]=
	{
		0,	// tar
		0x203000,	// grass
		0x303000,	// dust
	};

	for(int i=0;i<16;i++)
	{
		for(int j=0;j<4;j++)
		{
			skidstate[i][j]=false;
		}
	}

	memset(mat,0,sizeof(material)*8*3);

	for(i=0;i<8;i++)
	{
		for(int j=0;j<3;j++)
		{
		DWORD col;

			col = (i*16)<<24;
			col |= basiccol[j];

			mat[i*3+j].ambient=col;
			mat[i*3+j].diffuse=col;
			mat[i*3+j].specular=col;
			mat[i*3+j].bm.flags |= MAT_SHADOW|MAT_ALPHA;	// so that winding order doesn't matter.
		}
	}

	curquad=skidsused=0;
	for(i=0;i<NUMSKIDQUADS;i++)quadinuse[i]=false;
}


void RenderSkids()
{
	for(int i=0;i<skidsused;i++)
	{
		RenderSkidTri(skidquads+i*4,0,1,3,skidmatidx[i]);
		RenderSkidTri(skidquads+i*4,1,3,2,skidmatidx[i]);
	}
}

static void AddSkidQuad(vector3 *v1,vector3 *v2,vector3 *v3,vector3 *v4,int str)
{
	// OK, these four points - in order v1,v2,v4,v3 - form our quad.
	// First we need to correct their Y - i.e. plonk 'em

	triangle *tp;
	float y1=0,y2=0,y3=0,y4=0;

	boris2->currentworld->getY(v2,&y2,&tp,false);
	boris2->currentworld->getY(v3,&y3,&tp,false);
	boris2->currentworld->getY(v4,&y4,&tp,false);

	// we do v1 last, so that we can get the material at that wheel
	boris2->currentworld->getY(v1,&y1,&tp,false);

	int skidtype;
	switch(tp->mtl->bm.type)
	{
		case MATTYPE_KERB:
		case MATTYPE_ROUGHCONCRETE:
		case MATTYPE_MEDIUMCONCRETE:
		case MATTYPE_SMOOTHCONCRETE:
		case MATTYPE_PLANKS:
		case MATTYPE_TARMAC:
			skidtype = 0;break;
		case MATTYPE_GRAVELTRAP:
		case MATTYPE_SANDTRAP:
		case MATTYPE_DUST:
		case MATTYPE_DEEPSANDTRAP:
		case MATTYPE_COBBLES:
		case MATTYPE_GRAVEL:
		case MATTYPE_SAND:
		case MATTYPE_SANDBAGS:
			skidtype = 2;break;
		case MATTYPE_MUD:
		case MATTYPE_ROCK:
		case MATTYPE_SHORTGRASS:
		case MATTYPE_LONGGRASS:
		case MATTYPE_ICE:
		case MATTYPE_FRESHSNOW:
		case MATTYPE_MELTEDSNOW:
		case MATTYPE_DRIFTEDSNOW:
			skidtype = 1;break;
		default:skidtype=0;break;
	}

	v1->y = y1+YSPACING;
	v2->y = y2+YSPACING;
	v3->y = y3+YSPACING;
	v4->y = y4+YSPACING;

	// stick splitting code in here

	// ok, got that. Now grab a free quad from the circular buffer and fill it in

	int q = curquad*4;

	skidquads[q++] = *v1;
	skidquads[q++] = *v2;
	skidquads[q++] = *v4;
	skidquads[q] = *v3;

	skidmatidx[curquad++]=str*3+skidtype;
	if(curquad==NUMSKIDQUADS)curquad=0;
	skidsused++;
	if(skidsused>NUMSKIDQUADS)skidsused=NUMSKIDQUADS;
}



void CheckCarsForSkids()
{

	// we treat each wheel seperately

	for(int i=0;i<game->numcars;i++)
	{
		for(int j=0;j<4;j++)
		{
			bool newstate;

			newstate = cararray[i]->skidding[j];

			if(newstate && !skidstate[i][j])
			{
				// starting a new skid - record the position of the wheel

				GetContactPatch(cararray[i],j,&(skidstartpos1[i][j]),&(skidstartpos2[i][j]));
			}
			else if(!newstate && skidstate[i][j])
			{
				// ending a skid - get the position of the wheel and use this along with
				// the start pos (above) to create a new skid quad.

				vector3 v1,v2;
				GetContactPatch(cararray[i],j,&v1,&v2);
				AddSkidQuad(&(skidstartpos1[i][j]),&(skidstartpos2[i][j]),&v1,&v2,(rand()%6)+1);
			}
			else if(skidstate[i][j])
			{
				// still skidding.. if it's too long, break the skid

				vector3 v1,v2,v;
				GetContactPatch(cararray[i],j,&v1,&v2);

				v = v1 - skidstartpos1[i][j];
				if(v.MagSquared() > 9)
				{
					// end this skid
					AddSkidQuad(&(skidstartpos1[i][j]),&(skidstartpos2[i][j]),&v1,&v2,(rand()%6)+1);
					// start a new skid with this contact patch
					skidstartpos1[i][j] = v1;
					skidstartpos2[i][j] = v2;
				}
			}
			skidstate[i][j] = newstate;
		}
	}
}



