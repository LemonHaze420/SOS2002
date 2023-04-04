/*
	$Header$
	$Log$
	Revision 1.0  2002-07-30 10:19:46+01  img
	Initial revision

	Revision 1.0  2000-02-21 12:34:15+00  jjs
	Initial revision

	Revision 1.5  1999/03/23 10:59:44  jcf
	Start of week checkin

	Revision 1.4  1999/03/17 17:01:52  jcf
	First working threaded version

	Revision 1.3  1999/03/16 17:14:31  jcf
	<>

	Revision 1.2  1999/01/25 17:27:17  jcf
	general check-in after minor mods

	Revision 1.1  1998/12/15 17:51:14  jcf
	Initial revision

*/

#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "AI_Spline.h"

class ctrlpckt {

public:
	char steer;		// MINSHORT to MAXSHORT 
	char accbrake;		// as above, but -ve is braking and +ve is acceleration
	short flags;		// should always be last. See below for values

	void zero()
	{
		steer = accbrake = 0;
		flags = 0;
	}

	ctrlpckt() { zero() ; }

// conversion functions to/from floats

	void MakePacketData(float a,float b,float s)
	{
		s*=127;
		steer = (char)s;

		a = a*127 - b*127;
		accbrake=(char)a;
	}

	void GetPacketData(float *fa,float *fb,float *fs)
	{
		float a,b,s;

		if(accbrake<0)
		{
			a=0;
			b= -(float)accbrake;
		}
		else
		{
			a=(float)accbrake;
			b=0;
		}
		s=steer;
		
		*fa = a/(float)127.0;
		*fb = b/(float)127.0;
		*fs = s/(float)127.0;
	}

};

#define	CF_GEARUP		1
#define	CF_GEARDN		2
#define	CF_SCHARGE		4
#define	CF_REARGLANCE	8

#define	CF_FINISH		16	// used by replay controller to indicate end of stream

// controller codes (keys are their DIK_ values)

#define	CONC_XAXIS	-1
#define	CONC_YAXIS	-2
#define	CONC_1	0xf0
#define	CONC_2	0xf1
#define	CONC_3	0xf2
#define	CONC_4	0xf3
#define	CONC_5	0xf4
#define	CONC_6	0xf5
#define	CONC_7	0xf6
#define	CONC_8	0xf7
#define	CONC_NONE	1000

// controller functions

#define	CON_THROTTLE	0
#define	CON_BRAKE		1
#define	CON_LEFT		2
#define	CON_RIGHT		3
#define	CON_GEARUP		4
#define	CON_GEARDN		5
#define	CON_LESS	6
#define	CON_MORE	7
#define	CON_GLANCE	8

int GetAControl(char *name);



///// replay gubbins


extern unsigned long replaymemoryused;

#define	REPF_BEGIN		1	// obsolete
#define	REPF_POSITION	2
#define	REPF_YROTANDVEL	4
#define	REPF_BANG		8	// engine exploded
#define	REPF_PHYSDATA	16	// packet also contains physics data

#define	NUMBUFS	12

// a replay buffer (more correctly, a replay stream) is a growable block of memory
// containing blocks of data of variable length, in the following format:
//	flags, indicating block type
//	control packet
//	yrotation, angular velocity and vector velocity block if REPF_YROTANDVEL set
//	complete position block if REPF_POSITION set
//  physics data (oil, fuel, tyre states) if REPF_PHYSDATA set


struct yrotblock
{
	vector3 vel;
	float yRot,angvel,revs;
};

struct positionblock
{
	matrix3x4 maintransform;
};

struct replayphysblock
{
	float fuel,oil,tyresfront,tyresback;
};

#define	REPLAYBLOCKSIZE	32768

struct replaylist
{
	replaylist()
	{
		data=NULL;
	}
	~replaylist()
	{
		if(data)free(data);
	}

	char *data;
	struct replaylist *next;
};

class replay
{
public:

	struct replaylist *head,*tail;
	unsigned long sizeoftail;	// since all the other blocks will be full.
	unsigned long numblks;
	struct replaylist *blk;unsigned long idx;	// used by read

	replay()
	{
		blk=head=tail=NULL;
		idx=0;
		numblks=0;
	}

	~replay()
	{
		struct replaylist *p,*q;
		for(p=head;p;p=q)
		{
			q=p->next;
			delete p;
		}
	}

	void Load(HFILE h);
	void Save(HFILE h);

	replaylist *addnewblock()
	{
		replaylist *p = new replaylist;
		if(!head)
		{
			head=tail=p;
		}
		else
		{
			tail->next = p;
			tail = p;
		}
		p->data = (char *)malloc(REPLAYBLOCKSIZE);
		replaymemoryused+=REPLAYBLOCKSIZE;
		p->next=NULL;
		sizeoftail=0;
		numblks++;
		return p;
	}

	void add(void *block,unsigned long size);

	bool read(void  *data,unsigned long size);
};

class ReplayBufferManager
{
public:
	replay *bufs[NUMBUFS];

	void Shutdown()
	{
		for(int i=0;i<NUMBUFS;i++)
		{
			if(bufs[i])
			{
				delete bufs[i];
				bufs[i]=NULL;
			}
		}
	}

	void Initialise() 
	{
		replaymemoryused=0L;

		Shutdown();

		for(int i=0;i<NUMBUFS;i++)
			bufs[i]=new replay;
	}

	ReplayBufferManager() { for(int i=0;i<NUMBUFS;i++) bufs[i]=NULL; }

	~ReplayBufferManager() { Shutdown(); }

};



extern ReplayBufferManager replaybuffermanager;

/****Thu Dec 10 14:43:34 1998*************(C)*Broadsword*Interactive****
 *
 * Controller, control packet and replay buffer stuff
 *
 ***********************************************************************/

typedef enum { CONTROLLER_LOCAL,CONTROLLER_REPLAY,CONTROLLER_NETWORK,CONTROLLER_AI} ControllerType;

class Controller
{
protected:
	class car *car;
	class replay *replaybuffer;
	long replaycount,replaysize;

	int gearupbutton,geardownbutton;
	char wheel_curve[256];	// steering wheel curve
	ctrlpckt packet;	// local packet storage
	char pitkeys;
public:

	Controller();

	ControllerType type;

	void Attach(class car *c){	
		c->controller = this;
		car = c;
	}

	void InhibitGears()
	{
		// removes gear flags from the packet. Must be done
		// after BuildControlPacket, AI code etc. and before the physics.

		packet.flags &= ~(CF_GEARUP | CF_GEARDN);
	}

	virtual ctrlpckt GetControlPacket() = 0;	// vitual pure, must be overridden
	virtual void BuildControlPacket() { };	// call from main loop

	virtual void SetReplayBuffer(class replay *r) { replaybuffer=r; }
	virtual void Record(ctrlpckt &p);
	virtual void Save(HFILE h);	// saves the replay buffer

	virtual void SetPitKeys(char p)
	{
		pitkeys=p;
	}
};

class NetController : public Controller
{
public:
	NetController();
	virtual ctrlpckt GetControlPacket();
};



bool DoesReplayExist();
void SaveReplayControllers(int numcars);
void LoadReplayHeader(struct gamesetup *g);
int LoadReplayControllers(int numcars);


class ReplayController : public Controller
{
private:
	int ct;
public:
	int id;
	ReplayController();
	virtual ctrlpckt GetControlPacket();
	void Load(HFILE h);

	// these are stubs, they do nothing in replay controllers

	virtual void Save(HFILE h) { }
	virtual void Record(){}

	virtual void SetPitKeys(char p) { ; }

	char GetPitKeys() { return pitkeys; }
};



void InitialiseDirectInput(void *instance,int ff);	// call this at the start of the app
void ShutdownDirectInput();	// call this at the start of the app

class LocalController : public Controller
{
	friend void ProcessJoystick(LocalController *con,bool,bool);
private:
	int gearchange;
	float throttle,brake,turn;
	float dampedturn;
	bool gearupoldstate,geardownoldstate;
	bool glancestate;
	char *keybuffer;

protected:
	unsigned long lastspinctr;

public:
	bool leftstate,rightstate,gearupstate,geardownstate,throttlestate,brakestate,shiftstate,ctrlstate;
	bool automatic,analogthrottle;
	bool automatic_gearup;
	virtual void BuildControlPacket();	// call from main loop
	virtual ctrlpckt GetControlPacket(); // call from thread
	void SendControlPacket(ctrlpckt packet);
	LocalController(char *keybuf=NULL);	// this is the buffer polled from inside the main loop, with key states in it
	bool GetBooleanControlVal(int code);
};



class AIController : public LocalController 
{
	// Just a temporary thing to make it more interesting.
	float SpeedModifier;
	float LastSpeed;
	char PreviousSteer;
	char HoldZero;
	char HoldSteer;
	char LimitAccel;
	char LimitBrake;
	char SkidDirection;
	float SigmaError;
	float TickError;
	float LastAngle;
	bool Oscillating;
	int LastDirection;
	int Tester;
	int LastRLP;
	ctrlpckt LastPacket;
	float LastRevs;
	
	// Ian's AI now. I`ll leave the stuff below in for compatibility but chances are it'll be code-icided before long.
	AI_SplineInterfaceID carSplineInterface;
	object *debugTyreObject[10];

	//****************************************************************************************************************************
	// SDS - Seans stuff for new style aI, probably get rid of the stuff above
	//****************************************************************************************************************************

	float TurningSpeed; // Limits AI turniong ablilty
	float Gearing;	    // Limits AI gearchanging ability
	float Speed;		// Limts AI top speed
	float Avoidence;	// Avoidence look ahead
	float SteerAhead;	// Steering Look ahead
	float SClut[1000];  // spline mod Lookup table
	float LDT;			// Differential for average turn sum
	float AvgSpeed;		// AI cars average speed
	int TCounter;		// Tick Counter
	int Abs;
	int PState;
	int PARcount;
	int SlowEmDown;
	int MyPoint;		// Store the current point
	int FreezeDir;		// Counter to freeze the direction
	int Breaking;		// Break tick counter
	int ParkTick;
	int BrakeHelpCount;
	int BrakeHelpPointer;
	vector3 TargetVelocity;
	float MaxSpeed;		// Limit max speed
	float TurnBrake;		// Breaking Turn look ahead
	float TLS;			// Turn Look ahead Sensitivity
	float SLHD;
	float TLBD;			// Turn Look ahead Brake Divide
	int	  BSS;			// Brake system subtract
	float lastTD;
	float OTC;
	int ALP;			// Absolute Last Point
	float RRS;
	float BrakeOverride; // Helpers for hard corners
	int BrakeHelpFix; 
	char *FirstTime;
	int TurnCount;
	float TurnOffset;
	float LLTD;
	float BrakePointMult;
	int   tul;
	int	  LastPoint;
	int	  PitCounter;
	float LastVelocity;
	int   overtake;
	float LastOffset;
	float carwidth;
	int   LastTrackPoint;
	int	  LastCarIndex;
	int	  lastcardir;
	float CarSkidVelocity;
	// Pitstop vars
	int pitstart;    // Start trigger point
	int pitend;	     // end trigger point
	int pitcount;    // how many points in pit race line
	int pitpos;	     // actual position of pitstop in pit race line
	int pitcurrent;  // pointer into pit stop race line
	int pitsub;		 // pitstop sub mode
	int pitstaytime; // How long to stay in the pits
	int pitslowdown; //
	int brookhack;
	float CatchUpMult; // Catch Up Code multiplier
	int	NameKey; // Used for Various rand 
	float StartOffset;
	int StartMod;
	int LastLap;
	int FuelPerLap;
	int WheelCount;
	vector3 LastPitPos;
public:
	int difficulty;
	int State;
	AIController() ;//: LocalController() {};
	virtual ctrlpckt GetControlPacket();
};




#endif