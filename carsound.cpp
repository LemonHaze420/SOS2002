// $Header$

// $Log$
// Revision 1.2  2002-07-31 10:00:23+01  img
// Removed grdebug() calls
//
// Revision 1.1  2002-07-22 10:06:40+01  jjs
// Test version of SOS2002
//
// Revision 1.0  2000-02-21 12:34:10+00  jjs
// Initial revision
//

////////////// interface to the sound library for car sounds


#include <windows.h>
#include <stdio.h>
#if !defined(UNDER_CE)
#include <process.h>
#include <new.h>
#endif
#include <stdlib.h>

#include "BS2all.h"
#include "ALLcar.h"
#include "sound.h"
#include "frontend.h"

#include "carsound.h"
#include "overlay.h"

#include "param.h"
#include "camera.h"
#include "list.h"

#include "force.h"

//#include "Frontlib2.h"

extern void dprintf(char *,...);
extern car *cameracar,*cararray[];
extern int cameraview;
extern bool NoPlayers;
extern CameraBank *AllCameras;
extern bool Paused;

#define	VOL_3DSPOT	1.0f	// spot effect amplitude (skid, crash)
#define	VOL_3DCAR	0.7f	// amplitude of other car engines
#define	VOL_INCAR	0.3f	// amplitude of engine when in-car
#define	VOL_SURF	1.7f	// surface sound 
#define	GAIN_INCAR	1.4f
#define	GAIN_BUMPER	1.6f

#define	ALWAYS_LCAR_NOT_3D	1

#define	GETVELOCITY(x)	((x)*(float)TICKRATE*(float)VELOCITYSCALINGCONSTANT);
#define	snark dprintf
#define dprintf(x)

float internal_vol=1.0f;

CarSound carsounds;

struct
{
	char *filename;
	int id;
	int pri;
	float vol;
} soundtable [] =
{
	// the id should be -ve if this to be a non-3d sound

	{"SmallImpact",	SND_COLLISION1,10,	1.0f},
	{"MediumImpact",	SND_COLLISION2,10,	1.0f},
	{"LargeImpact",	SND_COLLISION3,10,	1.0f},
	{"comp_skid_01",	SND3D_SKID,9,			1.2f},
	{"EngineExplo",SND_EXPLODE,11,			1.0f},

	{"gearup_0to1",SND_GEARCHANGEUPREV,7,				1.0f},
	{"gearup_1to2",SND_GEARCHANGEUPNEUT,7,				1.0f},
	{"gearup_2to3",SND_GEARCHANGEUP1,7,				1.0f},
	{"gearup_3to4",SND_GEARCHANGEUP2,7,				1.0f},
	{"gearup_4to5",SND_GEARCHANGEUP3,7,				1.0f},
	{"gearup_5to6",SND_GEARCHANGEUP4,7,				1.0f},

	{"geardown_1to0",SND_GEARCHANGEDNNEUT,7,				1.0f},
	{"geardown_2to1",SND_GEARCHANGEDN1,7,				1.0f},
	{"geardown_3to2",SND_GEARCHANGEDN2,7,				1.0f},
	{"geardown_4to3",SND_GEARCHANGEDN3,7,				1.0f},
	{"geardown_5to4",SND_GEARCHANGEDN4,7,				1.0f},
	{"geardown_6to5",SND_GEARCHANGEDN5,7,				1.0f},

	{"Hilight",SND_HIGHLIGHT,12,1.0f},
	{"MenuPopup",SND_MENU,12,1.0f},
	{"MouseClick",SND_CLICK,12,1.0f},


	{"boing",	SND_BOING,12,					1.0f},	// debugging noise


	{NULL,0,0}	// terminator
};

//
//	This function is a thread started in Initialise and stopped in
//	Shutdown. It processes and plays exhaust 'blats'.

static bool exhaust_thread_running=true;
static bool exhaust_thread_active=false;
void exhaust_thread()
{
	// the thread runs while this boolean is true.

	exhaust_thread_active = true;

	while(exhaust_thread_running)
	{
		Sleep(20);	// a bit of a gap between each run
#if 1

		CarSoundParams *camcarparmset = carsounds.camcarparmset;
		// We can only generate exhaust sounds if there's an
		// exhaust car present and its engine is OK.

		if(camcarparmset && carsounds.exhaust_car && !carsounds.exhaust_car->engdead)
		{
			// the base chance of an exhaust sound is determined by the
			// revs and the parameters 'exhaustgrad' and 'exhaustint',

			float revs = carsounds.exhaust_car->revs;
			float chance_of_exhaust = revs * 
				camcarparmset->exhaustgrad+camcarparmset->exhaustint;

			// it's then made more likely if the revs have been
			// consistently falling for a period of time.

			float revd = -carsounds.integral_revdiff;
			if(revd<0)revd=0;

			// this plays a backfire sound when intrevdiff falls
			// below a certain level, and then sets intrevdiff
			// to a low value so it doesn't happen again for a while

			if(revd>camcarparmset->backfirethresh && !(rand()%30))
			{
				if(carsounds.backfire_sound[0]->is3d)
				{
					carsounds.backfire_sound[rand()%2]->Play3DNoDuplicate(NULL,false,	
						carsounds.exhaust_car->mainObject->transform.translation.x,
						carsounds.exhaust_car->mainObject->transform.translation.y,
						carsounds.exhaust_car->mainObject->transform.translation.z,
						0,0,0,		internal_vol,1);
				}
				else
				{
					carsounds.backfire_sound[rand()%2]->PlayNoDuplicate(NULL,false,internal_vol,
						0,1);
				}

				carsounds.integral_revdiff = 0;

				Jolt(EFF_HARD,1000,0.01f,(rand()%2) ? -90 : 90);
			}

			chance_of_exhaust *= 1.0f+(revd*camcarparmset->exhaustrevdiff);

			if(((float)(rand()%1000))*0.001f < chance_of_exhaust)
			{
				// it's been decided that we're going to play an exhaust sound,
				// so we now determine the volume of that sound. The base value
				// is carsounds.exhaustvolincar/voloutcar

				float vol = (cameraview == CAM_INCAR) ? 
						camcarparmset->exhaustvolincar : 
						camcarparmset->exhaustvoloutcar;

				vol *= internal_vol * 0.4f;
				if(cameraview!=CAM_INCAR && cameraview!=CAM_BUMPER) vol*=0.1f;

				// then we make it louder if the revs are falling off

				vol *= 1.0f+(revd*camcarparmset->exhaustrevdiff);

				// finally we make it quieter at higher revs, and clip it

				vol *= (1.0f - (revs*camcarparmset->exhaustfalloff*0.0001f));
				if(vol>1.0f)vol=1.0f;
				
				// and then we play it, if it would be audible.

				if(vol>0.0f)
				{
					if(carsounds.exhaust_sound[0]->is3d)
					{
						carsounds.exhaust_sound[rand()%10]->Play3DNoDuplicate(NULL,false,	
							carsounds.exhaust_car->mainObject->transform.translation.x,
							carsounds.exhaust_car->mainObject->transform.translation.y,
							carsounds.exhaust_car->mainObject->transform.translation.z,
							0,0,0,		1.0,1);
					}
					else
					{
						carsounds.exhaust_sound[rand()%10]->PlayNoDuplicate(NULL,false,vol,
							0,1);
					Jolt(EFF_HARD,vol*1000,0.01f,(rand()%2) ? -90 : 90);
					}
				}
			}
		}
#endif
	}

	exhaust_thread_active = false;
#if !defined(UNDER_CE)
	_endthread();
#endif

}

typedef void (__cdecl *THREADFUNC)(void *);
int ambcnt=0;


void CarSound::Initialise(HWND main_hwnd,bool use_eax)
{
	bool soundrv = soundsys.Initialise((long)main_hwnd,false,2.0);
	
	carvolume = 1.0;

	ambcnt=0;
	prevrevs=0.0;
	dampedaccel=0.0;
	dampedrevs=0.0;

	initialised=false;
	isplayinginternal = false;
	debugflags = 0L;
	exhaust_car = lastcar = NULL;
	integral_revdiff=0.0;
	lastsurfacetype=-1;

	exhaust_thread_running = true;
	exhaust_thread_active = false;

	soundsys.eax_on = load_internal_as_3D = use_eax;
	lastcar=NULL;
	top_engine = NULL;


	dprintf("WinMain: sound system initialised - eax mode %s",load_internal_as_3D ? "true" : "false");
	char buf[128];
	int i;

	arcPush(".\\archives\\sound.tc");

	for(i=0;i<NUMCARSOUNDSLOTS;i++)slots[i]=NULL;

	for(i=0;i<MAXSOUNDS;i++)
	{	
		soundtable3d[i] = soundtablenon3d[i] = NULL;
	}
	for(i=0;i<NUMCARSOUNDPAIRS;i++)
		pairs[i].c = NULL;

	// load the static sounds

	for(i=0;;i++)
	{
		if(!soundtable[i].filename)break;

		sprintf(buf,"%s.wav",soundtable[i].filename);
		if(soundtable[i].id < 0)
		{
			dprintf("loading static non3d sound %s into slot %d",
				soundtable[i].filename,-(soundtable[i].id + 1));

			if(!(soundtablenon3d[-(soundtable[i].id + 1)] = 
				soundsys.FindOrLoadSound(buf,true,false,soundtable[i].pri)))
			{
				fatalError("unable to load sound %s",soundtable[i].filename);
			}
			soundtablenon3d[-(soundtable[i].id+1)]->volmult = soundtable[i].vol;
		}
		else
		{
			dprintf("loading static 3d sound %s into slot %d",soundtable[i].filename,soundtable[i].id);

			if(!(soundtable3d[soundtable[i].id] = 
				soundsys.FindOrLoadSound(buf,true,true,soundtable[i].pri)))
			{
				fatalError("unable to load sound %s",soundtable[i].filename);
			}
			soundtable3d[soundtable[i].id]->volmult = soundtable[i].vol;
		}
	}

	
	dprintf("static sounds loaded");
	initialised = true;

	// load the exhaust sounds
	exhaust_sound[0] = soundsys.DuplicateOrLoadSound("NL_exh_01.wav",true,false,10);
	exhaust_sound[1] = soundsys.DuplicateOrLoadSound("NL_exh_02.wav",true,false,10);
	exhaust_sound[2] = soundsys.DuplicateOrLoadSound("NL_exh_03.wav",true,false,10);
	exhaust_sound[3] = soundsys.DuplicateOrLoadSound("NL_exh_04.wav",true,false,10);
	exhaust_sound[4] = soundsys.DuplicateOrLoadSound("NL_exh_05.wav",true,false,10);
	exhaust_sound[5] = soundsys.DuplicateOrLoadSound("NL_exh_06.wav",true,false,10);
	exhaust_sound[6] = soundsys.DuplicateOrLoadSound("NL_exh_07.wav",true,false,10);
	exhaust_sound[7] = soundsys.DuplicateOrLoadSound("NL_exh_08.wav",true,false,10);
	exhaust_sound[8] = soundsys.DuplicateOrLoadSound("NL_exh_09.wav",true,false,10);
	exhaust_sound[9] = soundsys.DuplicateOrLoadSound("NL_exh_10.wav",true,false,10);
/*
	exhaust_sound[10] = soundsys.DuplicateOrLoadSound("NL_exh_11.wav",false,false,10);
	exhaust_sound[11] = soundsys.DuplicateOrLoadSound("NL_exh_12.wav",false,false,10);
	exhaust_sound[12] = soundsys.DuplicateOrLoadSound("NL_exh_13.wav",false,false,10);
	exhaust_sound[13] = soundsys.DuplicateOrLoadSound("NL_exh_14.wav",false,false,10);
	exhaust_sound[14] = soundsys.DuplicateOrLoadSound("NL_exh_15.wav",false,false,10);
	exhaust_sound[15] = soundsys.DuplicateOrLoadSound("NL_exh_16.wav",false,false,10);
	exhaust_sound[16] = soundsys.DuplicateOrLoadSound("NL_exh_17.wav",false,false,10);
	exhaust_sound[17] = soundsys.DuplicateOrLoadSound("NL_exh_18.wav",false,false,10);
	exhaust_sound[18] = soundsys.DuplicateOrLoadSound("NL_exh_19.wav",false,false,10);
	exhaust_sound[19] = soundsys.DuplicateOrLoadSound("NL_exh_20.wav",false,false,10);
*/

	backfire_sound[0] = soundsys.DuplicateOrLoadSound("bfire01.wav",false,load_internal_as_3D,10);
	backfire_sound[1] = soundsys.DuplicateOrLoadSound("bfire02.wav",false,load_internal_as_3D,10);

	skid_sound[0] = soundsys.DuplicateOrLoadSound("skid.wav",true,false,10);
	skid_sound[1] = soundsys.DuplicateOrLoadSound("skid.wav",true,false,10);
	skid_sound[2] = soundsys.DuplicateOrLoadSound("skid.wav",true,false,10);
	skid_sound[3] = soundsys.DuplicateOrLoadSound("skid.wav",true,false,10);

	for(i=0;i<10;i++)
		if(!exhaust_sound[i])fatalError("no exhaust %d",i);
	for(i=0;i<2;i++)
		if(!backfire_sound[i])fatalError("no backfire %d",i);


	// initialise the exhaust thread
#if !defined(UNDER_CE)
	_beginthread((THREADFUNC)exhaust_thread,0,NULL);
#endif
}


void CarSound::LoadParameterSet(int n,char *carname)
{
	char buf[64],*sp;


	ParamFile p;

	strcpy(buf,carname);carname=buf;
	if(sp=strchr(buf,'_'))*sp=0;

	arcPush(".\\archives\\sound.tc");	// er.. you end up with this archive twice, but no matter, I hope.

	if(!p.Read(carname))
		fatalError("cannot find car sound parameters %s",carname);
	else
	{
		
		CarSoundParams *paramptr = params+n;

		rppctr=0; ppctr = 0;curparm=0;
		
#define GETPARAM(x,y,pp,qq,xx) \
	s = p.Find(y);	if(!s)x=pp; else sscanf(s,"%f",&x); \
	rppctr++; paramnames[ppctr] = y; paramptrs[ppctr]= &x; parammins[ppctr]=pp;parammaxs[ppctr]=qq;\
		paramgraphs[ppctr++]=xx
		
#define GETPARAMNOEDIT(x,y) \
	s = p.Find(y);	if(!s)fatalError("unable to find %s in carsound parameters",y); \
	sscanf(s,"%f",&x); \
		paramnames[rppctr] = y; paramptrs[rppctr++]= &x



		char *s;
		
		GETPARAM(paramptr->vol_intercept_bass,"volintbass",-100,100,GRAPH_BASS);
		GETPARAM(paramptr->vol_gradient_bass,"volgradbass",0,0.3f,GRAPH_BASS);
		GETPARAM(paramptr->pitch_intercept_bass,"pitchintbass",-100,100,GRAPH_BASS);
		GETPARAM(paramptr->pitch_gradient_bass,"pitchgradbass",0,0.1f,GRAPH_BASS);
		
		GETPARAM(paramptr->vol_intercept_mid,"volintmid",-100,100,GRAPH_MID);
		GETPARAM(paramptr->vol_gradient_mid,"volgradmid",0,0.3f,GRAPH_MID);
		GETPARAM(paramptr->pitch_intercept_mid,"pitchintmid",-100,100,GRAPH_MID);
		GETPARAM(paramptr->pitch_gradient_mid,"pitchgradmid",0,0.1f,GRAPH_MID);
		
		GETPARAM(paramptr->vol_intercept_top,"volinttop",-100,100,GRAPH_TOP);
		GETPARAM(paramptr->vol_gradient_top,"volgradtop",0,0.3f,GRAPH_TOP);
		GETPARAM(paramptr->pitch_intercept_top,"pitchinttop",-100,100,GRAPH_TOP);
		GETPARAM(paramptr->pitch_gradient_top,"pitchgradtop",0,0.1f,GRAPH_TOP);
		
		GETPARAM(paramptr->vol_max_bass,"volmaxbass",0,100,GRAPH_BASS);
		GETPARAM(paramptr->vol_max_mid,"volmaxmid",0,100,GRAPH_MID);
		GETPARAM(paramptr->vol_max_top,"volmaxtop",0,100,GRAPH_TOP);
		
		GETPARAM(paramptr->pitch_min_bass,"pitchminbass",0.01f,0.4f,GRAPH_BASS);
		GETPARAM(paramptr->pitch_min_mid,"pitchminmid",0.01f,0.4f,GRAPH_MID);
		GETPARAM(paramptr->pitch_min_top,"pitchmintop",0.01f,0.4f,GRAPH_TOP);
		
		GETPARAM(paramptr->exhaustfalloff,"exhaustfalloff",0,5,GRAPH_TOP);
		GETPARAM(paramptr->gearrevdampingdelay,"gearrevdampingdelay",5,20,GRAPH_TOP);	// in ticks
		GETPARAM(paramptr->gearrevdampingstep,"gearrevdampingstep",5,20,GRAPH_TOP);	// in ticks
		
		GETPARAM(paramptr->gearrevdepthtop,"gearrevdepthtop",0,1,GRAPH_TOP);	// in ticks
		GETPARAM(paramptr->gearrevdepthlow,"gearrevdepthlow",0,1,GRAPH_TOP);	// in ticks
		
		
		GETPARAMNOEDIT(paramptr->pitch_gradient_external,"pitchgradext");
		GETPARAMNOEDIT(paramptr->pitch_intercept_external,"pitchintext");
		
		GETPARAMNOEDIT(paramptr->bassthrottle,"bassthrottle");
		GETPARAMNOEDIT(paramptr->midthrottle,"midthrottle");
		GETPARAMNOEDIT(paramptr->topthrottle,"topthrottle");
		
		GETPARAMNOEDIT(paramptr->revdamping,"revdamping");
		
		GETPARAMNOEDIT(paramptr->exhaustgrad,"exhaustgrad");
		GETPARAMNOEDIT(paramptr->exhaustint,"exhaustint");
		GETPARAMNOEDIT(paramptr->exhaustvoloutcar,"exhaustvoloutcar");
		GETPARAMNOEDIT(paramptr->exhaustvolincar,"exhaustvolincar");
		GETPARAMNOEDIT(paramptr->exhaustrevdiff,"exhaustrevdiff");
		GETPARAMNOEDIT(paramptr->backfirethresh,"backfirethresh");		
	}

	arcPop();
}
void CarSound::Shutdown()
{
	dprintf("entering shutdown : waiting for thread");
	exhaust_thread_running = false;

	if(initialised)arcPop();
	
	do
	{
		Sleep(200);	// to give the exhaust thread time to die
	} while(exhaust_thread_active);
	
	dprintf("thread dead. Commencing shutdown.");
	for(int i=0;i<MAXSOUNDS;i++)
	{
		soundtable3d[i] = soundtablenon3d[i] = NULL;
	}
	dprintf("sound table cleared");
	
	soundsys.Shutdown();
	dprintf("soundsystem shutdown complete");
	
	initialised=false;
}

int CarSound::LoadSound(char *filename, bool is3d,int pri)
{
	// find a free slot and load the sound into it.
	
	// this may need to be hacked to work from the archive system too.
	
	int i;
	char buf[128];
	
	sprintf(buf,"%s.wav",filename);
	
	if(!initialised)return 0;
	
	Sound *s = soundsys.FindOrLoadSound(buf,true,is3d,pri);
	if(!s)fatalError("unable to load sound %s",buf);
	
	if(is3d)
	{
		// first see if we already have it
		
		for(i=0;i<MAXSOUNDS;i++)
			if(soundtable3d[i] == s)return i;
			
			// otherwise load it
			
			for(i=0;i<MAXSOUNDS;i++)
			{
				if(!soundtable3d[i])
				{
					dprintf("loading dynamic 3d sound %s into slot %d",filename,soundtable[i].id);
					soundtable3d[i] = s;
					return i;
				}
			}
			fatalError("cannot find a free slot for dynamic sound %s",filename);
	}
	else
	{
		// first see if we already have it
		
		for(i=0;i<MAXSOUNDS;i++)
			if(soundtablenon3d[i] == s)return -(i+1);
			
			for(i=0;i<MAXSOUNDS;i++)
			{
				if(!soundtablenon3d[i])
				{
					dprintf("loading dynamic 3d sound %s into slot %d",filename,soundtable[i].id);
					soundtablenon3d[i] = s;
					return -(i+1);
				}
			}
			fatalError("cannot find a free slot for dynamic sound %s",filename);
	}
	return 0; // cos otherwise the compiler complains.
}


void CarSound::StopAll()
{
	soundsys.StopAllSounds();
}


void CarSound::NominateListener(class camera *c)
{
	if(!initialised)return;
	listener = c;
}

int CarSound::Start3DSound(int id,car *carptr,bool loop)
{
	// start a 3D sound at the car's position and vel, and return the channel (or -1)
	
	if(!initialised)return 0;
	if(id<0)fatalError("attempted to play a non3D sound in 3D mode %d",id);
	
	// replace with actual code to get velocity and heading
	
	vector3 velocity = carptr->velocity;
	
	int rv = 
		soundsys.Play3D(soundtable3d[id],loop,
		
		carptr->mainObject->transform.translation.x,
		carptr->mainObject->transform.translation.y,
		carptr->mainObject->transform.translation.z,
		
		velocity.x,velocity.y,velocity.z,VOL_3DSPOT,1);
	
	if(rv<0)
	{
		dprintf("FATAL ERROR in Start3DSound - %s",soundsys.GetError());
	}
	
	return rv;
}

char *top_carsounds[]=			// put the different topcar sounds in here
{
	"high_03.wav",
	"high_03.wav",
	"high_03.wav",
	"high_03.wav",
	"high_03.wav",
};

extern car *playercar;
float gearchangemulttop,gearchangemultlow;



void CarSound::LoadEngineSounds(int ncs,class car **carptr)
{

	if(!initialised)return;
	numcars = ncs;
	for(int cidx=0;cidx<numcars;cidx++)
	{
		// here we load or duplicate a Sound (using an actual new DirectSound buffer) for
		// each sound we need for each external car composite sound

		static char *extcarsounds[]= {"NPC_01.wav","NPC_02.wav","NPC_03.wav","NPC_04.wav"};

		comp_engine[cidx] = soundsys.DuplicateOrLoadSound(extcarsounds[rand()%4],true,true,10);
	}

	// load engine bands
	low_engine = soundsys.DuplicateOrLoadSound("low_01.wav",true,load_internal_as_3D,12);
	high_engine = soundsys.DuplicateOrLoadSound("mid_01.wav",true,load_internal_as_3D,12);
	top_engine = soundsys.DuplicateOrLoadSound(top_carsounds[0],true,load_internal_as_3D,12);

	enginefire = soundsys.DuplicateOrLoadSound("enginefire.wav",false,false,12);

	// load surface sounds

	surface_l[SURF_TARMAC] = soundsys.DuplicateOrLoadSound("gen_surface_02.wav",true,false,8);	// mono
	surface_f[SURF_TARMAC] = 0;

	surface_l[SURF_GRASS] = soundsys.DuplicateOrLoadSound("gen_surface_01.wav",true,false,8);	// mono
	surface_r[SURF_GRASS] = soundsys.DuplicateOrLoadSound("gen_surface_01.wav",true,false,8);	// mono
	surface_f[SURF_GRASS] = SURFF_LOUD | SURFF_STEREO;

	surface_l[SURF_GRAVEL] = soundsys.DuplicateOrLoadSound("gravel_loop1.wav",true,false,8);	// left
	surface_r[SURF_GRAVEL] = soundsys.DuplicateOrLoadSound("gravel_loop2.wav",true,false,8);	// right
	surface_f[SURF_GRAVEL] = SURFF_STEREO | SURFF_LOUD;

	// you'll note it doesn't actually start them, it just loads them.

}

void CarSound::ResetEngineSounds()
{
	for(int i=0;i<NUMCARSOUNDSLOTS;i++)
	{
		if(slots[i])
		{
			comp_engine[slots[i]->id]->Stop();
			slots[i]->compsoundplaying=false;
			slots[i]=NULL;
		}
	}

	skid_sound[0]->Stop();
	skid_sound[1]->Stop();
	skid_sound[2]->Stop();
	skid_sound[3]->Stop();
}

#define	FACTOR	0.6

float inline remaprevs(car *c, float r)
{
/*
	float lim = c->maxrevs * 0.6;

	r*= 1.0/FACTOR;
	if(r > lim)
	{
		r -= lim;
		r *= FACTOR;
		r += lim;
	}
*/
	return r;
}
	
void CarSound::UpdateEngineSounds()
{
	if(!initialised)return;
	int i;
 
	car *tmpcamcar;	// horrible hack. We'll annull this if we're in replay mode


	tmpcamcar = replaymode ? NULL : cameracar;

	// step 1 - find the nearest car which is NOT in the list of cars currently playing.

	//		find the slot number for the furthest car, or alternatively the first free slot

	int furthestplaying;
	float furthestdist = 0.0f;
	float dist;

	vector3 *p;
	
//	dprintf("	getting positions");
	p = replaymode ?
		&(boris2->currentcamera->position) : 
		&(tmpcamcar->mainObject->transform.translation);

//	dprintf("	resetting");
	if(lastcar!=tmpcamcar)
	{
		// if the camera has changed reset the sound system.

		ResetEngineSounds();

	}



	if(replaymode)exhaust_car = NULL;

//	dprintf("iterator 1");
	for(i=0;i<NUMCARSOUNDSLOTS;i++)
	{
		if(!slots[i] || slots[i] == tmpcamcar)
		{
			furthestplaying=i;	// furthestplaying is a bit of a misnomer in this case; it's just a free slot
			break;
		}

		dist = slots[i]->mainObject->transform.translation.distanceSquared(p);

		if(dist>furthestdist)
		{
			furthestdist=dist;
			furthestplaying=i;
		}
	}

//	dprintf("iterator 2");
	// now find the nearest car which ain't in the list. You can tell it's not in the list by looking
	// at the compsoundplaying flag in the car (which is new).

	float nearestdist=10000.0f;
	int nearest = -1;

	for(i=0;i<numcars;i++)
	{
		car *nn = cararray[i];
		if(tmpcamcar != nn && !(nn->compsoundplaying) && !(nn->engdead))
		{

			dist = cararray[i]->mainObject->transform.translation.distanceSquared(p);

			if(dist<nearestdist)
			{
				nearestdist = dist;nearest = i;
			}
		}
	}

	if(nearest>=0)
	{
		// Now then. If there's a free slot, add this nearest car to the playing sounds.
		// If the nearest car not playing is closer than the furthest car playing,
		// remove the latter and add the former.

//		dprintf("slot OK");

		vector3 *nearestp = &(cararray[nearest]->mainObject->transform.translation);

		if(!slots[furthestplaying])
		{
			// start the sound playing in that slot

			car *nn = cararray[nearest];
			slots[furthestplaying]= cararray[nearest];
			comp_engine[nn->id]->Play3D(NULL,true,nearestp->x,0,nearestp->z,
				nn->velocity.x,nn->velocity.y,nn->velocity.z,VOL_3DCAR,0.2f);
			nn->compsoundplaying = true;
//			dprintf("	ADDING car %lx into free slot %d",nn,furthestplaying);
		}
		else if(nearestdist < furthestdist)
		{
			// remove the old sound in the slot

			comp_engine[slots[furthestplaying]->id]->Stop();
			slots[furthestplaying]->compsoundplaying=false;

			// start the new sound playing in the slot

//			dprintf("	REMOVING car %lx from slot %d",slots[furthestplaying],furthestplaying);
			car *nn = cararray[nearest];
			slots[furthestplaying] = nn;
			comp_engine[nn->id]->Play3D(NULL,true,nearestp->x,0,nearestp->z,
				nn->velocity.x,nn->velocity.y,nn->velocity.z,VOL_3DCAR,0.2f);
			nn->compsoundplaying = true;
///			dprintf("	ADDING car %lx to slot %d",nn,furthestplaying);
		}
	}
			
//	dprintf("added");

	// now set the frequencies, etc.

	float realrevs,revs;

	for(i=0;i<NUMCARSOUNDSLOTS;i++)
	{
		car *c = slots[i];

		if(c)
		{
			realrevs = c->revs;
			revs = realrevs;		// calculate pitch from revs
			if(revs<1000)revs=1000.0;

			Sound *s = comp_engine[c->id];
//			dprintf("processing sound %d - car %lx, sound %lx",i,c,s);

			s->SetFrequency(revs * camcarparmset->pitch_gradient_external * 0.01f + 
				camcarparmset->pitch_intercept_external * 0.01f);

			if(c->engdead)
			{
				// engine dead; remove from slot system.
				s->Stop();
				slots[i]= NULL;
				c->compsoundplaying=false;
			}
		}

//		if(enginefire->IsPlaying())		// huh?
//			enginefire->Stop();
	}

//	dprintf("freqs set");

	// now deal with the camera car

	if(replaymode)
	{
//		dprintf("stopping camcar");
		if(top_engine && top_engine->IsPlaying())
		{
			low_engine->Stop();
			top_engine->Stop();
			high_engine->Stop();
		}
//		dprintf("returning");
		return;
	}

//	dprintf("OK");

	realrevs = remaprevs(cameracar,cameracar->revs);
	revs = realrevs;		// calculate pitch from revs
	if(revs<1000)revs=1000.0;

	dampedrevs = camcarparmset->revdamping*dampedrevs + (1.0f-camcarparmset->revdamping)*revs;

	float revdiff = dampedrevs - prevrevs;
	prevrevs = dampedrevs;

	integral_revdiff += revdiff;
	integral_revdiff *= 0.9f;

	//grdebug("intrevdiff",integral_revdiff);

// integral revdiff controls backfires. Or will do.

	//grdebug("revdiff",revdiff);

	float myx = p->x;
	float myz = p->z;

	if(lastcar != cameracar)
	{
		// we've switched to a new camera car - so stop the old sounds and start up new ones

		overlay.SwitchToDash(cameracar->id);

		if(top_engine && top_engine->IsPlaying())
		{
			low_engine->Stop();
			top_engine->Stop();
			high_engine->Stop();
		}

//		dprintf("sound reloaded OK");
		if(load_internal_as_3D)
		{
			top_engine->Play3D	(NULL,true,	myx,0,myz,	0,0,0,		0.0,1);
			top_engine->buf3d->SetMode(DS3DMODE_DISABLE,DS3D_IMMEDIATE);
		}
		else
		{
			top_engine->Play(NULL,true,0,0,1);
		}

		lastcar = cameracar;
	}

	// start the two internal sounds if necessary, at very low volume

	if(!low_engine->IsPlaying())
	{
		if(low_engine->is3d)
		{
			low_engine->Play3D	(NULL,true,	myx,0,myz,	0,0,0,		0.0,1);
			high_engine->Play3D	(NULL,true,	myx,0,myz,	0,0,0,		0.0,1);
//			top_engine->Play3D	(NULL,true,	myx,0,myz,	0,0,0,		0.0,1);

			low_engine->buf3d->SetMode(DS3DMODE_DISABLE,DS3D_IMMEDIATE);
			high_engine->buf3d->SetMode(DS3DMODE_DISABLE,DS3D_IMMEDIATE);
//			top_engine->buf3d->SetMode(DS3DMODE_DISABLE,DS3D_IMMEDIATE);
		}
		else
		{
			low_engine->Play(NULL,true,0,0,1);
			high_engine->Play(NULL,true,0,0,1);
//			top_engine->Play(NULL,true,0,0,1);
		}


		isplayinginternal=true;
	}

	// set the frequencies

//			dampedrevs *= 0.9;

	float lowptc,highptc,topptc;

	lowptc = (dampedrevs * camcarparmset->pitch_gradient_bass + camcarparmset->pitch_intercept_bass)*0.01f;
	highptc = (dampedrevs * camcarparmset->pitch_gradient_mid + camcarparmset->pitch_intercept_mid)*0.01f;
	topptc = (dampedrevs * camcarparmset->pitch_gradient_top + camcarparmset->pitch_intercept_top)*0.01f;

extern float debug_pitch_low,debug_pitch_high,debug_pitch_top;

	if(lowptc<camcarparmset->pitch_min_bass)lowptc=camcarparmset->pitch_min_bass;
	if(highptc<camcarparmset->pitch_min_mid)highptc=camcarparmset->pitch_min_mid;
	if(topptc<camcarparmset->pitch_min_top)topptc=camcarparmset->pitch_min_top;

	low_engine->SetFrequency(lowptc);
	high_engine->SetFrequency(highptc);
	top_engine->SetFrequency(topptc);

	debug_pitch_low = lowptc;	debug_pitch_high = highptc; 	debug_pitch_top = topptc;

	// calculate the amplitudes

	float amp_low = (dampedrevs * camcarparmset->vol_gradient_bass + camcarparmset->vol_intercept_bass)*0.01f;
	float amp_high = (dampedrevs * camcarparmset->vol_gradient_mid + camcarparmset->vol_intercept_mid)*0.01f;
	float amp_top = (dampedrevs * camcarparmset->vol_gradient_top + camcarparmset->vol_intercept_top)*0.01f;

	gearchangemulttop = 1.0f - (camcarparmset->gearrevdepthtop*cameracar->gearrevdamping);
	gearchangemultlow = 1.0f - (camcarparmset->gearrevdepthlow*cameracar->gearrevdamping);


	if(amp_low>camcarparmset->vol_max_bass*0.01f)amp_low = camcarparmset->vol_max_bass*0.01f;
	if(amp_high>camcarparmset->vol_max_mid*0.01f)amp_high = camcarparmset->vol_max_mid*0.01f;
	if(amp_top>camcarparmset->vol_max_top*0.01f)amp_top = camcarparmset->vol_max_top*0.01f;


	if(low_engine->is3d)
	{
		amp_low*=1.2f;
		amp_high*=1.2f;
		amp_top*=1.2f;
	}

#define	AMPCLIP(x)	if(x<=0.0f)x=0.0f;if(x>1.0f)x=1.0f;

	float gain=1.0f;

	if(cameraview == CAM_INCAR)
		gain=GAIN_INCAR;
	else if(cameraview == CAM_BUMPER || cameraview == CAM_PITSTOP)
		gain=GAIN_BUMPER;
	gain *= VOL_INCAR * carvolume;

	internal_vol = gain;

	//dprintf("carvol %f",gain);

	vector3 diff = boris2->currentcamera->position - cameracar->mainObject->transform.translation;
	dist = diff.Mag();

	// convert dist to gain
	dist = 10/dist;
	if(dist>1)dist=1;
	gain *= dist;

	gearchangemulttop *= gain;
	gearchangemultlow *= gain;

	dampedaccel = cameracar->accelvalue*0.2f + dampedaccel*0.8f;


	amp_low *= (1.0f-camcarparmset->bassthrottle) + (camcarparmset->bassthrottle * dampedaccel);
	amp_high *= (1.0f-camcarparmset->midthrottle) + (camcarparmset->midthrottle * dampedaccel);
	amp_top *= (1.0f-camcarparmset->topthrottle) + (camcarparmset->topthrottle * dampedaccel);

	amp_top *= gearchangemulttop;
	amp_high *= gearchangemultlow;
	amp_low *= gearchangemultlow;

	AMPCLIP(amp_high);
	AMPCLIP(amp_low);
	AMPCLIP(amp_top);


	if(debugflags & SDF_BASS)amp_low = 0;
	if(debugflags & SDF_MID)amp_high = 0;
	if(debugflags & SDF_TOP)amp_top = 0;

	if(cameracar->engdead || cameracar->outoffuelct)
	{
		if(!enginefire->IsPlaying())
		{
			enginefire->Play(NULL,true,1,0,1);
		}

		amp_low=amp_high=amp_top=0;
	}
	else
	{
		if(enginefire->IsPlaying())
			enginefire->Stop();
	}

	if(Paused)amp_low=amp_high=amp_top=0;

	low_engine->SetVolume(amp_low);
	high_engine->SetVolume(amp_high);
	top_engine->SetVolume(amp_top);

extern float debug_amp_low,debug_amp_high,debug_amp_top;

	debug_amp_low = amp_low;
	debug_amp_high = amp_high;
	debug_amp_top = amp_top;


}



void CarSound::StartLoopingSound(class car *c,int id,bool onceonly)
{
	// first find a spare slot

	if(!initialised)return;
	if(onceonly) // this means only allow this sound to play once concurrently for each car.
	{
		for(int i=0;i<NUMCARSOUNDPAIRS;i++)
		{
			if(pairs[i].c == c && pairs[i].id == id)return;
		}
	}

	for(int i=0;i<NUMCARSOUNDPAIRS;i++)
	{
		if(!pairs[i].c)
		{
			// this one's free

			pairs[i].id = id;
			int chan = Start3DSound(pairs[i].id,c,true);
			if(chan>=0)
			{
				pairs[i].c = c;
				pairs[i].channel=chan;
			}
			else
			{
//				fatalError("start3Dsound failed for id %d in StartLoopingSound",pairs[i].id);
			}
			break;
		}
	}
}

void CarSound::StopLoopingSound(class car *c,int id)
{
	if(!initialised)return;
	for(int i=0;i<NUMCARSOUNDPAIRS;i++)
	{
		if( pairs[i].c && (!c || pairs[i].c == c) && (id==-1 || pairs[i].id == id))
		{
			soundsys.Stop(pairs[i].channel);
			pairs[i].c = NULL;
		}
	}
}


void CarSound::SpotEffect(class car *c,int id)
{
	if(!initialised)return;
	Start3DSound(id,c,false);
}

float CarSound::DistanceSquaredFromListener(vector3 *v)
{
	if(!initialised)return 1;	// doesn't matter what we return in this case.

	float dist = v->distanceSquared(&listener->position);

	return dist;

}

void CarSound::Non3DSpotEffect(int non3dsound,float vol,float pan,float pitch)
{
	if(!initialised)return;

	if(non3dsound>=0)fatalError("attempt to play a 3D effect as non3D %d",non3dsound);

	if(soundtablenon3d[-(non3dsound+1)])
	{
		soundsys.Play(soundtablenon3d[-(non3dsound+1)],false,vol,-1,pitch);
		Sleep(1);
		soundsys.Play(soundtablenon3d[-(non3dsound+1)],false,vol,1,pitch);
	}
	
}

void CarSound::DoSurfaceSound()
{
	int surfacetype,realsurfacetype;
	float ampbase=1.0;

	// get new surface type

	if(replaymode || NoPlayers)
	{
		surfacetype = -1;
	}
	else
	{
		float bodyY;
		triangle *bodytrianglePtr;

		if( !boris2->currentworld->getY(&cameracar->mainObject->concatenatedTransform.translation, 
			&bodyY, &bodytrianglePtr, false ) )
		{
			surfacetype = -1;
		}
		else
		{
			switch(bodytrianglePtr->mtl->bm.type)
			{
				
			case MATTYPE_KERB:
			case MATTYPE_ROUGHCONCRETE:
			case MATTYPE_MEDIUMCONCRETE:
			case MATTYPE_SMOOTHCONCRETE:
			case MATTYPE_PLANKS:
			case MATTYPE_TARMAC:
//				realsurfacetype = SURF_TARMAC;break;
				surfacetype = -1;break;
			case MATTYPE_GRAVELTRAP:
			case MATTYPE_SANDTRAP:
			case MATTYPE_DUST:
			case MATTYPE_DEEPSANDTRAP:
			case MATTYPE_COBBLES:
			case MATTYPE_GRAVEL:
			case MATTYPE_SAND:
			case MATTYPE_SANDBAGS:
				surfacetype=SURF_GRAVEL;break;
			case MATTYPE_MUD:
			case MATTYPE_ROCK:
			case MATTYPE_SHORTGRASS:
			case MATTYPE_LONGGRASS:
			case MATTYPE_ICE:
			case MATTYPE_FRESHSNOW:
			case MATTYPE_MELTEDSNOW:
			case MATTYPE_DRIFTEDSNOW:
				surfacetype = SURF_GRASS;
				break;
			default:surfacetype=SURF_TARMAC;break;
			}
		}
	}

	realsurfacetype = surfacetype;

	if(cameraview != CAM_BUMPER && cameraview != CAM_INCAR && cameraview != CAM_PITSTOP)
	{
		ampbase=0.6f;
	}

	if(lastsurfacetype != surfacetype)
	{
		if(lastsurfacetype>=0)
		{
			// switch off previous surface sounds

			if(surface_l[lastsurfacetype])
			{
				surface_l[lastsurfacetype]->Stop();
				if(surface_f[lastsurfacetype] & SURFF_STEREO)surface_r[lastsurfacetype]->Stop();
			}
		}

		if(surfacetype>=0 && surface_l[surfacetype])
		{
			if(surface_f[surfacetype] & SURFF_STEREO)
			{
				// ignore the pitches in the play code here - the pitch is actually set
				// further down.

				if(surface_l[surfacetype]->is3d)
				{
					surface_l[surfacetype]->Play3D	(NULL,true,	-1,0,0,	0,0,0,		0.0,1);
					surface_r[surfacetype]->Play3D	(NULL,true,	1,0,0,	0,0,0,		0.0,1);
					surface_l[surfacetype]->buf3d->SetMode(DS3DMODE_HEADRELATIVE,DS3D_IMMEDIATE);
					surface_r[surfacetype]->buf3d->SetMode(DS3DMODE_HEADRELATIVE,DS3D_IMMEDIATE);
				}
				else
				{

					surface_l[surfacetype]->Play(NULL,true,0.1f,-1,1);	// pan left
					surface_r[surfacetype]->Play(NULL,true,0.1f,1,1);	// pan right
				}
			}
			else
			{
				if(surface_l[surfacetype]->is3d)
					surface_l[surfacetype]->Play3D	(NULL,true,	
						cameracar->mainObject->transform.translation.x,
						cameracar->mainObject->transform.translation.y,
						cameracar->mainObject->transform.translation.z,
						0,0,0,		0.0,1);
				else
					surface_l[surfacetype]->Play(NULL,true,0.1f,0,1);	// mono : pan centre
			}
		}

		lastsurfacetype = surfacetype;
	}

	if(surfacetype>=0 && surface_l[surfacetype] && surface_l[surfacetype]->IsPlaying())
	{
		// set pitches and amplitudes

		float p,a,m;

		
		m = cameracar->velocity.IsNull()? 0 : cameracar->velocity.Mag()*2.25f;

		p = 0.7f + m/60.0f;
//		if(p>2.5)p=2.5;

		if(surface_f[surfacetype] & SURFF_LOUD)
		{
			a = (m-20.0f)/50.0f + 0.6f;
		}
		else
			a = (m-20.0f)/340.0f + 0.75f;

		a*= VOL_SURF * ampbase;

		if(a<0)a=0;
		if(a>1)a=1;

		//grdebug("M",m);
		//grdebug("P",p);
		//grdebug("A",a);

		surface_l[surfacetype]->SetFrequency(p*1.01f);
		surface_l[surfacetype]->SetVolume(a);

		if(surface_f[lastsurfacetype] & SURFF_STEREO)
		{
			surface_r[surfacetype]->SetFrequency(p*0.99f);
			surface_r[surfacetype]->SetVolume(a);
		}
	}

	// weirdly enough, we do the force feedback stuff down here too. Sorry. We use realsurfacetype to
	// determine the surface; it's independent of camera view - except when there are no players.

	if(1)
	{
#if defined(UNDER_CE)
		float mph = fabsf(cameracar->mph);
#else
		float mph = abs(cameracar->mph);
#endif
		// make it harder to turn the slower we're going
		float qq = 15000 - mph * 100;

		int joltmag = mph * 10;
		int joltchance = mph*2;
		int joltbase = 100;

//		qq*=2;
		if(qq>10000)qq=10000;

		if(cameracar->numskids >0)
		{
			joltchance *= 4;
			joltmag *= 10;
			qq=0;
		}

		switch(realsurfacetype)
		{
		case SURF_TARMAC:
			break;
		case SURF_GRAVEL:
			joltbase=3000;
			joltmag*=8;
			joltchance*=3;
			break;
		case SURF_GRASS:
			joltbase=3000;
			joltchance*=4;
			qq*=0.5f;
			joltmag*=10;
			break;
		default:break;
		}

		if((rand()%10000) < joltchance)
		{
			Jolt(EFF_HARD,(rand()+joltbase) % joltmag,0.01f,(rand()%2) ? -90 : 90);
		}

		static int gloop=0;
		if(gloop++ == 60)
		{
			gloop=0;
			SetForceFeedback(qq,qq,0,cameracar->revs);
		}
	}
}
	

void CarSound::Update()
{
	// update the listener and car pos/vel/heading data

	// listener


	car *c;
	vector3 vel,pos;
	int i;

	// now set the positions of the sound sources

	if(!initialised)return;
	DoSurfaceSound();

//	dprintf("sound update");

	NominateListener(boris2->currentcamera);
	soundsys.ScanDupsAndDeleteFinished();

	ProcessAmbients();

//	dprintf("looping for positions");

	for(i=0;i<numcars;i++)
	{
		c = cararray[i];

		vector3 qqq = c->mainObject->transform.translation -
			listener->position;

		comp_engine[i]->SetPosition(
			c->mainObject->transform.translation.x,	
			0,
			c->mainObject->transform.translation.z);

		vel = c->velocity;
		comp_engine[i]->SetVelocity(vel.x,vel.y,vel.z);
	}

//	dprintf("setting listener");

	if(listener->attached)
	{
		// UGLY!
//		dprintf("attached");

		for(i=0;i<numcars;i++)
		{
			if(cararray[i]->mainObject == listener->attached)
			{
				// assumed the attached object is a car
				vel = cararray[i]->velocity;
				pos = cararray[i]->mainObject->transform.translation;
//				pos += listener->direction*1.5;	// offset the listener by a bit
				break;
			}
		}
	}
	else
	{
//		dprintf("not attached");
		vel=vector3(0,0,0);
		pos=listener->position;
	}

//	dprintf("position obtained, setting");
	soundsys.SetListener(pos.x,pos.y,pos.z,vel.x,vel.y,vel.z,
		0,1,0,
		listener->direction.x,
		0,
		listener->direction.z);

	UpdateEngineSounds();

	for(i=0;i<numcars;i++)
	{
		if(cararray[i]!=cameracar && cararray[i]->numskids>0)
		{
			StartLoopingSound(cararray[i],SND3D_SKID,true);
		}
		else
		{
			StopLoopingSound(cararray[i],SND3D_SKID);
		}
	}

	for(i=0;i<4;i++)
	{
		if(cameracar->skidding[i])
		{
			skidvol[i]+=0.1f;
			if(skidvol[i]>1)skidvol[i]=1;
		}
		else
		{
			skidvol[i]-=0.1f;
			if(skidvol[i]<0)skidvol[i]=0;
		}

		if(skidvol[i]>0)
		{
			if(!skid_sound[i]->IsPlaying())
			{
				skid_sound[i]->Play(NULL,true,skidvol[i],(i%2)?0.8f:-0.8f,(i<2)?1.1f : 0.9f);
			}
			skid_sound[i]->SetVolume(skidvol[i]);
		}
		else
		{
			if(skid_sound[i]->IsPlaying())
			{
				skid_sound[i]->Stop();
			}
		}
	}



	// change the reverb setting

	if(!replaymode)
		SetReverb((cameracar->reverblo_on ? 1 : 0) + (cameracar->reverbhi_on ? 2 : 0));
}


// ambient sound

float __inline randpitch()
{
	float q = (rand()%1000);
	q/=500;	// 0-2
	q-=1;	// -1 to 1
	q/=10;	// -0.1 to 0.1
	q+=1;	// 0.9 to 1.1
	return q;
}

struct ambstruct
{
	float x,z,vol;
	char name[32];
	Sound *rootsnd;
	LPDIRECTSOUNDBUFFER buf;		// nonzero if playing
	LPDIRECTSOUND3DBUFFER buf3d;

};

struct ambstruct ambients[64];


void CarSound::CreateNewAmbient(float x,float z,char *name,float vol)
{
	if(ambcnt<64)
	{
		dprintf("attempting to set up ambient %d at %f %f - %s - vol %f",ambcnt,x,z,name,vol);

		if(ambients[ambcnt].rootsnd = soundsys.FindOrLoadSound(name,false,true,1))
		{
			ambients[ambcnt].x = x;
			ambients[ambcnt].z = z;
			strcpy(ambients[ambcnt].name,name);
			ambients[ambcnt].buf=NULL;
			ambients[ambcnt++].vol =vol;
		}
	}
}
	
void CarSound::ProcessAmbients()
{
	HRESULT rv;

	if(!initialised)return;

	for(int i=0;i<ambcnt;i++)
	{
		struct ambstruct *a = ambients+i;
		bool inrange;

		// determine if in range

		float dx = listener->position.x - a->x;
		float dz = listener->position.z - a->z;

		inrange = (dx*dx + dz*dz < 200*200);

		if(Paused)inrange=0;

		if(inrange && !a->buf)
		{
			// in range but not playing; dup and start it
			a->buf = a->rootsnd->Duplicate();

			if(!a->buf)
				dprintf("error in duplicate");
			else
			{
				rv=a->buf->QueryInterface(IID_IDirectSound3DBuffer,(LPVOID *)&(a->buf3d));

				if(FAILED(rv))
					soundsys.DSError("processambients queryint",rv);
				else
				{

					rv=a->buf3d->SetPosition(a->x,listener->position.y,a->z,DS3D_IMMEDIATE);
					if(FAILED(rv))soundsys.DSError("amb - setpos",rv);
					rv=a->buf3d->SetVelocity(0,0,0,DS3D_IMMEDIATE);
					if(FAILED(rv))soundsys.DSError("amb - setvel",rv);
					rv=a->buf->SetVolume(0);
					if(FAILED(rv))soundsys.DSError("amb - setvol",rv);
					
					rv=a->buf3d->SetMinDistance(10.0,DS3D_DEFERRED);
					rv=a->buf3d->SetMaxDistance(1000.0,DS3D_DEFERRED);

					rv=a->buf->Play(0,0,DSBPLAY_LOOPING);
					if(FAILED(rv))soundsys.DSError("amb - play",rv);
				}
			}
		}
		if(!inrange && a->buf)
		{
			// out of range and playing - stop it and release it

			a->buf->Stop();
			a->buf3d->Release();
			a->buf->Release();
			a->buf=NULL;
		}
	}
}

void CarSound::LoadAmbient()
{
	ParamFile p;
	if(!initialised)return;

	if(!p.ReadFromFile("ambient.txt"))
	{
		if(!p.Read("ambient"))
			return;
	}

	dprintf("ambient file read");

	char *s;

	/* format of ambients file:

			num:	10
			x0:		543.2
			z0:		182.0
			[vol0:	1]	(optional)
			samp0:	crowd.wav

			...
	*/

	if(s=p.Find("num"))
	{
		int num=atoi(s);
		for(int i=0;i<num;i++)
		{
			float x,z,vol;

			char buf[32];
			sprintf(buf,"x%d",i);
			if(!(s=p.Find(buf)))fatalError("cannot get ambient parameter %s",buf);
			sscanf(s,"%f",&x);
			sprintf(buf,"z%d",i);
			if(!(s=p.Find(buf)))fatalError("cannot get ambient parameter %s",buf);
			sscanf(s,"%f",&z);
			sprintf(buf,"vol%d",i);
			if(!(s=p.Find(buf)))vol=1;else 	sscanf(s,"%f",&vol);

			sprintf(buf,"samp%d",i);
			if(!(s=p.Find(buf)))fatalError("cannot get ambient parameter %s",buf);

			CreateNewAmbient(x,z,s,vol);
		}
	}
}


void PlayFrontEndSound(int n)
{
	static int sndsidx[] =
		{SND_HIGHLIGHT,0,SND_MENU,SND_CLICK};

	carsounds.Non3DSpotEffect(sndsidx[n]);
}

void CarSound::SetParameters(car *c)
{
	camcarparmset = params+cameracar->id;
}




























/* debugging cod */



void CarSound::Dump()
{
	if(!initialised)return;
	soundsys.Dump();
}

void CarSound::DrawGraph(float intercept,float gradient,float maximum,float ypos,DWORD col)
{
//	boris2->DrawLine(600,ypos,700,ypos+100,0xffffff);
}

void CarSound::ParamEditor(int mode)
{


	// WILL NO LONGER WORK



	float inc = 0.01f;

	switch(mode)
	{
	case CPEM_DRAW:
		overlay.DrawDebugWidget(600,20,300,paramnames[curparm],*paramptrs[curparm],parammins[curparm],
			parammaxs[curparm]);
		overlay.DrawDebugWidget(550,100,100,"gcmlow",gearchangemultlow,0,1,0);
		overlay.DrawDebugWidget(550,400,100,"gcmtop",gearchangemulttop,0,1,0);
		// draw graphs

		DrawGraph(camcarparmset->vol_intercept_bass,camcarparmset->vol_gradient_bass,camcarparmset->vol_max_bass,350,
			(paramgraphs[curparm]==GRAPH_BASS)?0xffffff:0x80ff80);
		DrawGraph(camcarparmset->vol_intercept_mid,camcarparmset->vol_gradient_mid,camcarparmset->vol_max_mid,400,
			(paramgraphs[curparm]==GRAPH_MID)?0xffffff:0x80ff80);
		DrawGraph(camcarparmset->vol_intercept_top,camcarparmset->vol_gradient_top,camcarparmset->vol_max_top,450,
			(paramgraphs[curparm]==GRAPH_TOP)?0xffffff:0x80ff80);

		break;
	case CPEM_DEC:
		inc = -0.01f;
	case CPEM_INC:
		inc *= parammaxs[curparm]-parammins[curparm];
		*paramptrs[curparm]+=inc;
		dprintf("incrementing %s by %f",paramnames[curparm],inc);
		break;
	case CPEM_PREV:
		curparm--;
		if(curparm<0)curparm=ppctr-1;
		break;
	case CPEM_NEXT:
		curparm++;
		if(curparm==ppctr)curparm=0;
		break;
	case CPEM_DUMP:
		FILE *a;
		if(a=fopen("c:\\carsound.txt","w"))
		{
			for(int i=0;i<rppctr;i++)
			{
				fprintf(a,"%s	:	%f\n",paramnames[i],*paramptrs[i]);
			}
			fclose(a);
		}
	default:break;

	}
}
#define  REGISTRY_DIRECTORY	"Software\\Hasbro Interactive\\Spirit of Speed 1937"


void check_init_carsounds()
{
	char buf[64];
	if(!carsounds.initialised)
	{
#if 0
		GetRegistryValue(REGISTRY_DIRECTORY,"Sounds_VolumeSwitch",buf);
		if(stricmp(buf,"Off"))
		{
			GetRegistryValue(REGISTRY_DIRECTORY,"EAX",buf);
			carsounds.Initialise(GetActiveWindow(),stricmp(buf,"on")?false:true);
		}
#endif
	}
}
