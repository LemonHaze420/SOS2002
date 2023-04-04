// Overlay.cpp: implementation of the Overlay class.
//
//////////////////////////////////////////////////////////////////////
//
// This manages the in-game overlay system
//

#define	NEARZ	0.0000002


#define	DEPTH_GENERIC_ROTATED	0.0000001f
#define	DEPTH_COCKPIT			0.002f
//define DEPTH_GENERIC_TRI		0.001	// defined in engine.cpp
#define	DEPTH_GENERIC			0.0015f
#define	DEPTH_COCKPIT_ENV		0.002f

#define	DEPTH_NEEDLE_SHADOWS	0.0001002f
//#define	DEPTH_NEEDLES			0.0000004	// was ...02
//#define	DEPTH_WHEEL				0.0000002

//Changed to accomodate the flag pole
#define	DEPTH_NEEDLES			0.0000008f
#define	DEPTH_WHEEL				0.0000004f
#define	DEPTH_FLAGPOLE			0.0000002f




#include <windows.h>
#include <stdio.h>
#if !defined(UNDER_CE)
#include <process.h>
#include <new.h>
#include <time.h>
#endif
#include <stdlib.h>
#include <stdarg.h>


#include "BS2all.h"
#include "ALLcar.h"
#include "Param.h"
#include "Overlay.h"
#include "Positiontable.h"
#include "frontend.h"
#include "camera.h"
#include <rgraphics.h>

extern struct gamesetup *game;
extern CameraBank *AllCameras;
extern car *cararray[];
extern int resX;

extern unsigned long Now();
extern unsigned long start_of_race;
extern bool do_exit_hack;
extern char resString[];

extern float camposzModifierInCar;

extern bool NoPlayers;

Overlay overlay;
extern car *cameracar,*playercar;
extern bool rearglance;

void dprintf(char *,...);
#define dprintf(x)

BorisMaterial papers[4]={0};
extern bool Paused;
extern bool carinfo_on;

void Overlay::Dprintf(int slot,char *s,...)
{
	char buf[256];
	va_list argptr;
	va_start(argptr,s);

	vsprintf(buf,s,argptr);

	font1->RenderString(30,20+slot*10,buf);
	va_end(argptr);
}

extern bool use_steering_wheel;

void Overlay::LoadDial(dash *d, int i,char *name,ParamFile &f)
{
	char buf[256],*s;

	if(s=f.Find(name))
	{
		strcpy(buf,s);
		s=strtok(buf,",");d->dials[i].needle=atoi(s);
		s=strtok(NULL,",");d->dials[i].x=atoi(s);
		s=strtok(NULL,",");d->dials[i].y=atoi(s);

		sprintf(buf,"%smin",name);
		if(s=f.Find(buf))
		{
			sscanf(s,"%d, %f",&d->dials[i].minvalue,&d->dials[i].minangle);
		}
		sprintf(buf,"%smax",name);
		if(s=f.Find(buf))
		{
			int tempint;
			// This next line is necessary. If it's not here maxvalue gets set to nonsense.
			// We're not changing the pointer so it shouldn`t break anything even on non-buggy systems.
			sscanf(s, "%d", &tempint);
			sscanf(s," %d, %f",&d->dials[i].maxvalue, &d->dials[i].maxangle);
			d->dials[i].maxvalue = tempint;
		}
	}
	else
		d->dials[i].needle = -1;
}


void Overlay::LoadCarOverlay(int idx,char *cararcname, bool playercar)
{
	int i;

	char buf[64];
	char name[64];
	sprintf(buf,".\\archives\\%s.tc",cararcname);


	if(!arcPush(buf))fatalError("unable to load push archive for car %s (overlays)",buf);

	// load the car's overlay parameters

	ParamFile f;
	char *s;

	dash *d = dashboards+idx;

	d->numcockpitparts=0;
	d->hasDial = false;

	sprintf(name,"%soverlay",resString);
	if(f.Read("overlay"))	// to be fixed - used old overlay
	{
		char buf[256];

		// get cockpit offset value

		if(playercar)
		{
			if(s=f.Find("cockpitoffset"))
			{
				strcpy(buf,s);
				s=strtok(buf,",");
				d->cockpitoffsetx=atoi(s);
				s=strtok(NULL,",");
				d->cockpitoffsety=atoi(s);
			}
			else fatalError("no cockpit offset in overlay params for %s",cararcname);

			if(s=f.Find("cockpitrotate"))
			{
				strcpy(buf,s);
				s=strtok(buf,",");
				d->rotatex=atoi(s);
				s=strtok(NULL,",");
				d->rotatey=atoi(s);

				d->rotatex -= d->cockpitoffsetx;
				d->rotatey += d->cockpitoffsety;
			}
			else { d->rotatex = d->cockpitoffsetx; 
				d->rotatey = d->cockpitoffsety; }

			// Get the modifier for the camera position
			// This MUST be made to work on a per-car basis, not globally.
			if (s=f.Find("camposzMod"))
			{
				strcpy(buf,s);
				s=strtok(buf,",");
				sscanf(s, "%f", &d->camposzmod);
			}

			// get number of cockpit parts

			if(s=f.Find("cockpitparts"))
				d->numcockpitparts = atoi(s);
			else fatalError("no cockpitparts in overlay params for %s",cararcname);

			if(s=f.Find("cockenvparts"))
				d->numcockpitpartsenv = atoi(s);
			else
				d->numcockpitpartsenv = 0;


			// now go hunting for the cockpit parts

			for(i=0;i<d->numcockpitparts;i++)
			{
				sprintf(name,"%scockpit%d",resString,i);
				sprintf(buf,"cockpit%d",i);
				// load the texture for this part

				d->cocktex[i]=getTexturePtr(buf,false,true)->textureHandle;
				dprintf("overlay %s loaded for %s into %lx",buf,cararcname,d->cocktex[i]);

				if(s=f.Find(buf))
				{
					strcpy(buf,s);
					s=strtok(buf,",");d->cockpitrects[i].x=atoi(s);
					s=strtok(NULL,",");d->cockpitrects[i].y=atoi(s);
					s=strtok(NULL,",");d->cockpitrects[i].w=atoi(s);
					s=strtok(NULL,",");d->cockpitrects[i].h=atoi(s);
				}
				else fatalError("no cockpit rect %d in overlay params for %s",i,cararcname);
			}

			for(i=0;i<d->numcockpitpartsenv;i++)
			{
				sprintf(buf,"cockenv%d",i);
				// load the texture for this part

				d->cockenvtex[i] = getTexturePtr(buf,false,true)->textureHandle;

				if(s=f.Find(buf))
				{
					strcpy(buf,s);
					s=strtok(buf,",");d->cockpitrectsenv[i].x=atoi(s);
					s=strtok(NULL,",");d->cockpitrectsenv[i].y=atoi(s);
					s=strtok(NULL,",");d->cockpitrectsenv[i].w=atoi(s);
					s=strtok(NULL,",");d->cockpitrectsenv[i].h=atoi(s);
				}
				else
				{
					d->numcockpitpartsenv=0;break;
				}
			}

			// cockpit stuff loaded... load needle data next

			if(s=f.Find("needles"))
				d->numneedles=atoi(s);
			else fatalError("unable to load needle count for %s",cararcname);

			float needleswidth,needlesheight;

			if(s=f.Find("needleswidth"))
				needleswidth = (float)atoi(s);
			if(s=f.Find("needlesheight"))
				needlesheight = (float)atoi(s);

			d->needletex = getTexturePtr("needles",false,true)->textureHandle;

			for(i=0;i<d->numneedles;i++)
			{
				sprintf(buf,"needle%dimg",i);
				if(s=f.Find(buf))
				{
					strcpy(buf,s);
					s=strtok(buf,",");d->needles[i].x=atoi(s);
					s=strtok(NULL,",");d->needles[i].y=atoi(s);
					s=strtok(NULL,",");d->needles[i].w=atoi(s);
					s=strtok(NULL,",");d->needles[i].h=atoi(s);
				}
				sprintf(buf,"needle%dpivot",i);
				if(s=f.Find(buf))
				{
					strcpy(buf,s);
					s=strtok(buf,",");d->needles[i].px=atoi(s);
					s=strtok(NULL,",");d->needles[i].py=atoi(s);
				}

				// here we work out the UV coordinates
				d->needles[i].u = ((float)(d->needles[i].x))/needleswidth;
				d->needles[i].uw = ((float)(d->needles[i].w))/needleswidth;
				d->needles[i].v = ((float)(d->needles[i].y))/needlesheight;
				d->needles[i].vh = ((float)(d->needles[i].h))/needlesheight;

				//// do exactly the same thing for the shadow coordinates,
				//// but mark the shadows with x = -1 if the needle doesn't have
				//// a shadow

				sprintf(buf,"needle%dshimg",i);
				if(s=f.Find(buf))
				{
					strcpy(buf,s);
					s=strtok(buf,",");d->shadows[i].x=atoi(s);
					s=strtok(NULL,",");d->shadows[i].y=atoi(s);
					s=strtok(NULL,",");d->shadows[i].w=atoi(s);
					s=strtok(NULL,",");d->shadows[i].h=atoi(s);
				}
				else
					d->shadows[i].x = -1;

				sprintf(buf,"needle%dshpivot",i);
				if(s=f.Find(buf))
				{
					strcpy(buf,s);
					s=strtok(buf,",");d->shadows[i].px=atoi(s);
					s=strtok(NULL,",");d->shadows[i].py=atoi(s);
				}

				// here we work out the UV coordinates
				d->shadows[i].u = ((float)(d->shadows[i].x))/needleswidth;
				d->shadows[i].uw = ((float)(d->shadows[i].w))/needleswidth;
				d->shadows[i].v = ((float)(d->shadows[i].y))/needlesheight;
				d->shadows[i].vh = ((float)(d->shadows[i].h))/needlesheight;
			}

		}

		// needles loaded, now for the dial data

		LoadDial(d,RPMDIAL,"rpm",f);
		LoadDial(d,KPHDIAL,"kph",f);
		LoadDial(d,TEMPDIAL,"temp",f);
		LoadDial(d,OILDIAL,"oil",f);
		LoadDial(d,FUELDIAL,"fuel",f);
		LoadDial(d,HOURSDIAL,"hours",f);
		LoadDial(d,MINUTESDIAL,"minutes",f);
		LoadDial(d,SECONDSDIAL,"seconds",f);

		// now load the alpha dial if used
		if(s = f.Find("dialorigin"))
		{
			d->hasDial = true;

			strcpy(buf,s);

			s=strtok(buf,","); d->Dialx = atoi(s);
			s=strtok(NULL,","); d->Dialy = atoi(s);
			s=strtok(NULL,","); d->Dialw = atoi(s);
			s=strtok(NULL,","); d->Dialh = atoi(s);

			d->dialtex = getTexturePtr("dial",false,true)->textureHandle;
			d->dialneedletex = getTexturePtr("dialneedle",false,true)->textureHandle;

			if(s = f.Find("dialscreenpos"))
			{
				strcpy(buf,s);

				s=strtok(buf,","); d->DialScX = atoi(s);
				s=strtok(NULL,","); d->DialScY = atoi(s);
			}
		}

		if(playercar)
		{
			// now load the mirror data if used
			if(s = f.Find("mirrortype"))
			{
				strcpy(buf,s);

				float modifier;

				modifier = (resX==512)?512.0f/800.0f:1.0f;

				s=strtok(buf,","); d->mirrortype = atoi(s);
				s=strtok(NULL,","); d->mirrorcam.x = atoi(s) * modifier;
				s=strtok(NULL,","); d->mirrorcam.y = atoi(s) * modifier;
				s=strtok(NULL,","); d->mirrorcam.w = atoi(s) * modifier;
				s=strtok(NULL,","); d->mirrorcam.h = atoi(s) * modifier;

				s = f.Find("mirrorpos");

				strcpy(buf,s);

				s=strtok(buf,","); d->mirrorpos.x1 = atoi(s) * modifier;
				s=strtok(NULL,","); d->mirrorpos.x2 = atoi(s) * modifier;
				s=strtok(NULL,","); d->mirrorpos.y1 = atoi(s) * modifier;
				s=strtok(NULL,","); d->mirrorpos.y2 = atoi(s) * modifier;

				if(d->mirrortype == 2)
				{
					s=strtok(NULL,","); d->mirrorpos.x3 = atoi(s) * modifier;
					s=strtok(NULL,","); d->mirrorpos.x4 = atoi(s) * modifier;
					s=strtok(NULL,","); d->mirrorpos.y3 = atoi(s) * modifier;
					s=strtok(NULL,","); d->mirrorpos.y4 = atoi(s) * modifier;
				}
			}
			else
				d->mirrortype = 0;

			// now load the mirror data 2 if used
			if(s = f.Find("mirrortype2"))
			{
				strcpy(buf,s);

				float modifier;

				modifier = (resX==512)?512.0f/800.0f:1.0f;

				s=strtok(buf,","); d->mirrortype2 = atoi(s);
				if(d->mirrortype2 == 2)
					d->mirrortype2 = 4;
				s=strtok(NULL,","); d->mirrorcam2.x = atoi(s) * modifier;
				s=strtok(NULL,","); d->mirrorcam2.y = atoi(s) * modifier;
				s=strtok(NULL,","); d->mirrorcam2.w = atoi(s) * modifier;
				s=strtok(NULL,","); d->mirrorcam2.h = atoi(s) * modifier;

				s = f.Find("mirrorpos2");

				strcpy(buf,s);

				s=strtok(buf,","); d->mirrorpos2.x1 = atoi(s) * modifier;
				s=strtok(NULL,","); d->mirrorpos2.x2 = atoi(s) * modifier;
				s=strtok(NULL,","); d->mirrorpos2.y1 = atoi(s) * modifier;
				s=strtok(NULL,","); d->mirrorpos2.y2 = atoi(s) * modifier;

				if(d->mirrortype2 == 4)
				{
					s=strtok(NULL,","); d->mirrorpos2.x3 = atoi(s) * modifier;
					s=strtok(NULL,","); d->mirrorpos2.x4 = atoi(s) * modifier;
					s=strtok(NULL,","); d->mirrorpos2.y3 = atoi(s) * modifier;
					s=strtok(NULL,","); d->mirrorpos2.y4 = atoi(s) * modifier;
				}
			}
			else
				d->mirrortype2 = 0;
			// now load steering wheel data (if any)

			if(use_steering_wheel && (s=f.Find("wheelparts")))
			{
				d->numwheelparts = atoi(s);

				for(i=0;i<d->numwheelparts;i++)
				{
					sprintf(buf,"wheel%d",i);
					d->wheeltex[i] = getTexturePtr(buf,false,true)->textureHandle;
					dprintf("wheel overlay loaded %s for %s into %lx",buf,cararcname,d->wheeltex[i]);

					if(s=f.Find(buf))
					{
						strcpy(buf,s);
						s=strtok(buf,",");d->wheelrects[i].x=atoi(s);
						s=strtok(NULL,",");d->wheelrects[i].y=atoi(s);
						s=strtok(NULL,",");d->wheelrects[i].w=atoi(s);
						s=strtok(NULL,",");d->wheelrects[i].h=atoi(s);
					}
					else fatalError("no wheel rect %d in overlay params for %s",i,cararcname);
				}

				if(s=f.Find("wheeloffset"))
				{
					strcpy(buf,s);
					s=strtok(buf,",");
					d->wheeloffsetx=atoi(s);
					s=strtok(NULL,",");
					d->wheeloffsety=atoi(s);
				}
				else fatalError("no wheel offset in overlay params for %s",cararcname);

				if(s=f.Find("wheelcentre"))
				{
					strcpy(buf,s);
					s=strtok(buf,",");
					d->wheelcentrex=atoi(s);
					s=strtok(NULL,",");
					d->wheelcentrey=atoi(s);
				}
				else fatalError("no wheel centre in overlay params for %s",cararcname);

				if(s=f.Find("wheelscale"))
				{
					sscanf(s,"%f",&d->wheelscale);
				}
				else d->wheelscale = 1.4f;
			}
			else d->numwheelparts = 0;
		}
	}


	arcPop();
}

void Overlay::Load()
{

	// car-specific stuff, now load generic stuff	

	flagpos=0;

	for(int i=0;i<game->numcars;i++)
	{
		LoadCarOverlay(i,game->cars[i].carname,cararray[i]==playercar);
	}

	dprintf("1 - OK");

	if(!arcPush(".\\archives\\overlays.tc"))
		fatalError("unable to find overlays archive");
	dprintf("2 - OK");

	// this should also load the "generic" overlay texture.

	if(!font1)
	{
		font1 = new font("font1");
		dprintf("f1 - OK");
		font2 = new font("font2");
		dprintf("f2 - OK");
		font3 = new font("font3");
		dprintf("f3 - OK");
		font4 = new font("font4");
		dprintf("f4 - OK");
		font5 = new font("font5");
		dprintf("f5 - OK");
		cfont = new font("cfont");
	}
	generic[0][0] = getTexturePtr("overlay00",false,false)->textureHandle;
	generic[0][1] = getTexturePtr("overlay01",false,false)->textureHandle;

	char *genfilenames[] = {"overlay10","overlay10we","overlay10it","overlay10ge","overlay10fr"};

	generic[1][0] = getTexturePtr(genfilenames[language],false,false)->textureHandle;
	generic[1][1] = getTexturePtr("overlay11",false,false)->textureHandle;

	dprintf("tex - OK");

	char *pausedfilenames[] = {"paused","pausedwe","pausedit","pausedge","pausedfr"};

	pausedtexture = getTexturePtr(pausedfilenames[language],false,true)->textureHandle;

	arcPop();

	LoadFlags();
	LoadCarInfo();

	SwitchToDash(0);
}


void Overlay::SwitchToDash(int i)
{
	curdash = dashboards+i;
	camposzModifierInCar = curdash->camposzmod;
	
}

void Overlay::RenderDial(int i,float val,int x,int y,float angleoffset,int rotatex,int rotatey,
						 float judder)
{

	// x and y are the centre of the dial. We need to work out the origin of the image and
	// render the dial there. But first, we need to work out the angle.

	int n = curdash->dials[i].needle;
	if(n<0)return;


	float tx=x,ty=y;

	boris2->rotatePointAroundPoint(&tx,&ty,rotatex,rotatey,angleoffset);
	x = tx; y = ty;

	val -= curdash->dials[i].minvalue;
	val /= (curdash->dials[i].maxvalue - curdash->dials[i].minvalue);

	// val now 0-1

	if(val<0)val=0;
	if(val>1)val=1;

	val *= (curdash->dials[i].maxangle - curdash->dials[i].minangle);
	val += curdash->dials[i].minangle;

	val += angleoffset;

	// add judder

	if(!Paused && !cameracar->engdead)
	{
		float rval=(float)(rand());
		rval /= (float)RAND_MAX;
		val += judder*rval;
	}



	boris2->addTo2DlistRotated(
		x+curdash->needles[n].x-curdash->needles[n].px,
		y+curdash->needles[n].y-curdash->needles[n].py,
		curdash->needles[n].w,curdash->needles[n].h,
		curdash->needles[n].u,curdash->needles[n].v,curdash->needles[n].uw,curdash->needles[n].vh,
		curdash->needletex,
		MAT_TCLAMP+MAT_SCLAMP,val,
		x,y,
		DEPTH_NEEDLES);

	if(curdash->shadows[n].x>=0)
	{
		boris2->addTo2DlistRotated(
		x+curdash->shadows[n].x-curdash->shadows[n].px,
		y+curdash->shadows[n].y-curdash->shadows[n].py,
		curdash->shadows[n].w,curdash->shadows[n].h,
		curdash->shadows[n].u,curdash->shadows[n].v,curdash->shadows[n].uw,curdash->shadows[n].vh,
		curdash->needletex,
		MAT_TCLAMP+MAT_SCLAMP,val,
		x+4,y+4,
		DEPTH_NEEDLE_SHADOWS);
	}

}

void Overlay::RenderDialOut(int i,float val,int x,int y,float angleoffset,int rotatex,int rotatey,
						 float judder)
{

	// x and y are the centre of the dial. We need to work out the origin of the image and
	// render the dial there. But first, we need to work out the angle.

	float tx=x,ty=y;

	boris2->rotatePointAroundPoint(&tx,&ty,rotatex,rotatey,angleoffset);
	x = tx; y = ty;

	val -= curdash->dials[i].minvalue;
	val /= (curdash->dials[i].maxvalue - curdash->dials[i].minvalue);

	// val now 0-1

	if(val<0)val=0;
	if(val>1)val=1;

	val *= (curdash->dials[i].maxangle - curdash->dials[i].minangle);
	val += curdash->dials[i].minangle;

	val += angleoffset;

	// add judder

	if(!Paused && !cameracar->engdead)
	{
		float rval=(float)(rand());
		rval /= (float)RAND_MAX;

		val += judder*rval;
	}

	boris2->addTo2DlistRotated(
		x-6,
		y-53,
		10,62,
		0,1.0/64.0f,10/32.0f,62/64.0f,
		curdash->dialneedletex,
		MAT_TCLAMP+MAT_SCLAMP,val,
		x,y,
		DEPTH_NEEDLES);
}

void Overlay::DrawGeneric(int x,int y,int sx,int sy,int w,int h)
{
	float u,v,uw,vh;
	int bx,by;

	bx = sx >> 8;
	by = sy >> 8;
	sx &= 0xff;
	sy &= 0xff;


	u = (float)sx;
	v = (float)sy;

	uw = (float)w;
	vh = (float)h;

	u /= 256.0f;	// width of generic bmp
	v /= 256.0f;	// height of generic bmp
	uw /= 256.0f;
	vh /= 256.0f;

	boris2->addTo2Dlist(
		x,y,w,h,u,v,uw,vh,generic[bx][by],DEPTH_GENERIC);
}


void Overlay::DrawRotatedGeneric(int x,int y,int w,int h,int srcx,int srcy,int srcw,int srch,int rx,int ry,
								 float angle)
{
	float u = srcx;	
	float v = srcy;
	float uw = srcw;
	float vh = srch;

	u /= 256.0f;	v /= 256.0f;
	uw /= 256.0f;vh /= 256.0f;

	boris2->addTo2DlistRotated(x,y,w,h,u,v,uw,vh,generic[1][0],0,angle,rx,ry,DEPTH_GENERIC_ROTATED);
}


// x1.. etc are offsets into the rectangle starting at sx,sy 
void Overlay::DrawGenericTri(int dx,int dy,int sx,int sy,int x1,int y1,int x2,int y2,int x3,int y3)
{
	float u1,v1,u2,v2,u3,v3;
	int bx,by;

	bx = sx >> 8;
	by = sy >> 8;
	sx &= 0xff;
	sy &= 0xff;


	u1 = ((float)(x1+sx))/256.0f;
	v1 = ((float)(y1+sy))/256.0f;
	u2 = ((float)(x2+sx))/256.0f;
	v2 = ((float)(y2+sy))/256.0f;
	u3 = ((float)(x3+sx))/256.0f;
	v3 = ((float)(y3+sy))/256.0f;

	boris2->addTriTo2Dlist(
		x1+dx,y1+dy,u1,v1,
		x2+dx,y2+dy,u2,v2,
		x3+dx,y3+dy,u3,v3,
		generic[bx][by]);
}

char *TimeString(unsigned long t)
{
		static char buf[256];

		unsigned long m,s,h;

		h = t % 100;
		s = (t/100)%60;
		m = t/(100*60);

		sprintf(buf,"%02d:%02d:%02d",m,s,h);
		return buf;
}

void Overlay::Render()
{
	char buf[32];
//	float ang = cameracar->zangle * 0.08;
	float ang = 0;
	float revs = cameracar->revs;

	if(cameracar->engdead)revs=0;

extern float jjsHack1;	// this is the rotation imposed on the environment mapping. A crude hack :)


	bool showpause=false;
	if(game->flags & GSF_REPLAY)
	{
		if(Paused && (raceglobals.state == RACE_DURING || raceglobals.state == RACE_BEFORE)) showpause=true;
	}
	else
	{
	if(Paused || raceglobals.state ==RACE_FINISHED) 
			showpause=true;
	}

	if(showpause)
	{
#if 0
		DarkenScreen(0,0,800,600,0x80000000,0.0000001f);
#endif
		boris2->addTo2Dlist(400-128,250-128,256,256,0,0,1,1,pausedtexture,DEPTH_GENERIC_ROTATED);
		alphabase=0xa0;
	}

	DrawFlags();
	StartFlag();
	if(carinfo_on && !cameracar->CurrentPitstop)DrawCarInfo();

	cx += curdash->cockpitoffsetx;		// cx,cy is a small judder
	cy += curdash->cockpitoffsety;
	
	static PreviousAid=5;
	static int ThisAid;

	if (UseDriverAids)
	{
		ThisAid=racinglineset.getLine(0)->driveinfo[positiontable.rlp1[cameracar->id]].gears-1;
		static char buf[15];
		if (ThisAid < 1)
			ThisAid = PreviousAid;
		sprintf(buf, "%d\0", ThisAid);
		font5->printf(368,100,buf);
		PreviousAid = ThisAid;
	}

	if(cameraview == CAM_INCAR || cameraview == CAM_BUMPER)
	{
		// we need to determine the angle around z, so we can set the cameraangle inside boris.
		// This is used to rotate the sprites.

		vector3 yaxis(0,1,0),rotyaxis;
		float cosang,angz;

		// rotate Y axis into car space
		cameracar->mainObject->transform.multiplyV3By3x3mat(&rotyaxis,&yaxis);
		jjsHack1 = cameracar->yRotationAngle;

		// rotate this around the world Y axis by -heading

		matrix3x4 m;
		m.Transpose3x4(&cameracar->yRotationMatrix);
		m.multiplyV3By3x3mat(&yaxis,&rotyaxis);


		cosang = yaxis.y;
		angz = acos(cosang);
		if(yaxis.x<0)angz*=-1;

		boris2->cameraangle = angz;
	}


	if(cameraview == CAM_INCAR && !AllCameras->UsingTrackside && !rearglance)
	{
		if(cameracar->shadow_on)boris2->shadow_2d=0x00808080;

		// make sure car body is invisible

		cameracar->bodyObject->visible = false;
		cameracar->driverObject->visible = false;

		int i;

		for(i=0;i<curdash->numcockpitparts;i++)
		{
			boris2->addTo2DlistRotated(
				cx+curdash->cockpitrects[i].x,
				cy+curdash->cockpitrects[i].y,
				curdash->cockpitrects[i].w+1,
				curdash->cockpitrects[i].h+1,
				0,0,1,1,
				curdash->cocktex[i],
				MAT_TCLAMP+MAT_SCLAMP,

				ang,curdash->rotatex,curdash->rotatey,		// angle, (x,y) of rotation point rel. to image origin

				DEPTH_COCKPIT);
		}

		jjsHack1 = cameracar->yRotationAngle;	// sets a rotation angle for the environment mapping

		for(i=0;i<curdash->numcockpitpartsenv;i++)
		{
			boris2->addTo2DlistRotatedEnvMapped(
				cx+curdash->cockpitrectsenv[i].x,
				cy+curdash->cockpitrectsenv[i].y,
				curdash->cockpitrectsenv[i].w+1,
				curdash->cockpitrectsenv[i].h+1,
				0,0,1,1,
				curdash->cockenvtex[i],
				MAT_TCLAMP+MAT_SCLAMP,

				ang,curdash->rotatex,curdash->rotatey,		// angle, (x,y) of rotation point rel. to image origin

				DEPTH_COCKPIT_ENV);
		}

		// now render the needles for the dials.

		if(!cameracar->CurrentPitstop)
		{
			RenderDial(KPHDIAL,cameracar->mph * 1.60344f,
				cx+curdash->dials[KPHDIAL].x,cy+curdash->dials[KPHDIAL].y,ang,curdash->rotatex,curdash->rotatey,	0.05f);

			RenderDial(RPMDIAL,revs,
				cx+curdash->dials[RPMDIAL].x,cy+curdash->dials[RPMDIAL].y,ang,curdash->rotatex,curdash->rotatey,	0.1f);

			// these three dials are %ages
			RenderDial(TEMPDIAL,cameracar->temperature,
				cx+curdash->dials[TEMPDIAL].x,cy+curdash->dials[TEMPDIAL].y,ang,curdash->rotatex,curdash->rotatey);

			RenderDial(OILDIAL,cameracar->oil,
				cx+curdash->dials[OILDIAL].x,cy+curdash->dials[OILDIAL].y,ang,curdash->rotatex,curdash->rotatey);

			RenderDial(FUELDIAL,cameracar->fuel,
				cx+curdash->dials[FUELDIAL].x,cy+curdash->dials[FUELDIAL].y,ang,curdash->rotatex,curdash->rotatey);
#if !defined(UNDER_CE)
			if (curdash->dials[HOURSDIAL].needle >= 0)
			{
				long timelong;	
				struct tm *tm;
				time(&timelong);
				tm = localtime(&timelong);
				if (tm->tm_hour > 11) tm->tm_hour -= 12;
				RenderDial(HOURSDIAL, tm->tm_hour*60+tm->tm_min, 
					cx+curdash->dials[HOURSDIAL].x, cy+curdash->dials[HOURSDIAL].y,ang,curdash->rotatex,curdash->rotatey,0); 
				RenderDial(MINUTESDIAL, tm->tm_min,
					cx+curdash->dials[MINUTESDIAL].x, cy+curdash->dials[MINUTESDIAL].y,ang,curdash->rotatex,curdash->rotatey,0);
				RenderDial(SECONDSDIAL, tm->tm_sec,
					cx+curdash->dials[SECONDSDIAL].x, cy+curdash->dials[SECONDSDIAL].y,ang,curdash->rotatex,curdash->rotatey,0);
			}
#endif
		}

		// now render the steering wheel

		vector3 wv;

		cameracar->wheel[0]->wheelObject->getAngle(&wv);


		cx -= curdash->cockpitoffsetx;	// so we just get the judder again.
		cy -= curdash->cockpitoffsety;


		if(use_steering_wheel)
		{
			for(i=0;i<curdash->numwheelparts;i++)
			{
				int tmpx;

				tmpx = 0;

	//			if(i==1)tmpx=10;

				boris2->addTo2DlistRotated(
				tmpx+cx+curdash->wheelrects[i].x+curdash->wheeloffsetx - curdash->wheelcentrex*curdash->wheelscale,
				cy+curdash->wheelrects[i].y+curdash->wheeloffsety - curdash->wheelcentrey*curdash->wheelscale,
				curdash->wheelrects[i].w * curdash->wheelscale,
				curdash->wheelrects[i].h *curdash->wheelscale,
				0,0,1,1,
				curdash->wheeltex[i],
				MAT_TCLAMP+MAT_SCLAMP,

				wv.y*4,
				cx+curdash->wheeloffsetx,
				cy+curdash->wheeloffsety,
				DEPTH_WHEEL);
			}
		}
		boris2->shadow_2d=0xffffff;

	}
	else
	{
		if (NoPlayers)
			return;

		boris2->cameraangle=0;

		float kph;
		if(curdash->hasDial)
		{
#define REVSX	631

			switch(language)
			{
			case L_ENGLISH:
			case L_WELSH:
			font1->printf(REVSX,536,"%%r%d",(int)
				((cameracar->mph>0) ? cameracar->mph : 0));
			font2->printf(REVSX+9,544,"D");	// MPH
				break;
			default:
				
				kph = (cameracar->mph>0) ? cameracar->mph : 0;
				kph *= 1.609344f;
				font1->printf(REVSX,536,"%%r%d",(int)kph);
				font2->printf(REVSX+9,544,"E");	// KPH
				break;
			}

			boris2->addTo2Dlist(curdash->DialScX,curdash->DialScY,curdash->Dialw,curdash->Dialh,1/256.0f,1/256.0f,curdash->Dialw/256.0f,curdash->Dialh/256.0f,curdash->dialtex,DEPTH_GENERIC);
			RenderDialOut(RPMDIAL,revs,
				curdash->DialScX+curdash->Dialx,curdash->DialScY+curdash->Dialy,ang,curdash->rotatex,curdash->rotatey,	0.1f);

		}
#if !defined(UNDER_CE)
		else
		{
			// here we render the out-of-car view dials etc.
			// draw the revmeter	
			
			DrawGeneric(552,410,	266,277,	246,182);
			
			// ah, now for the Nasty Bit.. drawing the segment. 
			//		float theta = revs * ((71.33489*(PI/180))/8000.0); // angle
			
			float theta = revs * ((70.0f*(PI/180))/8000.0f); // angle
			
			float W = 227,H = 165;	// size of rectangle in source bmp
			float w1 = 165;			// centre of dial's X 
			int sx=3, sy=291;		// source x,y
			int dx=568,dy=425;		// dest x,y
			
			if(theta < PI / 2)	// first case, < 90 degrees
			{
				
				float h=tan(theta)*W;	// height of first triangle
				
				if(h<H)
				{
					// one triangle only!
					
					DrawGenericTri(dx,dy,sx,sy,	
						w1,H,		0,H,		0,H-h);
					
				}
				else
				{
					// two triangles
					
					
					DrawGenericTri(dx,dy,sx,sy,
						w1,H,		0,H,		0,0);
					
					float w2 = w1-H*tan((PI/2.0f)-theta)+61;
					
					if(w2<0)w2=0;
					
					DrawGenericTri(dx,dy,sx,sy,
						0,0,		w2,0,		w1,H);
					
				}
			}
			else
			{
				// draw three triangles
				
				DrawGenericTri(dx,dy,sx,sy,
					w1,H,0,H,0,0);
				DrawGenericTri(dx,dy,sx,sy,
					0,0,w1,0,w1,H);
				
				theta -= PI/2;
				if(theta> 26.0f*(PI/180.0f))theta=26.0f*(PI/180);
				
				float w2 = H*tan(theta);
				
				if(w1+w2 > W) w2= W-w1;
				
				DrawGenericTri(dx,dy,sx,sy,
					w1,0,		w1+w2,0,	w1,H);
				
			}
			
			// render the speed

			switch(language)
			{
			case L_ENGLISH:
			case L_WELSH:
				font1->printf(733,526,"%%r%d",(int)
				((cameracar->mph>0) ? cameracar->mph : 0));
			font2->printf(742,534,"D");	// MPH
				break;
			default:
				
				kph = (cameracar->mph>0) ? cameracar->mph : 0;
				kph *= 1.609344f;
				font1->printf(733,526,"%%r%d",(int)kph);
				font2->printf(742,534,"E");	// KPH
				break;
			}
		}
#endif
	}
	// and here we render the constant overlays

	// lap counter (this is going to require localisation)

	int qx,qy;	// temps

	if(positiontable.laps[cameracar->id]>=0 && raceglobals.state != RACE_AFTER)
	{
		switch(language)
		{
		case L_ENGLISH:
		default:
		font2->RenderString(36,17,"A");	// LAP
		font3->printf(130,7,"%%r%d",positiontable.laps[cameracar->id]+1);		// bignum (lap number)
		font3->RenderString(128,7,"a");	// slash
		font1->printf(138,16,"%d",game->laps);	// small number (number of laps)
		font2->RenderString(36,45,"A");	// LAP
		font1->printf(82,41,TimeString(Now() - positiontable.lapbegun[cameracar->id]));
			break;
		case L_WELSH:
			cfont->RenderString(20,15,"lap");
			font3->printf(130,7,"%%r%d",positiontable.laps[cameracar->id]+1);		// bignum (lap number)
			font3->RenderString(128,7,"a");	// slash
			font1->printf(138,16,"%d",game->laps);	// small number (number of laps)
			cfont->RenderString(20,43,"lap");	// LAP
			font1->printf(82,41,TimeString(Now() - positiontable.lapbegun[cameracar->id]));
			break;
		case L_GERMAN:
			cfont->RenderString(20,15,"runde");
			font3->printf(140,7,"%%r%d",positiontable.laps[cameracar->id]+1);		// bignum (lap number)
			font3->RenderString(138,7,"a");	// slash
			font1->printf(148,16,"%d",game->laps);	// small number (number of laps)
			cfont->RenderString(20,43,"runde");	// LAP
			font1->printf(102,41,TimeString(Now() - positiontable.lapbegun[cameracar->id]));
			break;
		case L_ITALIAN:
			cfont->RenderString(20,15,"giro");
			font3->printf(130,7,"%%r%d",positiontable.laps[cameracar->id]+1);		// bignum (lap number)
			font3->RenderString(128,7,"a");	// slash
			font1->printf(138,16,"%d",game->laps);	// small number (number of laps)
			cfont->RenderString(20,43,"giro");	// LAP
			font1->printf(82,41,TimeString(Now() - positiontable.lapbegun[cameracar->id]));
			break;
		case L_FRENCH:
			cfont->RenderString(20,15,"tour");
			font3->printf(130,7,"%%r%d",positiontable.laps[cameracar->id]+1);		// bignum (lap number)
			font3->RenderString(128,7,"a");	// slash
			font1->printf(138,16,"%d",game->laps);	// small number (number of laps)
			cfont->RenderString(20,43,"tour");	// LAP
			font1->printf(92,41,TimeString(Now() - positiontable.lapbegun[cameracar->id]));
			break;
		}
	}


	if(game->numcars!=1)
	{
		font4->printf(405,26,"%%r%d/",positiontable.pos[cameracar->id]+1);	// position
		font1->printf(400,42,"%d",game->numcars);		// out of N
	}


	extern bool race_begun;

	if(!(game->flags & GSF_REPLAY) || race_begun)
	{
		char *racetext[]={"eng","ras","gara","rennen","course"};

		switch(language)
		{
		case L_ENGLISH:
	font2->RenderString(636,46,"C",JUST_RIGHT);	// RACE
			break;
		default:
			cfont->RenderString(636,44,racetext[language],JUST_RIGHT);	// RACE
			break;
		}

	font1->printf(648,41,TimeString(Now() - start_of_race));	// race time
	}

	if(positiontable.lapbest[cameracar->id])	
	{
		char *besttext[]={"eng","gorau","migliore","bestzeit","meilleur"};

		switch(language)
		{
		case L_ENGLISH:
		font2->RenderString(636,18,"B",JUST_RIGHT);	// BEST
			break;
		default:
			cfont->RenderString(636,16,besttext[language],JUST_RIGHT);
			break;
		}
		font1->printf(648,14,TimeString(positiontable.lapbest[cameracar->id]));	// best lap time
	}

	// Print the split times.
	if (positiontable.DisplaySplitTime[cameracar->id])
	{
		font1->printf(648,68,TimeString(positiontable.DisplaySplit[cameracar->id]));
	}
	if (positiontable.SplitDifferenceBehind[cameracar->id] != -1 && positiontable.DisplaySplitTime[cameracar->id])
	{
		font1->printf(628,95,"+");
		font1->printf(648,95,TimeString(positiontable.SplitDifferenceBehind[cameracar->id]));
	}
	if (positiontable.SplitDifferenceAhead[cameracar->id] != -1 && positiontable.DisplaySplitAheadTime[cameracar->id])
	{
		font1->printf(628,122,"-");
		font1->printf(648,122,TimeString(positiontable.SplitDifferenceAhead[cameracar->id]));
	}

	// gear indicator. First we need to figure out the number of gears we have.

	int numgears = cameracar->numgears-1;	// constant for now. This is the number of forward gears (i.e. not inc. R or N)
	
	DrawGeneric(593,567,10,18,(numgears+2)*28-4,16);

	// now draw the current gear

	DrawGeneric(593+cameracar->curgear*28,567-5,
		11+cameracar->curgear*28,35,
		22,22);


	// if end of race, draw message

	static float sizemult=0.00001f;

	if(raceglobals.state == RACE_AFTER)
	{	
		// if the race is finished, draw the 'race over' overlay.
		// Make it zoom in.

		int ww,hh;
		extern bool practice_mode;


		if(!Paused)
		{
			if(qualmode)
			{
				ww = 252*sizemult;
				hh = 60*sizemult;
				overlay.DrawRotatedGeneric(400-ww/2,300-hh/2,ww,hh,
					0,188,256,68,
					400,300,	0);
			}
			else if(!practice_mode)
			{
				ww = 240*sizemult;
				hh = 36*sizemult;
				overlay.DrawRotatedGeneric(400-ww/2,300-hh/2,ww,hh,
					0,150,256,38,
					400,300,	0);
			}
		}
		if(sizemult<1.5f)
		{
			sizemult += 0.05f;
		}
		else
			FadeOut();

	}
	else
	{
		sizemult=0.01;
		fadeout=0;
		PaperRotate = 15.0f;
	}
	oktorun=true;

}

void Overlay::Shutdown()
{
	if(font1)
	{
		delete font1;delete font2;delete font3;delete font4;
		font1=font2=font3=font4=NULL;
	}
}


void Overlay::SetJudder()
{
	cpjudder = cameracar->mph / 30;

	if(cpjudder && !Paused && !cameracar->engdead)
	{
		cx = rand()%cpjudder;
		cy = rand()%cpjudder;

		if(rand()%1)cx=-cx;
		if(rand()%1)cy=-cy;
	}
	else
	{
		cx = cy = 0;
	}
	cy -= cameracar->xangle*300.0f;
}

void Overlay::FadeOut()
{
	static long time;
	long alphaval;
	if(fadeout==0)
		time = timeGetTime();
	alphaval=((timeGetTime()-time)/6) + alphabase ;  //the bigger the slower
	if(alphaval>260)
		raceglobals.state = RACE_PAPER1;
	if(alphaval>255)
		alphaval=255;
#if 0
	DarkenScreen(0,0,800,600,alphaval<<24,0.0000001f);
#endif
	fadeout = 1;
}

void Overlay::DoPaper(bool demo)
{
	float scale = 1.0f/(PaperRotate+1.0f);
	long i;
	static bool PaperOk = false;
	static DWORD time;

	if(demo)
	{
		raceglobals.state = RACE_FINISHED;
		return;
	}

	if(PaperRotate > 14.95f)
	{
		if(arcPush(".\\archives\\papers.tc"))
		{
			char paperName[24];

			sprintf(paperName,"a%s.3df",game->trackname);
			papers[0].textureHandle = getTexturePtr(paperName,false,true)->textureHandle;
			paperName[0]++;
			papers[1].textureHandle = getTexturePtr(paperName,false,true)->textureHandle;
			paperName[0]++;
			papers[2].textureHandle = getTexturePtr(paperName,false,true)->textureHandle;
			paperName[0]++;
			papers[3].textureHandle = getTexturePtr(paperName,false,true)->textureHandle;
			time = timeGetTime();
		}
		if(papers[0].textureHandle)
			PaperOk = true;
	}
	if(PaperOk)
	{
		renderBeginScene(TRUE, 0x00);
		boris2->start2Dlist();
		boris2->renderstart();
		boris2->addTo2DlistRotated(400-(204*scale),300-(270*scale),204*scale,270*scale,0.0,0.0,1.0,1.0,papers[0].textureHandle,0,PaperRotate,400,300,DEPTH_GENERIC_ROTATED);
		boris2->addTo2DlistRotated(400,300-270*scale,204*scale,270*scale,0.0,0.0,1.0,1.0,papers[1].textureHandle,0,PaperRotate,400,300,DEPTH_GENERIC_ROTATED);
		boris2->addTo2DlistRotated(400-(204*scale),300,204*scale,270*scale,0.0,0.0,1.0,1.0,papers[2].textureHandle,0,PaperRotate,400,300,DEPTH_GENERIC_ROTATED);
		boris2->addTo2DlistRotated(400,300,204*scale,270*scale,0.0,0.0,1.0,1.0,papers[3].textureHandle,0,PaperRotate,400,300,DEPTH_GENERIC_ROTATED);
		boris2->render2Dlist();
		FASTFTOL(&i,512*scale);
		if(i>255)
			i=255;
#if 0
		DarkenScreen(0,0,800,600,(255-i)<<24,0.0000001f);
#endif
		PaperRotate-=0.1f;
		if(PaperRotate<=0.0f)
		{
			PaperRotate = 0.0f;
			if(timeGetTime()-time>5000)
				PaperOk = false;
		}
	}
	else
		raceglobals.state = RACE_FINISHED;
}


#define	BARHEIGHT	100
void Overlay::DrawDebugWidget(int x,int y,float height,char *title,float val,float minv,float maxv,float zero)
{
	// first draw the title
	extern font *testFont;

	if(!oktorun)return;

	testFont->printf(x,y,"%s %2.2f",title,val);
	y+=20;

	// vertical bar

	boris2->addTo2Dlistcolor(x+15,y,4,height,	0,0,0.1f,0.1f,	0xeeeeee);

	// work out parameters


	val -= minv;
	val /= (maxv-minv);
	val *= (float)height;
	if(val<0)val=0;

	// add horizontal block

	boris2->addTo2Dlistcolor(x,height+y-val,30,3,	0,0,0.1f,0.1f,	0xffffff);


	// add zero bar

	if(zero!=minv)
	{
		zero -= minv;
		zero /= (maxv-minv);
		zero *= (float)height;
		if(zero<0)zero=0;
		boris2->addTo2Dlistcolor(x,height+y-zero,30,3,	0,0,0.1f,0.1f,	0x80ff80);
	}


}
/*
void Overlay::LoadFlags()
{
	ParamFile p;
	int numflags;

	float flagsh = 256.0;
	float flagsw = 256.0;

	arcPush(".\\archives\\flags.tc");

	if(p.Read("flags"))
	{
		char *s;
		char buf[128];

		if(s=p.Find("numflags"))
			numflags = atoi(s);
		else
			fatalError("no numflags in flags file");

		for(int i=0;i<numflags;i++)
		{
			sprintf(buf,"flag%d",i);
			if(s=p.Find(buf))
			{
				strcpy(buf,s);
				if(s=strtok(buf,","))flags[i].x=atoi(s);
				if(s=strtok(NULL,","))flags[i].y=atoi(s);
				if(s=strtok(NULL,","))flags[i].w=atoi(s);
				if(s=strtok(NULL,","))flags[i].h=atoi(s);
				if(s=strtok(NULL,","))flags[i].tex=getTexturePtr(s,false,false)->textureHandle;
				dprintf("flagtex is %s",s);

				flags[i].u = ((float)(flags[i].x))/flagsw;
				flags[i].uw = ((float)(flags[i].w))/flagsw;
				flags[i].v = ((float)(flags[i].y))/flagsh;
				flags[i].vh = ((float)(flags[i].h))/flagsh;

				dprintf("flagtex : %lx %d,%d,%d,%d",flags[i].tex,flags[i].x,flags[i].y,flags[i].w,flags[i].h);

			}
			else fatalError("flag set  %s not found in flags file");
		}
	}
	arcPop();
}

				




void Overlay::DrawFlags()
{
	int corner=-1,middle=-1;

	static int flagpositions[] = {100,400,590};


	for(int i=0;i<3;i++)
	{
		if(showflags & (1<<i))
		{

			boris2->addTo2Dlist(
				-50,flagpositions[i],flags[i].w,flags[i].h,
			flags[i].u,flags[i].v,flags[i].uw,flags[i].vh,
			flags[i].tex,DEPTH_WHEEL);
		}
	}
}
*/


void Overlay::LoadFlags()
{

//	archivestack.Push("flags");
	arcPush(".\\archives\\flags.tc");

	// Load the actual bitmaps into memory.
	flagtexture = getTexturePtr("flagframes",false,false)->textureHandle;
	
	arcPop();
	// Don`t need this archive any more.
	//archivestack.Pop();


	//Set the flag positions
	for(int i=0;i<4;i++)
	{
		flags[i][0].x=0;
		flags[i][1].x=41;
		flags[i][2].x=76;
		flags[i][3].x=115;
		flags[i][4].x=157;
		flags[i][5].x=200;
		
		flags[i][0].w=40;
		flags[i][1].w=34;
		flags[i][2].w=38;
		flags[i][3].w=42;
		flags[i][4].w=42;
		flags[i][5].w=40;
	}

	for(i=0;i<6;i++)
	{
		flags[0][i].y=0;
		flags[1][i].y=64;
		flags[2][i].y=128;
		flags[3][i].y=192;
		
		flags[0][i].h=64;
		flags[1][i].h=64;
		flags[2][i].h=64;
		flags[3][i].h=64;
	}

	for(i=0;i<4;i++)
	{
		for(int j=0;j<6;j++)
		{
			flags[i][j].u=flags[i][j].x*0.00390625f;
			flags[i][j].v=flags[i][j].y*0.00390625f;
			flags[i][j].uw=flags[i][j].w*0.00390625f;
			flags[i][j].vh=flags[i][j].h*0.00390625f;
		}
	}

	pole.x=242;
	pole.y=0;
	pole.w=13;
	pole.h=255;

	pole.u=pole.x*0.00390625f;
	pole.v=pole.y*0.00390625f;
	pole.uw=pole.w*0.00390625f;
	pole.vh=pole.h*0.00390625f;

}

void Overlay::DrawFlags()
{
	

	static float scale=1.3f;

	static int flagpositions[] = {80,80,180,280};
	static int frame =0;
	static int framehold=0;

	if(++framehold>1)
	{
		if(++frame>5) frame=0;
		framehold=0;
	}

	

	for(int i=0;i<4;i++)
	{
		if(showflags & (1<<i))
		{

			boris2->addTo2DlistRotated(
				90-flags[i][frame].w*scale*1.4f,flagpositions[i],flags[i][frame].w*scale*1.4f,flags[i][frame].h*scale,
			flags[i][frame].u,flags[i][frame].v,flags[i][frame].uw,flags[i][frame].vh,
			flagtexture,0,-0.52f,90,flagpositions[i],DEPTH_WHEEL);

			
			boris2->addTo2DlistRotated(
				84,flagpositions[i],pole.w*scale*0.63f,pole.h*scale*.35f,
			pole.u,pole.v,pole.uw,pole.vh,
			flagtexture,0,-0.52f,90,flagpositions[i],DEPTH_FLAGPOLE);
			
		}


			

	}

}


void Overlay::LoadCarInfo()
{
	int i;
	arcPush(".\\archives\\pitstop.tc");

	// Load the actual bitmaps into memory.
	carinfotexture = getTexturePtr(resX==512?"512CarInfo":"CarInfo",false,false)->textureHandle;

	arcPop();
	// Don`t need this archive any more.
	//archivestack.Pop();


	//Set the positions

	if(resX==512)
	{
		for(int i=0;i<5;i++)
		{
			number[i].x=2;
			number[i].y=56+i*15-(i&1);

			number[i].w=8;
			number[i].h=10;
		}
		
		for(i=0;i<5;i++)
		{
			number[i+5].x=12;
			number[i+5].y=56+i*15-(i&1);

			number[i+5].w=8;
			number[i+5].h=10;
		}
		
		percent.x=4;
		percent.y=34;
		percent.w=14;
		percent.h=12;
	}
	else
	{
		for(int i=0;i<5;i++)
		{
			number[i].x=2;
			number[i].y=53+i*15;

			number[i].w=8;
			number[i].h=15;
		}
		
		for(i=0;i<5;i++)
		{
			number[i+5].x=12;
			number[i+5].y=53+i*15;

			number[i+5].w=8;
			number[i+5].h=15;
		}
		
		percent.x=4;
		percent.y=34;
		percent.w=14;
		percent.h=16;
	}	
	//Fuel
	infosymbols[0].x=24;
	infosymbols[0].y=66;
	infosymbols[0].w=38;
	infosymbols[0].h=55;

	//Oil
	infosymbols[1].x=2;
	infosymbols[1].y=2;
	infosymbols[1].w=64;
	infosymbols[1].h=29;

	//Car
	infosymbols[2].x=65;
	infosymbols[2].y=2;
	infosymbols[2].w=63;
	infosymbols[2].h=126;

	
	
	for(i=0;i<10;i++)
	{
		number[i].u=number[i].x*0.0078125f;
		number[i].v=number[i].y*0.0078125f;
		number[i].uw=number[i].w*0.0078125f;
		number[i].vh=number[i].h*0.0078125f;
		if(resX==512)
		{
			number[i].w*=688.0f/800.0f;
			number[i].h*=688.0f/800.0f;
		}
	}

	percent.u=percent.x*0.0078125f;
	percent.v=percent.y*0.0078125f;
	percent.uw=percent.w*0.0078125f;
	percent.vh=percent.h*0.0078125f;

	if(resX==512)
	{
		percent.w *= 688.0f/800.0f;
		percent.h *= 688.0f/800.0f;
	}

	infosymbols[0].u=infosymbols[0].x*0.0078125f;
	infosymbols[0].v=infosymbols[0].y*0.0078125f;
	infosymbols[0].uw=infosymbols[0].w*0.0078125f;
	infosymbols[0].vh=infosymbols[0].h*0.0078125f;

	infosymbols[1].u=infosymbols[1].x*0.0078125f;
	infosymbols[1].v=infosymbols[1].y*0.0078125f;
	infosymbols[1].uw=infosymbols[1].w*0.0078125f;
	infosymbols[1].vh=infosymbols[1].h*0.0078125f;

	infosymbols[2].u=infosymbols[2].x*0.0078125f;
	infosymbols[2].v=infosymbols[2].y*0.0078125f;
	infosymbols[2].uw=infosymbols[2].w*0.0078125f;
	infosymbols[2].vh=infosymbols[2].h*0.0078125f;

}


void Overlay::DrawCarInfo()
{

	static int sx[]={668,727,700};
	static int sy[]={160,187,250};
	
	float modifier;

	modifier = (resX==512)?512.0f/800.0f:1.0f;
#if 0
	DarkenScreen(657*modifier,155*modifier,138*modifier,231*modifier);	
#endif
	boris2->addTo2Dlist(
		sx[0],sy[0],infosymbols[0].w,infosymbols[0].h,
		infosymbols[0].u,infosymbols[0].v,infosymbols[0].uw,infosymbols[0].vh,
		carinfotexture,DEPTH_WHEEL);

  	
	RenderCarInfoPercent(sx[0]+10, sy[0]+60, cameracar->fuel);

	
	boris2->addTo2Dlist(
		sx[1],sy[1],infosymbols[1].w,infosymbols[1].h,
		infosymbols[1].u,infosymbols[1].v,infosymbols[1].uw,infosymbols[1].vh,
		carinfotexture,DEPTH_WHEEL);
	
	RenderCarInfoPercent(sx[1]+11, sy[1]+32, cameracar->oil);


	boris2->addTo2Dlist(
		sx[2],sy[2],infosymbols[2].w,infosymbols[2].h,
		infosymbols[2].u,infosymbols[2].v,infosymbols[2].uw,infosymbols[2].vh,
		carinfotexture,DEPTH_WHEEL);

	RenderCarInfoPercent(sx[2]-42, sy[2]+10, cameracar->tyresfront);
	RenderCarInfoPercent(sx[2]-42, sy[2]+90, cameracar->tyresback);

}


void Overlay::RenderCarInfoPercent(int x, int y, float pcnt)
{
	int digit1,digit2,digit3;
	float pc;

	if(pcnt>99.5f) pcnt=100.0f;
	if(pcnt<0.0f) pcnt=0.0f;

	pc=pcnt;
	
	digit1=(int)floor(pc/100.0f);
	pc-=100.0f*digit1;

	digit2=(int)floor(pc/10.0f);
	pc-=10.0f*digit2;

	digit3=pc;

	int offsetx=-9;

	if(digit2>0) offsetx=-5;
	if(digit1>0) offsetx=0;
	
	if(digit1>0)
	{
		boris2->addTo2Dlist(
			x,y,number[digit1].w,number[digit1].h,
			number[digit1].u,number[digit1].v,number[digit1].uw,number[digit1].vh,
			carinfotexture,DEPTH_WHEEL);
	}
	
	if(digit2>0 || digit1>0)
	{
		boris2->addTo2Dlist(
			x+9+offsetx,y,number[digit2].w,number[digit2].h,
			number[digit2].u,number[digit2].v,number[digit2].uw,number[digit2].vh,
			carinfotexture,DEPTH_WHEEL);
	}

	boris2->addTo2Dlist(
		x+18+offsetx,y,number[digit3].w,number[digit3].h,
		number[digit3].u,number[digit3].v,number[digit3].uw,number[digit3].vh,
		carinfotexture,DEPTH_WHEEL);
	
	boris2->addTo2Dlist(
		x+29+offsetx,y-1,percent.w,percent.h,
		percent.u,percent.v,percent.uw,percent.vh,
		carinfotexture,DEPTH_WHEEL);
}



void Overlay::StartFlag()
{
	
	if(!showstart) return;
	
	static float scale=1.7f;

	static int flagpositionx = 390;
	
	static int frame =0;
	static int framehold=0;

	int flagpositiony = flagpos*flagpos*flagpos*150.0f;

	float angle=0.75f*flagpos+1.97f;
	angle*=angle;

#if defined(UNDER_CE)
	float sn,cs;
	_SinCosA(&sn,&cs, angle);
	angle=-0.5f*sn-0.85f;

#else
	angle=-0.5f*(sin(angle))-0.85f;
#endif
	float squash=flagpos*9.0f-4.0f;
	if(squash<-1.0f) squash=-1.0f;
	if(squash>1.0f) squash=1.0f;
	
//	grdebug("squash",squash);
//	grdebug("fpos",flagpos);

	if(++framehold>1)
	{
		if(++frame>5) frame=0;
		framehold=0;
	}

	if(squash<0.0f)
	{
		squash=(1.0f+squash)*flags[2][frame].w*scale*1.4f;
		
//		grdebug("nsquash",squash);

		boris2->addTo2DlistRotated(
			squash+flagpositionx-flags[2][frame].w*scale*1.4f,flagpositiony,flags[2][frame].w*scale*1.4f-squash,flags[2][frame].h*scale,
		flags[2][frame].u,flags[2][frame].v,flags[2][frame].uw,flags[2][frame].vh,
		flagtexture,0,angle,flagpositionx+100,flagpositiony+100,DEPTH_WHEEL);

		
		boris2->addTo2DlistRotated(
			flagpositionx-7,flagpositiony,pole.w*scale*0.63f,pole.h*scale*.35f,
		pole.u,pole.v,pole.uw,pole.vh,
		flagtexture,0,angle,flagpositionx+100,flagpositiony+100,DEPTH_FLAGPOLE);
	}		
	else
	{
		squash=(1-squash)*flags[2][frame].w*scale*1.4f;
		
//		grdebug("nsquash",squash);

		boris2->addTo2DlistRotated(
			flagpositionx,flagpositiony,flags[2][frame].w*scale*1.4f-squash,flags[2][frame].h*scale,
		flags[2][frame].u+flags[2][frame].uw,flags[2][frame].v,-flags[2][frame].uw,flags[2][frame].vh,
		flagtexture,0,angle,flagpositionx+100,flagpositiony+100,DEPTH_WHEEL);

		
		boris2->addTo2DlistRotated(
			flagpositionx-7,flagpositiony,pole.w*scale*0.63f,pole.h*scale*.35f,
		pole.u,pole.v,pole.uw,pole.vh,
		flagtexture,0,angle,flagpositionx+100,flagpositiony+100,DEPTH_FLAGPOLE);
	}		


}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////	


Overlay::Overlay()
{
	font1=font2=font3=font4=font5=NULL;
	UseDriverAids = oktorun = false;
	alphabase=0;
	flagpos=0.0f;

}

Overlay::~Overlay()
{
	Shutdown();
}

