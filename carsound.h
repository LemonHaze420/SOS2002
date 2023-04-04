/*
	$Header$
	$Log$
	Revision 1.0  2000-02-21 12:34:12+00  jjs
	Initial revision

	Revision 1.1  1999/03/29 09:23:49  jcf
	Initial revision



*/


// sound library encapsulation

#define	MAXSOUNDS	32	// number of sounds of each type permitted to be loaded
#define	NUMCARSOUNDPAIRS	32


// these sounds are defined in carsound.cpp's tables

#define	SND3D_SKID		3

#define	SND_GEARCHANGE	-1	// non3d sounds are distinguised by -ve ids

#define	SND_SAY0		-2
#define	SND_SAY1		-3
#define	SND_SAY2		-4
#define	SND_SAY3		-5
#define	SND_SAY4		-6
#define	SND_SAY5		-7

#define	SND_BOING		-8

#define	SND_GEARCHANGEUPREV	-9
#define	SND_GEARCHANGEUPNEUT	-10
#define	SND_GEARCHANGEUP1	-11
#define	SND_GEARCHANGEUP2	-12
#define	SND_GEARCHANGEUP3	-13
#define	SND_GEARCHANGEUP4	-14

#define	SND_GEARCHANGEDNNEUT	-15
#define	SND_GEARCHANGEDN1	-16
#define	SND_GEARCHANGEDN2	-17
#define	SND_GEARCHANGEDN3	-18
#define	SND_GEARCHANGEDN4	-19
#define	SND_GEARCHANGEDN5	-20

#define	SND_CLICK	-21
#define	SND_HIGHLIGHT	-22
#define	SND_MENU	-23
#define	SND_COLLISION1	-24
#define	SND_COLLISION2	-25
#define	SND_COLLISION3	-26
#define	SND_EXPLODE	-27


// structure used to keep track of what car is playing which sounds

struct carsoundpair { 
	class car *c;
	int id;

	int channel;
};

// surface types
#define	SURF_TARMAC	0
#define	SURF_GRAVEL	1
#define	SURF_GRASS	3

// surface sound flags
#define	SURFF_STEREO	1
#define	SURFF_LOUD	2

// debug flags
#define	SDF_BASS	1
#define	SDF_MID		2
#define	SDF_TOP		4

#define	NUMCARSOUNDSLOTS	3

struct CarSoundParams
{
	float gearrevdampingdelay,gearrevdampingstep,gearrevdepthtop,gearrevdepthlow;

	float pitch_gradient_bass,pitch_gradient_mid,pitch_gradient_top;
	float vol_gradient_bass,vol_gradient_mid,vol_gradient_top;

	float pitch_intercept_bass,pitch_intercept_mid,pitch_intercept_top;
	float vol_intercept_bass,vol_intercept_mid,vol_intercept_top;
	float pitch_intercept_external,pitch_gradient_external;

	float bassthrottle,midthrottle,topthrottle;

	float revdamping,exhaustgrad,exhaustint,exhaustvolincar,exhaustvoloutcar,
		exhaustfalloff,exhaustrevdiff;
	float vol_max_bass,vol_max_mid,vol_max_top;
	float pitch_min_bass,pitch_min_mid,pitch_min_top;

	float backfirethresh;
};

class CarSound
{
public:

	float carvolume;

	class car *slots[NUMCARSOUNDSLOTS];
	float skidvol[4];

	class camera *listener;	// the car that's currently listening

	// sounds we are going to use. These are loaded either from the static table in carsound.cpp
	// or when the car is constructed using LoadSound.

	class Sound *soundtable3d[MAXSOUNDS];	
	class Sound *soundtablenon3d[MAXSOUNDS];

	class Sound *enginefire;

	carsoundpair pairs[NUMCARSOUNDPAIRS];

	// these are the composite engine sounds. We use these for external cars.
	class Sound *comp_engine[12];
	class Sound *skid_sound[4];

	// exhaust sounds
	class Sound *exhaust_sound[20];
	class Sound *backfire_sound[4];

	// these are the banded engine sounds. We use these for the player cars
	class Sound *low_engine,*high_engine,*top_engine;

	// generic surface sound
	class Sound *surface_l[4],*surface_r[4];
	unsigned long surface_f[4];

	bool load_internal_as_3D;	// decides whether the player's car sounds are loaded as 3D
	class car *lastcar;
	int lastsurfacetype;
	bool isplayinginternal;
	float prevrevs,dampedaccel,dampedrevs;

	int numcars;	// OK, so this is duplication but I'm in a hurry
	bool initialised;
	unsigned long debugflags;

	// parameters from carsound.txt


	CarSoundParams params[16],*camcarparmset;
	void LoadParameterSet(int n,char *carname);
	void SetParameters(car *c);

	void ProcessAmbients();


#define	NUM_PARAMS	64

	// end of parameters

	char *paramnames[NUM_PARAMS];
	float *paramptrs[NUM_PARAMS];
	float parammins[NUM_PARAMS];
	float parammaxs[NUM_PARAMS];
	int paramgraphs[NUM_PARAMS];
	int ppctr,rppctr;

#define	GRAPH_BASS	0
#define	GRAPH_MID	1
#define	GRAPH_TOP	2

#define	CPEM_DRAW	0
#define	CPEM_INC	1
#define	CPEM_DEC	2
#define	CPEM_NEXT	3
#define	CPEM_PREV	4
#define	CPEM_DUMP	5

	int curparm;
	void DrawGraph(float intercept,float gradient,float maximum,float ypos,DWORD col);
	void ParamEditor(int mode);


	class car *exhaust_car;
	float integral_revdiff;

	void DoSurfaceSound();
	void Initialise(HWND main_hwnd,bool use_eax);
	void Shutdown();
	int LoadSound(char *filename, bool is3d,int pri); // takes an abbreviated file name; no extension or path
	void StopAll();
	int Start3DSound(int id,car *carptr,bool loop);

	void NominateListener(class camera *c);

	void Update(); // call once a frame - updates the positions and plays waiting sounds.

	void LoadEngineSounds(int numcars,class car **carptr);
	void UpdateEngineSounds();

	void StartLoopingSound(class car *c,int sound,bool onceonly=false); 
	// id -1 will stop all sounds on that car, car NULL will stop all instances of that sound
	void StopLoopingSound(class car *c,int sound); 
	void SpotEffect(class car *c,int sound);

	void Non3DSpotEffect(int non3dsound,float vol=1.0,float pan=1.0,float pitch=1.0);
	void LoadAmbient();
	void CreateNewAmbient(float x,float z,char *name,float vol);

	void Dump();

	float DistanceSquaredFromListener(vector3 *v);

	CarSound()
	{
		;
	}

	void debug(unsigned long n)	// toggles debug flags
	{
		debugflags ^= n;
	}

	// NOTE that the thread is actually started in Initialise and
	// ended by Shutdown; all these routines do is set and reset the
	// exhaust car.

	void SetExhaustCar(class car *c)
	{
		exhaust_car = c;
	}

	void ResetExhaustCar()
	{
		exhaust_car = NULL;
	}

	void SetReverb(int revtype);
	void ResetEngineSounds();

	bool replaymode;

};


extern CarSound carsounds;