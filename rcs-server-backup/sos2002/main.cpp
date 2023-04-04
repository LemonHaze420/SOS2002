// $Header$

// $Log$
// Revision 1.2  2002-08-07 15:39:58+01  img
// Quick bug fix version for the sky.
//
// Revision 1.1  2002-07-22 10:06:35+01  jjs
// Test version of SOS2002
//
// Revision 1.0  2000-02-21 12:34:18+00  jjs
// Initial revision
//


#if !defined(UNDER_CE)
static char *gnort = "8234923743849759238741297523849shortybigeyesinflatableaardvark";
#endif
#define	DEMO	0
#define	ALLOWTABBING	0

char *BuildDate =
#if DEMO

"Packaging and Manual - © 1999 Hasbro Interactive, Inc.  All rights reserved.~"
"Game Code and Artwork - © 1999 Broadsword Interactive Limited.  All rights reserved.~"
"PC version published under exclusive licence from Broadsword Interactive Limited.";

#else
#if 0
"V 1.1.2~";
#else
"MATROX";
#endif
#endif

#define UseZbackDetail
#define FRAMEDUMP 0

#if !defined(UNDER_CE)
static const char *rcs_id="$Id$";
#endif
/****Mon Mar 15 12:08:44 1999*************(C)*Broadsword*Interactive****
 *
 * This is a rewrite of testmain.cpp - it contains the main framework
 * code for the Spirit of Speed application.
 *
 ***********************************************************************/


char password[20];	

 
#define STRICT


#define  PROGRAM_NAME    TEXT("Spirit of Speed")
#define  WINDOW_TITLE    TEXT("Spirit of Speed")
#define  REGISTRY_DIRECTORY	"Software\\Hasbro Interactive\\Spirit of Speed 1937"

#define	USE_FRONT_END	0
#define	WINDOW_MODE	1

// these two are mutually exclusive
#define	DEBUGCAMS	1
#define	CARSOUNDPARAMEDITOR	0

// SHOW_DEMO defined in SOS_Data.h

#define	USEVOODOO	FALSE
#define	WINDOWHACK	FALSE
#define	COUNTDOWN	7
#define	MOVABLECARCAM	0
#define	NOSOUND	1

#define	XRES	800
#define	YRES	600
#define FRONTEND_XRES 800
#define FRONTEND_YRES 600
#define FRONTEND_DEPTH 16


bool grdebugstate=false;		
bool carinfo_on,load_shadows;

//include for frames per second timer
//#include <sys/types.h>
//#include <sys/timeb.h>
//-----------------------------------


#include <windows.h>
#include <stdio.h>
#if !defined(UNDER_CE)
#include <process.h>
#include <new.h>
#include <crtdbg.h>
#include <time.h>
#endif
#include <stdlib.h>
#define DIRECTINPUT_VERSION 0x0700
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
//#include "gamedata.h"
#include "frontend.h"
//#include "reg.h"
#include "Pitstop.h"
//#include "FrontLib2.h"
#include "sound.h"

#include "force.h"
#include "skids.h"
void dprintf(char *,...);
bool InitialiseInputs();

//int FrontEnd(HWND MainWindow);
class FrontEndClass *MainFrontEnd=NULL;
//extern SOSData		MySOSData;
HWND main_hwnd;

bool bActive = false;
int numMirrors=1;
int resX;
int mirrorx[2],mirrory[2];
int mirrorw[2],mirrorh[2];
int ResolutionX, ResolutionY;
#if FRAMEDUMP
bool DumpFrames = false;
int DumpCount = 0;
static char BmpHeader[]={
0x42,0x4D,0x36,0xF9,0x15,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,
0x00,0x00,0x20,0x03,0x00,0x00,0x58,0x02,0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00,
0x00,0x00,0x00,0xF9,0x15,0x00,0x12,0x0B,0x00,0x00,0x12,0x0B,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};
#endif
int detaillevel = 1;
int polysperframe,texswaps,vbufs;

car * cararray[NUMCARS],*playercar,*cameracar;
gamesetup *game;
bool moved = false;
bool practice_mode = false;
bool exit_on_outoffuel=false;
void PlayMovie(char *filename, int xpos, int ypos, int xres, int yres);
#define dprintf(x)

/*
	cameraview is the camera view as drawn, realcameraview is 
	what the user interface thinks it is. These are normally the same except when
	realcameraview is CAM_INCAR and the car is not the player car, in which case
	cameraview is set to CAM_OUT1
*/

int cameraview = CAM_OUT1,realcameraview=CAM_OUT1;
bool detachmode=false;
bool lagmode=false;
extern bool PlayingMovie;

display * boris2display = 0;
camera  * boris2camera = 0;
display * boris2mdisplay = 0;
camera	* boris2mirror = 0;
display * boris2mdisplay2 = 0;
camera	* boris2mirror2 = 0;
world   * boris2world = 0;
timer   * boris2timer;
sky     * boris2sky;

float zback=1700.0;	// these come from the track config file
font * testFont;

float camposy=0.18f;
float camposz=0.04f;
float camposzModifierInCar=0.0f;
int jjshack99=0;
bool do_exit_hack=false;
int ticksthisframe=0;

CameraBank *AllCameras;
camera *FloatingCamera;
bool UseFloatingCamera;
FILE *CameraLocations;

PitstopGraphics *AllPitstopGraphics;
struct raceglobalstruct raceglobals;

bool Paused=false;
struct gamesetup GlobalGame;
bool NoPlayers;
bool PaperFlag;

LPARAM heldlParam;
bool HandleKey(WPARAM key);
char resString[10]="";
extern bool StopMovie;
//Random start time
int startcount;
/*
FILE *RecordingFile;
FILE *PlaybackFile;
*/
long FAR PASCAL WindowProc( HWND hWnd, UINT message, 
						   WPARAM wParam, LPARAM lParam )
{
	
	switch( message )
	{
#if !defined(UNDER_CE)
	case WM_NCHITTEST:
			return HTCLIENT;
#endif
#if !ALLOWTABBING
#if !defined(UNDER_CE)
	case WM_POWERBROADCAST:
			return BROADCAST_QUERY_DENY;
        case WM_SYSCOMMAND:
            // Prevent moving/sizing and power loss in fullscreen mode
            switch( wParam )
            {
                case SC_MOVE:
                case SC_SIZE:
                case SC_MAXIMIZE:
                case SC_MONITORPOWER:
                    return 0;
            }
            break;
#endif 
#endif
#if !defined(UNDER_CE)
	case WM_ACTIVATEAPP:
		bActive = (wParam ? true : false);
		break;
#endif
	case WM_CREATE:
		break;
	case WM_TIMER:
		break;
	case WM_SETCURSOR:
		SetCursor( NULL );
		break;
	case WM_MOVE:		// window moved  ...  hold the new position and set a flag so we can resize our window
		heldlParam = lParam;				  // Renderer requires this format
		moved = true;
		break; 

	case WM_LBUTTONDOWN:
#if 0
		StopMovie = true;
#endif
	case WM_KEYDOWN:
		
		switch( wParam )
		{		
			case VK_ESCAPE:
#if 0
				StopMovie = true;
#endif
				// engine_deinitialise();           
#if !USE_FRONT_END
			if(!(game->flags & GSF_REPLAY))
				SaveReplayControllers(game->numcars);
			DestroyWindow( hWnd );
			dprintf("WinMain: removing texture store");
			removeTextureStore();
			dprintf("WinMain: removing model store");
			removeModelStore();
//			removeModelStore7();
			dprintf("deleteFX");
			DeleteFX();
			dprintf("delete boris");
			delete boris2;
			dprintf("deinitialise render-3D");
#if !defined(UNDER_CE)
			renderDeInitialise(CLOSE_ALL);
#endif
			dprintf("3D shutdown OK");
			return 0;

//			case VK_ESCAPE:
//			do_exit_hack=true;
//			return 0;

#endif
			default:
#if 0
				if(!PlayingMovie)
#endif
					if(HandleKey(wParam))
						return 0;
						
		};
		break;
	case WM_DESTROY:
		

		PostQuitMessage( 0 );
		break;
     }
		
		
	return DefWindowProc( hWnd, message, wParam, lParam );
}

//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------

bool make_window( HINSTANCE hInstance, int nCmdShow )
{
	
	WNDCLASS main_wc;

	memset(&main_wc,0,sizeof(WNDCLASS));

	/*
* set up and register window class
	*/
    
    main_wc.lpfnWndProc = WindowProc;
    main_wc.hInstance = hInstance;
#if !defined(UNDER_CE)
    main_wc.hIcon = LoadIcon(hInstance,"icon1.ico");       
   main_wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	main_wc.lpszMenuName = PROGRAM_NAME;
#endif
    main_wc.lpszClassName = PROGRAM_NAME;
    RegisterClass( &main_wc );
    
    /*
	* create a window
	*/
	
    main_hwnd = CreateWindowEx(
#if defined(UNDER_CE)
		0,
#else
        0,
#endif
        PROGRAM_NAME,
        WINDOW_TITLE,
#if defined(UNDER_CE)
		WS_VISIBLE,
#else
#if WINDOW_MODE
        0,
#else
		WS_POPUP,
#endif
#endif
#if WINDOWHACK	
		1200,
#else
        0, //0,
#endif
        0, //0,
#if WINDOW_MODE
        XRES+6,//GetSystemMetrics( SM_CXSCREEN ),
        YRES+25,//GetSystemMetrics( SM_CYSCREEN ),
#else
		XRES,//GetSystemMetrics( SM_CXSCREEN ),
        YRES,//GetSystemMetrics( SM_CYSCREEN ),
#endif
        NULL,
        NULL,
        hInstance,
        NULL );
	
    if( !main_hwnd )
    {
        return false;
    }
#if !defined(UNDER_CE)
#if WINDOWHACK
    ShowWindow( main_hwnd, SW_SHOWMINIMIZED );
#else
    ShowWindow( main_hwnd, nCmdShow);
#endif
    UpdateWindow( main_hwnd );
    SetFocus( main_hwnd );
#endif	
	return true;
}

// 'new' trap routine

int outOfMemory( size_t )
{
	
	fatalError("Insufficient Memory");
	
	return(0);
}


static char zzz[80];	// temporary workspace

bool eax = false;
bool rearglance=false;

gamesetup *front_end_stub()
{
	char *cn;
	
	char buf[256];
	int i;

#if USE_FRONT_END
	dprintf("entering front end");

	MoveWindow(main_hwnd, 0,0,FRONTEND_XRES,FRONTEND_YRES,false);
	
	if (!MainFrontEnd)
		MainFrontEnd = new FrontEndClass(main_hwnd, WINDOW_MODE);
	if (!MainFrontEnd->Start())
		return NULL;
	dprintf("leaving front end");

/*	
	FILE *a=fopen("worldname.txt","r");
	if(!a)fatalError("cannot find worldname.txt");
	fgets(GlobalGame.trackname,32,a);
	GlobalGame.trackname[strlen(GlobalGame.trackname)-1]=0;
*/	
	char Buffer[256];

	// Check if we want a replay.
	GetRegistryValue(REGISTRY_DIRECTORY, "Replay", Buffer);
	if (!stricmp(Buffer, "TRUE"))
	{
		// We need a replay.
		if(DoesReplayExist())
		{
			LoadReplayHeader(&GlobalGame);
			GlobalGame.flags |= GSF_REPLAY;
			return &GlobalGame;
		}
	}	

	for (i=0 ; i<12 ; i++)
	{
		char CarString[10];
		sprintf(CarString, "RaceCar%d", i);
		GetRegistryValue(REGISTRY_DIRECTORY, CarString, GlobalGame.cars[i].carname);
		char *NameString = GlobalGame.cars[i].carname;
	}
	GetRegistryValue(REGISTRY_DIRECTORY, "Track", GlobalGame.trackname);
	
	// Find out what kind of race we're talking.

	overlay.qualmode=false;
	exit_on_outoffuel = false;
	practice_mode = false;

	GetRegistryValue(REGISTRY_DIRECTORY, "RaceType", Buffer);
	if (!stricmp(Buffer, "SINGLERACE"))
	{
		GetRegistryValue(REGISTRY_DIRECTORY, "Laps", Buffer);
		GlobalGame.laps = atoi(Buffer);
		if (GlobalGame.laps < 1)
			GlobalGame.laps = 5;
	
		GetRegistryValue(REGISTRY_DIRECTORY, "NumberOfCars", Buffer);
	
		GlobalGame.numcars = atoi(Buffer);
	}
	else if (!stricmp("Practice", Buffer))
	{
		exit_on_outoffuel = true;
		GlobalGame.laps = 99;
		GlobalGame.numcars = 1;
		practice_mode=true;
	}
	else if (!stricmp("Qualify", Buffer))
	{
		exit_on_outoffuel = true;
		GlobalGame.laps = 4;
		GlobalGame.numcars = 1;
		overlay.qualmode=true;
	}

	GlobalGame.flags = 0L;
	GlobalGame.cars[0].isplayer = true;
	
	return &GlobalGame;
#else


//	FILE *a=fopen("worldname.txt","r");
//	if(!a)fatalError("cannot find worldname.txt");
//	fgets(GlobalGame.trackname,32,a);
//	GlobalGame.trackname[strlen(GlobalGame.trackname)-1]=0;

	strcpy(GlobalGame.trackname,"donington");
	GlobalGame.numcars = 1;
	GlobalGame.laps = 5;
	GlobalGame.flags = 0L;

	for(i=0;i<GlobalGame.numcars;i++)
	{
		GlobalGame.cars[i].isplayer=false;
		strcpy(GlobalGame.cars[i].carname, "Mercedes154_15");
	}

	GlobalGame.cars[0].isplayer=true;	

	strcpy(GlobalGame.cars[0].carname, "AutoUnionC_4");
	strcpy(GlobalGame.cars[1].carname, "Mercedes154_15");
	strcpy(GlobalGame.cars[2].carname, "Mercedes125_16");
	strcpy(GlobalGame.cars[3].carname, "AlfaBimotore_66");
	strcpy(GlobalGame.cars[4].carname, "BonnerSpecial_30");
	strcpy(GlobalGame.cars[5].carname, "Miller_48");
	strcpy(GlobalGame.cars[11].carname, "NapierRailton_19");

	return &GlobalGame;
#endif
}

float debug_amp_low=0,debug_amp_high=0,debug_amp_top=0;
float debug_pitch_low=0,debug_pitch_high=0,debug_pitch_top=0;

bool use_steering_wheel=true;

extern void positionCamera(car *c);
extern void positionMirrorCamera(car *c);

void SetDriverVisibility(bool v)
{
	cameracar->bodyObject->visible = v;
	cameracar->driverObject->visible = v;
	cameracar->FaxleObject->visible = v;
	cameracar->RaxleObject->visible = v;
	cameracar->RwheelObjectLEFT->visible = v;
	cameracar->RwheelObjectRIGHT->visible = v;
	if(cameracar->bodyShadowObject)
	{
		cameracar->bodyShadowObject->visible = v;
		cameracar->FwheelShadowObjectLEFT->visible = v;
		cameracar->FwheelShadowObjectRIGHT->visible = v;
		cameracar->RwheelShadowObjectLEFT->visible = v;
		cameracar->RwheelShadowObjectRIGHT->visible = v;
		cameracar->FaxleShadowObject->visible = v;
		cameracar->RaxleShadowObject->visible = v;
	}
}


void SetShadowVisibilities()
{
	vector3 d;
	car *c;
	float f;
	bool v;

	for(int i=0;i<game->numcars;i++)
	{
		c = cararray[i];

		bool hasshad = c->bodyShadowObject ? true : false;
		bool iscamcar = (c==cameracar);
		bool incarview = (cameraview==CAM_INCAR || cameraview==CAM_BUMPER);

//		dprintf("hasshad %s iscamcar %s incarview %s",
//			hasshad?"true":"false",
//			iscamcar?"true":"false",
//			incarview?"true":"false");

		if(!(!hasshad || (iscamcar && incarview)))
		{
			d = boris2->currentcamera->position - c->mainObject->transform.translation;
			f = d.MagSquared();

			if(f>10000)v=false;
			else v=true;

//			dprintf("dist %f   vis %s",f,v?"true":"false");

			c->bodyShadowObject->visible = v;
			c->FwheelShadowObjectLEFT->visible = v;
			c->FwheelShadowObjectRIGHT->visible = v;
			c->RwheelShadowObjectLEFT->visible = v;
			c->RwheelShadowObjectRIGHT->visible = v;
			c->FaxleShadowObject->visible = v;
			c->RaxleShadowObject->visible = v;
		}
	}
}







static int ambsnd=0;
extern char ccode[];
void SetCameraView()
{
	switch(cameraview)
	{
	case CAM_INCAR:
		cameracar->attachCamera(ATTACH_FOLLOW, &vector3(0,-camposy,camposz+camposzModifierInCar));
		if(!AllCameras->UsingTrackside)
			SetDriverVisibility(false);
		break;


		// NOTE that the coordinates here are ignored; look in CameraPosition.cpp

	default:
		cameracar->attachCamera(ATTACH_FOLLOW, &vector3(0,-0.5f,1000));

		SetDriverVisibility((cameraview==CAM_BUMPER || 
			(!(game->flags & GSF_REPLAY)) && cameraview==CAM_PITSTOP)? false : true);
		positionCamera(cameracar);
		break;
	}
}


//////////////////////////////Responding to Keys Pressed////////////////////////////

char local_controller_keybuffer[256];
char old_controller_keybuffer[256];

void CheckKeysPressed()
{

	extern void PollKeys(char *);

	memcpy(&old_controller_keybuffer,&local_controller_keybuffer,256);

	PollKeys((char *)&local_controller_keybuffer);

#define	ISPRESSED(x)	(local_controller_keybuffer[x] & 0x80)
#define	KEYDOWN(x)	((local_controller_keybuffer[x] & 0x80) && (!(old_controller_keybuffer[x] & 0x80)))

//	if(cameracar->controller->type != CONTROLLER_REPLAY)
	{
		if(KEYDOWN(DIK_TAB))
		{
			if(!practice_mode)
			{
				SetDriverVisibility(true);
				// Change to the next car.
				if (cameracar->id == GlobalGame.numcars-1)
					cameracar = cararray[0];
				else
					cameracar = cararray[cameracar->id + 1];
				if(realcameraview == CAM_INCAR && cameracar!=playercar)
					cameraview=CAM_OUT1;
				else
					cameraview=realcameraview;

				SetCameraView();
			}
		}

		if(ISPRESSED(DIK_END))
		{
			if(zback<100)
				zback-= 1.0f;
			else if(zback<1000)
				zback-=10;
			else
				zback-=100;
			dprintf("zback %f",zback);
		}
		if(ISPRESSED(DIK_HOME))
		{
			if(zback<100)
				zback+= 1.0f;
			else if(zback<1000)
				zback+=10;
			else
				zback+=100;
			dprintf("zback %f",zback);
		}
	}

	if (UseFloatingCamera)
	{
		vector3 CameraDirection = FloatingCamera->direction;
		static vector3 CurrentPosition, OldPosition, DisplayedPosition;
		static vector3 FloatingDirection=vector3(0.0f,0.0f,0.0f);

		static int Speed=4;

		CurrentPosition = FloatingCamera->position;
		OldPosition = CurrentPosition;

		if ISPRESSED(DIK_NUMPAD0)
			Speed = 20;

		if ISPRESSED(DIK_NUMPAD8)
		{
			CameraDirection.x = -CameraDirection.x;
			CameraDirection.y = -CameraDirection.y;
			FloatingCamera->moveCameraPosition(&vector3(CameraDirection.x*Speed*0.1f, 0.0f, CameraDirection.z*Speed*0.1f));
		}
		if ISPRESSED(DIK_NUMPAD2)
		{
			CameraDirection.x = -CameraDirection.x;
			CameraDirection.y = -CameraDirection.y;
			FloatingCamera->moveCameraPosition(&vector3(CameraDirection.x*Speed*-0.1f, 0.0f, CameraDirection.z*Speed*-0.1f));
		}
		if ISPRESSED(DIK_NUMPAD4)
		{
			vector3 NewDirection;
			NewDirection = CameraDirection;
			FloatingDirection.y+=PI*0.001f*Speed;
			if (FloatingDirection.y > 2*PI) FloatingDirection.y -= 2*PI;
		}
		if ISPRESSED(DIK_NUMPAD6)
		{
			vector3 NewDirection;
			NewDirection = CameraDirection;
			FloatingDirection.y-=PI*0.001f*Speed;
			if (FloatingDirection.y < 0) FloatingDirection.y += 2*PI;
		}
		if ISPRESSED(DIK_NUMPAD3)
		{
			/*vector3 NewDirection;
			NewDirection = CameraDirection;
			FloatingDirection.x-=PI*0.001f*Speed;
			if (FloatingDirection.x < 0) FloatingDirection.x += 2*PI;
			*/
			FloatingCamera->moveCameraPosition(&vector3(0.0f, -Speed*0.01f, 0.0f));
		}
		if ISPRESSED(DIK_NUMPAD9)
		{
			/*vector3 NewDirection;
			NewDirection = CameraDirection;
			FloatingDirection.x+=PI*0.001f*Speed;
			if (FloatingDirection.x > 2*PI) FloatingDirection.x -= 2*PI;
			*/
			FloatingCamera->moveCameraPosition(&vector3(0.0f, Speed*0.01f, 0.0f));
		}

		CurrentPosition = FloatingCamera->position;
		if (CurrentPosition != OldPosition)
		{
			DisplayedPosition = CurrentPosition - OldPosition;
		}
		FloatingCamera->setCameraAngle(&FloatingDirection);
		
		Speed = 4;
	}
}

int kp=0;

__int64 current_time_stamp,time_stamp_paused,time_stamp_pauseddiff;

unsigned long start_of_race;
__int64 counter,frequency,tmp64;
static __int64 ticklength,basecounter,real_basecounter,ticksrun=(__int64)1;

void SetTimeStamp()
{
	__int64 ts;

	QueryPerformanceCounter((LARGE_INTEGER *)&ts);
	ts /= (frequency/100);

	if(Paused)
	{
		current_time_stamp = time_stamp_paused;
		time_stamp_pauseddiff = ts - time_stamp_paused;
	}
	else
		current_time_stamp = ts-time_stamp_pauseddiff;
}


bool HandleKey(WPARAM key)
{
	// assorted keys
	static int LastPosition=999;
	vector3 CarPosition;
	char CameraLocation[100];
	static int pwidx=0;
	static int rrrr=0;

	// TO QUIT - <RETURN>90210


	if(key<='9' && key>='0')
	{
		if(pwidx<10)
		{
			password[pwidx++]=key;
			password[pwidx]=0;
			if(!strcmp(password,"90210"))
			{
				for(int i=0;i<game->numcars;i++)
					cararray[i]->Test();
			}
			if(!strcmp(password,"2624589"))
			{
				kp=100;
			}
			if(!strcmp(password,"98989898"))
			{
				for(int i=0;i<game->numcars;i++)
					cararray[i]->gravval=0.4f;
			}
			if(!strcmp(password,"89898989"))
			{
				for(int i=0;i<game->numcars;i++)
					cararray[i]->gravval=2.0f;
			}
			if(!strcmp(password,"67676767"))
			{
				for(int i=0;i<game->numcars;i++)
					cararray[i]->bounciness=0.9f;
			}
			if(!strcmp(password,"76767676"))
			{
				for(int i=0;i<game->numcars;i++)
					cararray[i]->bounciness=0.4f;
			}
			if(!strcmp(password,"0898"))
			{
				for(int i=0;i<game->numcars;i++)
				{
					if(cararray[i]!=playercar)
					{
						for(int j=0;j<4;j++)
						{
							cararray[i]->oldheight[j]-=0.5f;
						}
					}
				}
			}
			if(!strcmp(password,"0989"))
			{
				for(int j=0;j<4;j++)
				{
					playercar->oldheight[j]-=1.0f;
				}
			}

			if(!strcmp(password,"999"))
			{
				playercar->flamingwheels=!playercar->flamingwheels;
		}

		}
	}
	switch(key)
	{
	case VK_RETURN:
		pwidx=0;break;



	case VK_F1:	// in car / out car camera

		if(Paused)
		{
			// continue

			if(raceglobals.state!=RACE_AFTER && raceglobals.state!=RACE_FINISHED)
			Paused = false;
		}
		else if(!AllCameras->UsingTrackside && !rearglance && cameraview!=CAM_PITSTOP)
		{
			AllCameras->UseTrackside(false, false);	
			UseFloatingCamera = false;
			realcameraview++;
			detachmode=false;
			lagmode=false;
			if(realcameraview>CAM_MAX)realcameraview = CAM_INCAR;
			if(realcameraview == CAM_INCAR && cameracar!=playercar)
				cameraview=CAM_OUT1;
			else
				cameraview=realcameraview;
			
			SetCameraView();
		}break;

	case VK_F2:	// detach camera
		if(Paused)
		{
			if(playercar->controller->type!=CONTROLLER_REPLAY && !playercar->engdead && !playercar->outoffuelct)
				playercar->disqualified=true;

			do_exit_hack=true;
			practice_mode = true;	// orrid ack
		}
		else if(!AllCameras->UsingTrackside && cameraview!=CAM_BUMPER && cameraview!=CAM_PITSTOP && !rearglance)
		{
			AllCameras->UseTrackside(false, false);	
			UseFloatingCamera = false;
			detachmode=!detachmode;
			lagmode=false;
			SetCameraView();
		}break;
	case VK_F3:	// lag mode camera
		if(!AllCameras->UsingTrackside && cameraview!=CAM_BUMPER && cameraview!=CAM_PITSTOP)
		{
			AllCameras->UseTrackside(false, false);	
			UseFloatingCamera = false;
			lagmode=!lagmode;
			detachmode=false;
			SetCameraView();
		}break;
	case VK_F6:
		jjshack99^=1;break;


#if FRAMEDUMP
	case VK_F11:
		DumpFrames=!DumpFrames;
		break;
#endif

	
	case VK_F12:	// Show car info overlay
		if(!Paused)
		{
			carinfo_on=!carinfo_on;
		}

		return true;
/*
	case 'C':
		if (UseFloatingCamera)
		{
			sprintf(CameraLocation, "%d %f %f %f\n\0", 999, FloatingCamera->position.x, FloatingCamera->position.y, FloatingCamera->position.z); 
			fwrite(CameraLocation, 1, strlen(CameraLocation), CameraLocations);
		}
		else
		{
			if (positiontable.rlp1[0] != LastPosition)
			{
				CarPosition = cararray[0]->mainObject->concatenatedTransform.translation;
	
				sprintf(CameraLocation, "%d %f %f %f\n\0", positiontable.rlp1[0], CarPosition.x, CarPosition.y, CarPosition.z); 
				fwrite(CameraLocation, 1, strlen(CameraLocation), CameraLocations);
			}
		}
		LastPosition = positiontable.rlp1[0];
		break;

		// Queue a New camera position for viewing.
	case 'N':
		if (UseFloatingCamera)
		{
			AllCameras->UseTrackside(true, false);
			AllCameras->QueueCamera(FloatingCamera);
			AllCameras->UseTrackside(true, false);break;
		}
		break;

	case 'S':
		cameracar->CurrentPitstop = new Pitstop(cameracar->id, AllPitstopGraphics);
		break;
*/
	case VK_F4:
		grdebugstate = !grdebugstate;
		grenable(grdebugstate);break;


#if CARSOUNDPARAMEDITOR
	case 'R':
		carsounds.ParamEditor(CPEM_INC);break;
	case 'F':
		carsounds.ParamEditor(CPEM_DEC);break;
	case 'E':
		carsounds.ParamEditor(CPEM_NEXT);break;
	case 'D':
		carsounds.ParamEditor(CPEM_PREV);break;
	case 'M':
		carsounds.ParamEditor(CPEM_DUMP);break;

#endif

#if DEBUGCAMS

	case 'D':
		AllCameras->UseTrackside(false, false);
		UseFloatingCamera = true;
		boris2->setCurrentCamera(FloatingCamera);
		carsounds.NominateListener(FloatingCamera);
		break;
	case 'F':
		AllCameras->UseTrackside(false,false);
		boris2->setCurrentCamera(boris2camera);
		UseFloatingCamera = false;
		break;

	case 'T':
		if (NoPlayers)
			AllCameras->UseTrackside(true,true);
		else
			AllCameras->UseTrackside(true,false);

		SetDriverVisibility(true);

/*
		dprintf("x%d	:	%f",ambsnd,boris2->currentcamera->position.x);
		dprintf("z%d	:	%f",ambsnd,boris2->currentcamera->position.z);
		dprintf("vol%d	:	1",ambsnd);
		dprintf("samp%d	:	crowd.wav",ambsnd);

		ambsnd++;
*/
#endif


//	case VK_F6:carsounds.debug(SDF_BASS);break;
//	case VK_F7:carsounds.debug(SDF_MID);break;
//	case VK_F8:carsounds.debug(SDF_TOP);break;


//	case VK_F9:boris2->envmapped2d = !boris2->envmapped2d;
//		break;


	case VK_ESCAPE:
	case VK_PAUSE:
		if((raceglobals.state==RACE_BEFORE || raceglobals.state == RACE_DURING || raceglobals.state == RACE_RACEWON)&& !Paused)
		{
			Paused = true;
			time_stamp_paused=current_time_stamp;
		}
		break;

	}

	return false;
		

}

int blackdelay=0; //delay put in so that player can see they've been black flagged
static int slowcount[16];
void FlagCheck()
{

	overlay.showflags = 0;
	static int flashct=0;	// just a little cycling wossname, 0 to 30, period 0.5 second.
	static bool disq=false; //local disqualification
	int i;

	if(flashct++ > 30)flashct=0;

	switch(raceglobals.state)
	{
		// yellow flag checks. Yellow flag comes out when any car has had mph<30 for a lot of ticks.

	case RACE_RACEWON:
		// *someone* has won the race. Flash the check flag
		if(flashct++>15)
			overlay.showflags|=FLAG_CHECK;
		// deliberate fallthrough!
	case RACE_DURING:
		for(i=0;i<game->numcars;i++)
		{
			if(cararray[i]!=playercar)
			{
				if(cararray[i]->mph<20)
					slowcount[i]++;
				else
					slowcount[i]=0;
			}

		if(cararray[i]->CurrentPitstop) cararray[i]->wrongwayct=0;
		//Must put in something for AI pitstops
		
		if(cararray[i]==playercar)
			{
				for(int j=0;j<game->numcars;j++)
				{
					if((slowcount[j]>10*60) || (cararray[j]->engdead))
					{
						racingLine *RacingLine;
						RacingLine = racinglineset.getLine(0);
						float numpoints	= RacingLine->pointsPerLine;

						if(numpoints==475) 
						{
							RacingLine = racinglineset.getLine(1);
							numpoints	= RacingLine->pointsPerLine;
						}

						float cpos = positiontable.rlp1[playercar->id];
						float aipos = positiontable.rlp1[cararray[j]->id];

						float difpos =aipos-cpos;

						if (difpos<100 && difpos>-25) overlay.showflags|=FLAG_YELLOW;
						else if(difpos<0)
						{
							if((numpoints + difpos)<100) overlay.showflags|=FLAG_YELLOW;
						}
						else
						{
							if((numpoints - difpos)<25) overlay.showflags|=FLAG_YELLOW;
						}
					}
				}
			}

			if(cararray[i]->wrongwayct > 400)
			{
				if(cararray[i]==playercar)
				{
					if(!playercar->disqualified)
					{
						if(flashct>15 || playercar->wrongwayct <800) overlay.showflags|=FLAG_BLACKNWHITE;

						if(playercar->wrongwayct >1000)
						{
							playercar->disqualified=true;
						}
					}
				}
				else
					overlay.showflags|=FLAG_YELLOW;
				
		
			}
		}
		if(playercar->disqualified)
		{
			overlay.showflags|=FLAG_BLACK;
			if(0 && ++blackdelay>200)
			{
				raceglobals.state = RACE_AFTER;
				cameracar->disqualified=true;
			}

		}

		if(playercar->blackflag_on)cameracar->wrongwayct+=2; // twice as fast!

	

		if(playercar->blownct > 250 || playercar->outoffuelct > 1200)	// more than 20 seconds out of fuel
		{
			if(exit_on_outoffuel || raceglobals.state ==RACE_RACEWON)
				raceglobals.state = RACE_AFTER;
		}

		break;
	case RACE_AFTER:
		if(!cameracar->disqualified) overlay.showflags|=FLAG_CHECK;
		else overlay.showflags|=FLAG_BLACK;
		break;
	}

	if(playercar!=cameracar)overlay.showflags=0;
}







/////////////////////////////// Physics Thread /////////////////////////////////////

int rlfirst, rlmain, rllast;	// first, main and last lap racing lines respectively


//// this routine is run every tick, controlled by ThreadProc


//novelcount counts how many ticks the car is going very slowly
unsigned long novelcount=0;
//store old view while in pits;
int storeview=cameraview;

unsigned long pitstopdelay=0;

bool global_dosprites;

void Tick(bool dosprites,bool initial_tick)
{
	int i;
	if(Paused)
	{
		global_dosprites=false;
		boris2->runsprites=false;
		goto skipit;
	}

	global_dosprites=dosprites;
	boris2->runsprites=true;

	if(pitstopdelay)pitstopdelay--;

	runParticles();

	for(i=0;i<game->numcars;i++)
	{
		ctrlpckt p;
		
		Controller *con = cararray[i]->controller;
		p=con->GetControlPacket();

		if(cararray[i]->outoffuelct || (con->type==CONTROLLER_REPLAY && raceglobals.state!=RACE_DURING))
			p.accbrake=-120;

		if(con->type == CONTROLLER_LOCAL)
		{
			rearglance = (p.flags & CF_REARGLANCE) ? !detachmode : false;
		}

		if(p.flags & CF_FINISH)
		{
			dprintf("finish flag caught");
			raceglobals.state = RACE_AFTER;
		}

		// inhibit gear changes before the start of the race

		if(raceglobals.state == RACE_BEFORE && con->type!=CONTROLLER_REPLAY)
		{
			p.flags &= ~(CF_GEARUP|CF_GEARDN);
			cararray[i]->nomove=true;
//			p.accbrake = -100;
		}
		else cararray[i]->nomove = false;

		if (cararray[i]->CurrentPitstop)
		{
			if (!(cararray[i]->CurrentPitstop->Update()))
			{
				// returns false if the pitstop is over.
				delete cararray[i]->CurrentPitstop;
				cararray[i]->CurrentPitstop = NULL;
				cameraview=storeview;
				positionCamera(cameracar);
				SetCameraView();

				// and begin a delay

				pitstopdelay = TICKRATE*5;	// five seconds
			}
		}
		else
		{
			if(cararray[i]==playercar && cameracar==playercar)
			{
				if(cameracar->mph<1 && cameracar->mph>-1) novelcount++; else novelcount=0;
				if(cameracar->pitstop_on && !pitstopdelay && novelcount>10)
				{
					storeview=cameraview;
					cameraview = CAM_PITSTOP;
					positionCamera(cameracar);
					SetCameraView();					
					cameracar->CurrentPitstop = new Pitstop(cameracar->id, AllPitstopGraphics);
					
				}
			}

			if(p.flags & CF_GEARUP)
				cararray[i]->shiftUpGear();	
			if(p.flags & CF_GEARDN)
				cararray[i]->shiftDownGear();
			
			float a,b,s;
			p.GetPacketData(&a,&b,&s);

			cararray[i]->accelvalue=a;
			cararray[i]->move(s,a,b);
			cararray[i]->doSuspension(s);
		}
	}
	if(AllCameras->UpdateCameras())
	{
		positionCamera(cameracar);
		if(cameraview == CAM_INCAR)
		{
			positionMirrorCamera(cameracar);
		}
	}
	positiontable.Update(game->numcars);
	FlagCheck();

	// update the sound sources and the listener

skipit:
	if(!initial_tick)carsounds.Update();
	CheckCarsForSkids();
}


static void TestParticles()
{
	unsigned long cols[] = { 0xffff0000,0xff00ff00,0xff0000ff,0xffffffff,0xff000000};
	int qqq=0;

	vector3 v;

	cameracar->mainObject->transform.multiplyV3By3x4mat(&v,&vector3(1,1,4));

	for(int i=0;i<(rand()%10);i++)
	{
		makeParticle(&v,		// pos
			&vector3(0,0.1f,0),		// vel
			&vector3(0,-0.1f,0),	// acceleration
			&vector3(0.1f,0.1f,0.1f),	// pos randomness
			&vector3(0.02f,0.02f,0.02f),	// vel randomness
			60*4,2,cols[qqq]);			// tickstolive, size, col
		qqq = (qqq+1)%5;
	}

}




static void ReplaceController(car *c)
{
	// replace what is assumed to be a LocalController with an AIController.
	// Beware of what this might do to the replay system!

	delete c->controller;	// delete the old controller
	Controller *newcon = (Controller *) new AIController();
	newcon->Attach(c);
}



////////////////////////////////////// Main /////////////////////////////////////////

unsigned long Now()
{
	return (unsigned long)current_time_stamp;
}

unsigned long winner_finish_time = 0;
int reallanguage;

void CheckFinished()
{
	// check to see if someone has finished the race

	for(int i=0;i<game->numcars;i++)
	{
		if(positiontable.laps[i] >= game->laps)
		{
			if(raceglobals.playerlastlap<0)
			raceglobals.playerlastlap = positiontable.laps[playercar->id]; // record player lap
			
			if (!positiontable.finishtime[i])
			{
				positiontable.finishtime[i] = Now() - start_of_race;
				dprintf("car %d FINISHED : setting finishtime to %ld",i,positiontable.finishtime[i]);
				if(!winner_finish_time)winner_finish_time = positiontable.finishtime[i];
			}
			
			if (raceglobals.state != RACE_AFTER)
			{
				if(playercar->id == i)	// the player finished. Huzzah!
				{
					if (raceglobals.state == RACE_DURING)
						if(!practice_mode && !overlay.qualmode)
						{
							char Buffer[255];
#if 0
							GetRegistryValue(REGISTRY_DIRECTORY, "Key_RaceType", Buffer);
#endif
							if (strcmp(Buffer, "SCENARIO"))
								PaperFlag = true;
						}
					raceglobals.state = RACE_AFTER;
				}
				else
					raceglobals.state = RACE_RACEWON;	// change state
			}
			}
		}
				
}

int defaultreverb=0;
char *jcfhack1;	// uuuuuuuuuuuuuuuuugghhh!
bool race_begun;

#if defined(UNDER_CE)
extern "C" int APIENTRY WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#else
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#endif
{
	int i;
	int Success;
	float worldscale=1.0;	// global scaling factor
	int lod_scaling_counter;
	char buf[256];

	extern void SetConCodesArray(int);


//	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_DEBUG);
//	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG);
//	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG);

//	int tf = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
//	tf |= _CRTDBG_CHECK_ALWAYS_DF;

	srand(7);

	BOOL screen_saver_active;	// true if the screen saver is enabled

// Open a file for recording.
//	RecordingFile = fopen("c:\\sosrecord.dat", "wb");

#if !ALLOWTABBING & !defined(UNDER_CE)
	SystemParametersInfo(SPI_GETSCREENSAVEACTIVE,0,&screen_saver_active,0);
	
	// turn off the screensaver
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,FALSE,NULL,0);

	SystemParametersInfo(SPI_SCREENSAVERRUNNING,TRUE,NULL,0);
#endif

	time_stamp_paused = time_stamp_pauseddiff=0;

	make_window(hInstance,nCmdShow);
	dprintf("WinMain: window made");

#if !defined(UNDER_CE)
    _PNH old_handler = _set_new_handler( outOfMemory );
	dprintf("WinMain: handler set");
#endif
	// and get how many processor ticks there are in a second
	QueryPerformanceFrequency((LARGE_INTEGER *)&frequency);

	SetTimeStamp();

	// work out the length of a game tick in processor ticks
	ticklength = (int)(frequency/TICKRATE);	// in car.cpp

	if(!frequency)
		fatalError("QueryPerformanceFrequency returned zero!");

	// Read the registry for current directory etc.  IMG

	// push the global archive onto the stack

	if(!arcPush("archives\\global.tc"))
		fatalError("unable to open global archive");

	dprintf("WinMain: global archive pushed");
#if 0
	GetRealRegistryValue("Software\\Hasbro Interactive\\Spirit of Speed 1937","Language",buf);
	if(!stricmp(buf,"english"))
		reallanguage=L_ENGLISH;
	else if(!stricmp(buf,"welsh"))
		reallanguage=L_WELSH;
	else if(!stricmp(buf,"german"))
		reallanguage=L_GERMAN;
	else if(!stricmp(buf,"italian"))
		reallanguage=L_ITALIAN;
	else if(!stricmp(buf,"french"))
		reallanguage=L_FRENCH;
	else 
#endif
		reallanguage=L_ENGLISH;
#if !defined(UNDER_CE)
	SetConCodesArray(reallanguage);
#endif

	// initialise the renderer; this just gets some data and sets up structures

	renderInitialise(main_hwnd,800, 600, false);//800,600,true);//false);

	// set up the DirectDraw surfaces
#if 0
	renderDeInitialise(CLOSE_3D);

	LPVIDEOMODES StartOfVideoModes;
	LPVIDEOMODES vidmodes = StartOfVideoModes = GetVideoModes();

	// Find the resolution we need from the "registry".
	GetRegistryValue(REGISTRY_DIRECTORY, "ScreenMode", buf);

	if (!stricmp(buf, "NULL"))
	{
		// We don`t have a registry. Default to 800x600.
		strcpy(buf, "800x600");
	}
	// Terminate the string at the x.
	*(strchr(buf, 'x')) = 0;
	ResolutionX = atoi(buf);
	ResolutionY = atoi(buf+strlen(buf)+1);
	
	int vm=0,fd=-1, FrontEnd_fd;

	// Find the screenmode for the front end.
	for(;;)
	{
		if(!vidmodes->height)break;

		dprintf("checking video mode %d : %dx%dx%d",vm,vidmodes->width,vidmodes->height,vidmodes->depth);

		if (vidmodes->width == FRONTEND_XRES && vidmodes->height == FRONTEND_YRES && vidmodes->depth == FRONTEND_DEPTH)
		{
			FrontEnd_fd = vm;
		}

		vidmodes++;vm++;
	}

	dprintf("video mode = %d",FrontEnd_fd);
	if(FrontEnd_fd==-1)fatalError("unable to find mode");


	// initialise the sound system and load the sounds 

	// initialise the joystick/wheel if there is one



	// always turn force feedback off
//	force_feedback_level = 0;


////////// Temporary mirror hack
	char *p;

	FILE *f1;
	BorisMaterial Merc154Mask;
	f1=fopen("image12.3df","rb");
	p=new char[32823];
	fread(p,32823,1,f1);
	fclose(f1);
	Merc154Mask.textureHandle=registerTexture(p);
	Merc154Mask.flags=MAT_NOBLEND;

	delete[] p;
#define MIRRORZ 0.0000001f
	D3DTLVERTEX vert[4];

        vert[0].sz = MIRRORZ;
        vert[0].rhw = 1.0f;
        vert[0].color =  RGB_MAKE(255,255,255);
        vert[0].tu=0.0;
        vert[0].tv=0.0;

        vert[1].sz = MIRRORZ;
        vert[1].rhw = 1.0f;
        vert[1].color =  RGB_MAKE(255,255,255);
        vert[1].tu=1.0;
        vert[1].tv=0.0;

        vert[2].sz = MIRRORZ;
        vert[2].rhw = 1.0f;
        vert[2].color =  RGB_MAKE(255,255,255);
        vert[2].tu=1.0;
        vert[2].tv=1.0;

        vert[3].sz = MIRRORZ;
        vert[3].rhw = 1.0f;
        vert[3].color =  RGB_MAKE(255,255,255);
        vert[3].tu=0.0;
        vert[3].tv=1.0;
//////////////

	/////////////////////////////////////////////////////////////////////////////////////////////

//	SetRegistryValue(REGISTRY_DIRECTORY,"Sounds_VolumeSwitch","On");
	dprintf("entering front end loop");
//	PlayMovie("gamedata\\video\\logo5.avi",0,0,800,600);
//	PlayMovie("gamedata\\video\\intro.avi",0,0,800,600);
	cameraview=CAM_OUT1;
#else
	ResolutionX = XRES;
	ResolutionY = YRES;
#endif
	for(;;)	// FRONT END LOOP
	{

		bool has_sound=false;

#if !NOSOUND
		GetRegistryValue(REGISTRY_DIRECTORY,"Sounds_VolumeSwitch",buf);
		if(stricmp(buf,"Off"))
		{

			GetRegistryValue(REGISTRY_DIRECTORY, "EAX", buf);
			if(!stricmp(buf,"on"))eax = true; else eax = false;	
			eax = true	;

//			eax = false; // turn eax always off
			dprintf("initialising sound");
			carsounds.Initialise(main_hwnd,eax);	// bool indicates whether EAX is used
			dprintf("initialising sound done OK");
			has_sound=true;
		}
#endif


		dprintf("initialising replay buffers");
		replaybuffermanager.Initialise();
		dprintf("replay buffers OK"	);

		// HERE WE SHOULD RUN THE FRONT-END CODE

#if 0
		ResetVideoMode(FrontEnd_fd);
		dprintf("running front end");
#endif
		
		// startup DI so we can query the joystick
		 
		InitialiseDirectInput(hInstance,0);
		if(!(game=front_end_stub()))break;	// for now it's just a stub
		
		ShutdownDirectInput();

		carinfo_on=false;
		dprintf("WinMain: front end run OK");
#if 0
		GetRegistryValue(REGISTRY_DIRECTORY,"CarVolume",buf);
		if(!stricmp(buf,"low"))
			carsounds.carvolume = 0.2;
#endif
		// make the sound agree with the front end
#if !NOSOUND
		GetRegistryValue(REGISTRY_DIRECTORY,"Sounds_VolumeSwitch",buf);


		if(has_sound)
		{
			if(!stricmp(buf,"Off"))
			{
				dprintf("closing down sound after state switch");
				GetRegistryValue(REGISTRY_DIRECTORY,"Sounds_VolumeSwitch",buf);
				carsounds.Shutdown();
				has_sound=false;
			}
		}
		else
		{
			if(stricmp(buf,"Off"))
			{
				dprintf("initialising sound after state switch");
				GetRegistryValue(REGISTRY_DIRECTORY,"Sounds_VolumeSwitch",buf);
				carsounds.Initialise(main_hwnd,eax);	// bool indicates whether EAX is used
				dprintf("initialising sound done OK");
				has_sound=true;
			}
		}
#endif

		
#if 0
		{
			char buf[20];
			// Find the resolution we need from the "registry".
			GetRegistryValue(REGISTRY_DIRECTORY, "ScreenMode", buf);
			// Terminate the string at the x.

			if(!strchr(buf,'x'))strcpy(buf,"800x600");

			*(strchr(buf, 'x')) = 0;
			ResolutionX = atoi(buf);
			ResolutionY = atoi(buf+strlen(buf)+1);
		}
		MoveWindow(main_hwnd, 0,0,ResolutionX,ResolutionY,false);
		// Now look for the requested video mode for the game.
		vidmodes = StartOfVideoModes; vm=0;
		for(;;)
		{
			if(!vidmodes->height)break;
			
			dprintf("checking video mode %d : %dx%dx%d",vm,vidmodes->width,vidmodes->height,vidmodes->depth);

#if FRAMEDUMP
			if(vidmodes->width == ResolutionX && vidmodes->height==ResolutionY && vidmodes->depth==32)
#else
			if(vidmodes->width == ResolutionX && vidmodes->height==ResolutionY && vidmodes->depth==16)
#endif
			{
				// this data is used when the front end doesn't run
				
				fd = vm;
				
			}

			vidmodes++;vm++;
		}
		// If the screenmode requested isn`t valid, use the front-end mode.
		if (fd == -1)
			fd = FrontEnd_fd;

		// Reset the video mode to the game mode.
		// This is the *last* thing to do, so the postcard isn`t erased from the screen.
		if (fd != FrontEnd_fd)
		{
			ResetVideoMode(fd);
			sprintf(resString,"%d",resX=StartOfVideoModes[fd].width);
		}
		else
		{
			resX=800;
			strcpy(resString,"");
		}
#endif
		// Push the track archive

		sprintf(zzz,"archives\\%s.tc",game->trackname);
		dprintf("WinMain: pushing archive %s",zzz);
#if 0
		use_steering_wheel=true;
		GetRegistryValue(REGISTRY_DIRECTORY, "SteeringWheel", buf);
		if(!stricmp("Off",buf))use_steering_wheel=false;
#endif
		jcfhack1 = game->trackname;

		if(!arcPush(zzz))
			fatalError("unable to open track archive %s",zzz);


		// NOW START UP THE 3D SUBSYSTEM
#if 0
		renderStart3D();
#endif
		dprintf("WinMain: 3D started");
		startEngine( 0, 1, 40000, 40000, 40000,	200.0f );

		dprintf("WinMain: engine started");

		// initialise the display
//		boris2display  = new display(MySOSData.ActualVideoMode.width,MySOSData.ActualVideoMode.height,  0,0);	
		boris2display  = new display(ResolutionX,ResolutionY,  0,0);	
		dprintf("WinMain: display made %d %d",800,600);

		// in the beginning we create the heavens...


		// ... and the earth

		boris2->useRacingline=false;	// for now
		boris2world   = new world( "track",worldscale,64,64);
		dprintf("WinMain: track made");

#if DEMO
		if(!(boris2world->worldModel->hasTexture("dunlopsent_a0a.3df")))
		{
			fatalError("Cannot use this world in a demo");
		}
#endif
		//Load in the clipping lines for the track

		ulong cliphandle = arcExtract_txt("cliplines");
		
		boris2world->numcliplines=0;		

		if(cliphandle)
		{
			char tempbuf[128],*s;

			arcGets(tempbuf,128,cliphandle);
			while(strncmp(tempbuf, "end", 3))
			{
				
				if(tempbuf[0]!='*')
				{
					int i=0;
					while(tempbuf[i]==0) {tempbuf[i]=' ' ; i++;}

					s = strtok(tempbuf,", ");
					sscanf(s,"%f, %f",&boris2world->clipline[boris2world->numcliplines].x1,&boris2world->clipline[boris2world->numcliplines].y1);
					arcGets(tempbuf,128,cliphandle);

					i=0;
					while(tempbuf[i]==0) {tempbuf[i]=' ' ; i++;}

					s = strtok(tempbuf,", ");
					sscanf(s,"%f, %f",&boris2world->clipline[boris2world->numcliplines].x2, &boris2world->clipline[boris2world->numcliplines].y2);
					
					boris2world->numcliplines++;
				}
				arcGets(tempbuf,128,cliphandle);
			}


		}

		// load the track-specific data

		ParamFile pf;
		vector3 startposarray[16];
		int rlfirst,rlmain,rllast;
		float startingyangle=0.0;
		char *pptr;

		zback=1200.0;	// default z back plane

		rlfirst = rlmain = rllast = 0;
		if(!pf.Read("trackdata"))fatalError("unable to read track data");
		dprintf("read done");
		for(i=0;i<16;i++)
		{
			char spname[16];
			sprintf(spname,"spx%d",i);
			pptr = pf.Find(spname);
			if(pptr)
			{
				sscanf(pptr,"%f",&startposarray[i].x); 
				startposarray[i].x *= worldscale;
				startposarray[i].y = 0.0f;
				spname[2]='z';
				pptr = pf.Find(spname);
				sscanf(pptr,"%f",&startposarray[i].z);
				startposarray[i].z *= worldscale;
			}
			else
				startposarray[i] = vector3(10,0,10);


			dprintf("start position %d = %f,%f,%f",i,startposarray[i].x,
				startposarray[i].y,startposarray[i].z);
		}

		float SkyRotate = 0.0f;


		if(pptr = pf.Find("yangle"))
			sscanf(pptr,"%f",&startingyangle);
		if(pptr = pf.Find("rlfirst"))rlfirst = atoi(pptr);
		if(pptr = pf.Find("rlmain"))rlmain = atoi(pptr);
		if(pptr = pf.Find("rllast"))rllast = atoi(pptr);
		if(pptr = pf.Find("zback"))
		{
			dprintf("zback parameter exists, %s",pptr);
			float qqqq;
			sscanf(pptr,"%f",&qqqq);


			if(qqqq>zback)zback=qqqq;
			dprintf("zback set to %f",zback);
		}
		if (pptr = pf.Find("SkyRotate"))
			sscanf(pptr,"%f",&SkyRotate);		

		if(pptr = pf.Find("reverb"))defaultreverb = atoi(pptr);

		dprintf("racing lines : first lap = %d, main = %d, final lap = %d",rlfirst,rlmain,rllast);
		dprintf("starting yangle = %f",startingyangle);


		// ... and the debugging font.

		testFont = new font("smallfont");
		dprintf("WinMain: debugging font loaded");
		grdebuginit(testFont);	// set up the graphical debugger
		grenable(grdebugstate);

		// create a camera	

		boris2camera  = new camera( &vector3(0,0,0),
								600.0f, zback * worldscale ); // zeye 0 = 360 900 = 90 e.t.c.
		AllCameras = new CameraBank();//cararray, 1024.0f, zback * worldscale);
		boris2mirror  = new camera( &vector3(0,0,0),
								500.0f, 90.0f );
		boris2mirror2  = new camera( &vector3(0,0,0),
								500.0f, 90.0f );
		FloatingCamera = new camera(&vector3(0.0f, 10.0f, 0.0f), 1024.0f, zback*worldscale);



		dprintf("WinMain: camera made");

	

		LoadFX();
		dprintf("WinMain: sprite FX loaded");

		boris2->loadInSpriteList(worldscale);
		dprintf("sprite list loaded");

		boris2sky	  = new sky( "skydome",							// mulder filename prefix
			10.0f,							// scale of all co-ordinates (useful for distancing beyond z-clip)
			&vector3(0.0f, 0.0f, 0.0f),	// initial orientation of the sky model
//			&vector3(0.0f, 0.00009f, 0.0f),	// angular increment for rotating the sky
			&vector3(0.0f, SkyRotate, 0.0f),	// angular increment for rotating the sky
			90.0f						// update rate ( updates per second )
			);
		dprintf("WinMain: sky made");


		// set all the boris2 parameters

		boris2->setCurrentCamera( boris2camera );							// associate camera to engine
		boris2->setCurrentWorld( boris2world );							    // associate world to engine
		boris2->setCurrentDisplay(boris2display);							// associate display to engine
		boris2->setCurrentSky( boris2sky );
//		boris2->fogOn();

		dprintf("WinMain: parameters set");

		//AllCameras->CreateCameras();

		AllCameras->UpdateCameras();

		// load an ALPHA texture for an environment map of the sky.

		boris2->setCurrentSkyEnvironmentMap("env5.3df"); 
		boris2->setCurrentDetailMap("detailmap.3df"); 
		dprintf("WinMain: envmap loaded");


		// load the racing line data. Here, we check for the existence of an RLF in the archive
		// stack first.

		ulong rlhandle;
		bool hasraceline=false;

		// SDS - Need to put in the multiple race line stuff here...
		// Three racelines - each with different Zback Values
		// each raceline is prefixed by either
		// l_ m_ h_  - low medium high detail 

#ifdef UseZbackDetail
//		GetRegistryValue(REGISTRY_DIRECTORY, "DetailLevel", buf);
strcpy(buf,"High");
		if (!stricmp(buf, "Low"))
			{
			if(rlhandle = arcExtract_rlf("l_raceline"))
				{
				detaillevel=0;
				dprintf("low detail racing line set found");arcDispose(rlhandle);racinglineset.Initialise();loadRacingLineSet("l_raceline",worldscale);hasraceline=true;
				}
			}
		if (!stricmp(buf, "Medium"))
			{
			if(rlhandle = arcExtract_rlf("m_raceline"))
				{
				detaillevel=1;
				dprintf("Medium detail racing line set found");arcDispose(rlhandle);racinglineset.Initialise();loadRacingLineSet("m_raceline",worldscale);hasraceline=true;
				}
			}
		if (!stricmp(buf, "High"))
			{
			if(rlhandle = arcExtract_rlf("h_raceline"))
				{
				detaillevel=2;
				dprintf("High detail racing line set found");arcDispose(rlhandle);racinglineset.Initialise();loadRacingLineSet("h_raceline",worldscale);hasraceline=true;
				}
			}
#else
	if(rlhandle = arcExtract_rlf("raceline"))
		{
		dprintf("High detail racing line set found");
		arcDispose(rlhandle);
		racinglineset.Initialise();
		loadRacingLineSet("raceline",worldscale);
		hasraceline=true;
		}
#endif


		SetTimeStamp();
		raceglobals.playerlastlap = -1;

		positiontable.Initialise(rlfirst,rlmain,rllast,game->laps);
		boris2camera->setZbackplane(zback);
#if 0
		GetRegistryValue(REGISTRY_DIRECTORY,"ForceFeedback",buf);
#endif		
		int force_feedback_level = 2;

		if(!strcmp(buf,"low"))
			force_feedback_level = 1;
		if(!strcmp(buf,"off"))
			force_feedback_level = 0;
		if(!strcmp(buf,"none"))
			force_feedback_level = 0;

		PaperFlag = false;
		// CONSTRUCT THE CARS
#if 0
		GetRegistryValue(REGISTRY_DIRECTORY,"ShadowsSwitch",buf);
#endif
		load_shadows = strcmp(buf,"Off");


		NoPlayers = true;
		bool use_ff = false;

		for(i=0;i<game->numcars;i++)
		{
			dprintf("WinMain: making car %d",i);
			Controller *con;

			slowcount[i]=0;
			carsounds.LoadParameterSet(i,game->cars[i].carname);

			sprintf(zzz,"archives\\%s.tc",game->cars[i].carname);
			cararray[i] = new car( zzz, 1.0,  exhaustFX,i);
			dprintf("WinMain: car %s made",game->cars[i].carname);

#if DEMO
			if(!(cararray[i]->bodyObject->modelData->hasTexture("MERCBenz125X_a0a.3df") ||
				cararray[i]->bodyObject->modelData->hasTexture("MBexhauPIP2_a0a.3df") ))
				fatalError("unable to use this car in demo");
#endif

			// edit following lines to position cars correctly

			vector3 sp;

			char RegKey[30];
			sprintf(RegKey, "QualifiedPosition%d", i);
#if 0
			GetRegistryValue(REGISTRY_DIRECTORY, RegKey, buf);
			int Position = atoi(buf)-1;
			if (Position >= game->numcars) Position = game->numcars-1;
#else
			int Position = i;
#endif
			sp = startposarray[Position];
/*			
			// If we're the player car, we need a qualification position.
			if (i == 0 || i == SwappedCar)
			{
				sp = startposarray[i==SwappedCar ? 0 : SwappedCar];
			}
			else
				sp = startposarray[i];
*/	
			dprintf("WinMain; setting car pos to %f,%f,%f",
				sp.x,sp.y,sp.z);
			dprintf("car %d is in position %d, position %d", i, atoi(buf),Position);

			cararray[i]->setBodyPosition(&sp);

			cararray[i]->setBodyAngle( &vector3(0, startingyangle, 0) );
			cararray[i]->yRotationAngle = startingyangle;
			dprintf("WinMain: car angle set");

			
			
			cararray[i]->insertIntoWorld();
			dprintf("WinMain: car in world");
			cararray[i]->setviewOutcar();
			dprintf("WinMain: view set");
			cararray[i]->plonkOnGround();
			dprintf("WinMain: car plonked");



			if(game->flags & GSF_REPLAY)
			{
				dprintf("WinMain: replay game - making replay controller");
				con = (Controller *)(new ReplayController());
				((ReplayController*)con)->id = i;

				if(game->cars[i].isplayer)
				{
					dprintf("WinMain: car is player");
						cameracar=playercar=cararray[i];

					SetCameraView();
					NoPlayers = false;
				}
			}
			else
			{
				if(game->cars[i].isplayer)
				{
					dprintf("WinMain: car is player");
					con = (Controller *)(new LocalController((char *)&local_controller_keybuffer));
					use_ff = true;

					// Read the registry to find out if we need to use the automatic gearbox.
#if 0
					GetRegistryValue(REGISTRY_DIRECTORY, "GearBox", buf);
#endif
					if (!strcmp(buf, "AUTOMATIC"))
						((LocalController *)con)->automatic=true;
					dprintf("WinMain: local controller made");
					cameracar=playercar=cararray[i];

					SetCameraView();
					NoPlayers = false;
				}
				else
				{
					con = (Controller *)(new AIController());
					dprintf("WinMain: ai controller made");

				}
			}

			con->Attach(cararray[i]);
			dprintf("WinMain: controller attached");

			// set up a replay buffer for the controller
			replay *replbuf = replaybuffermanager.bufs[i];
			if(!replbuf)fatalError("cannot allocate a replay buffer");
			dprintf("WinMain: replay buffer allocated");
			con->SetReplayBuffer(replbuf);
			dprintf("WinMain: replay buffer attached");
		}


		if (NoPlayers)
		{
			if (cararray[1])
				cameracar = cararray[1];
			else
				cameracar = cararray[0];
			AllCameras->UseTrackside(true, true);
			AllCameras->UsingTrackside =true;
			SetCameraView();

			force_feedback_level=0;
		}

		// initialise the controller setup. returns true if no force feedback.

		if(InitialiseInputs())use_ff = false;
		

		if((game->flags & GSF_REPLAY) || !use_ff)force_feedback_level=0;

		InitialiseDirectInput(hInstance,force_feedback_level);
		dprintf("force feedback %d",force_feedback_level);

		carsounds.SetParameters(cameracar);

		dprintf("WinMain: all cars made");

		dprintf("Making pitstops.");
		AllPitstopGraphics = new PitstopGraphics(".\\Archives\\Pitstop.tc");

			// initialise replay data
		if(game->flags & GSF_REPLAY)
		{
			cameraview=CAM_OUT1;	// just a value..
			LoadReplayControllers(game->numcars);
			dprintf("WinMain: replay buffers initialised and loaded");
		}

		INITIALISEFRAMERATECOUNTER(20);	// initialise the FRC 
		dprintf("WinMain: frc initialised");

		// make the sun!

		boris2world->setSun( &vector3( 0.2f,-0.2f,0.8f ),
			125,125,125,
			255,255,255 );

		dprintf("WinMain: sun made");

		boris2->wframeOff();
//		boris2->fogOff();
		boris2->lightingOff();
		boris2->envmapped2dOn();

		dprintf("WinMain: rendering parameters set");

		arcPop();							// remove the level archive

		dprintf("WinMain: track archive popped, loading overlays");

		overlay.language = reallanguage;


		overlay.Load();

		overlay.SwitchToDash(cameracar->id);

		dprintf("WinMain: overlays loaded");

		boris2mdisplay  = new display(-overlay.curdash->mirrorcam.w,overlay.curdash->mirrorcam.h,  overlay.curdash->mirrorcam.x,overlay.curdash->mirrorcam.y);	
		boris2mdisplay2  = new display(-overlay.curdash->mirrorcam2.w,overlay.curdash->mirrorcam2.h,  overlay.curdash->mirrorcam2.x,overlay.curdash->mirrorcam2.y);	

		initialiseParticles();
#if 0
		GetRegistryValue(NULL, "DriverAids", buf);
#endif
		if (!strcmp(buf, "On"))
			overlay.UseDriverAids = true;
		else overlay.UseDriverAids = false;
		ambsnd=0;


		Paused=false;

		time_stamp_paused = time_stamp_pauseddiff=0;

		//----------------------------------------------------------------------------------
		//
		// GAME LOOP

		race_begun=false;
		if(cameraview == CAM_PITSTOP)
		{
			cameraview = storeview;
		}

		realcameraview = cameraview;

		lod_scaling_counter = 100;	// don't start lod scaling until 20 frames.

		winner_finish_time = 0;
		blackdelay=0;
		raceglobals.state = RACE_BEFORE;
		
		dprintf("WinMain: entering game loop");
		positiontable.Update(game->numcars);
		do_exit_hack = false;

		dprintf("position update done");
		Tick(false,true);	// one tick for luck!
		dprintf("initial tick done");


		// record the time at the start of the game
		QueryPerformanceCounter((LARGE_INTEGER *)&basecounter);
		real_basecounter=basecounter;

		startcount=((rand()%4)+5)*100;

		ticksrun = (__int64)1;

		// set the listener to the attached camera

		carsounds.NominateListener(boris2camera);
		carsounds.SetParameters(cameracar);

		// start the engine sounds

		dprintf("loading engine sounds");
		carsounds.LoadEngineSounds(game->numcars,cararray);

		if(game->flags & GSF_REPLAY)
		{
			AllCameras->UseTrackside(true,true);
			SetDriverVisibility(true);
			carsounds.replaymode = true;
		}
		else
		{
			carsounds.replaymode = false;
			AllCameras->UseTrackside(false, false);	
		}

		positionCamera(cameracar);
		SetCameraView();

		detachmode=lagmode=false;

//		soundsys.PlayStreamBuffer("e:\\sounds\\meadow2.wav");
//		soundsys.StopStream(10000);

		carsounds.LoadAmbient();
		InitialiseSkids();

		memset(&local_controller_keybuffer,0,256);

		bool RacePositionsSet = false;

		SetTimeStamp();
		start_of_race = (long)current_time_stamp;

		for(;;)
		{
			MSG msg;

			Sleep(0);

			int nEvents = soundsys.stream.rghEvent ? SOUND_NUMEVENTS : 0;

			DWORD dwEvt = MsgWaitForMultipleObjects(nEvents,soundsys.stream.rghEvent,FALSE,0,QS_ALLINPUT);

			if(dwEvt!=WAIT_TIMEOUT)
			{
				dwEvt -= WAIT_OBJECT_0;

				if(dwEvt<nEvents)
				{
					soundsys.StreamToBuffer(dwEvt);
				}
				else if(dwEvt==nEvents)
				{

					while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
					{
						if( GetMessage( &msg, NULL, 0, 0 ) )
						{
#if !defined(UNDER_CE)
							if(msg.message != WM_ACTIVATEAPP)
#endif
							{
							TranslateMessage(&msg); 
							DispatchMessage(&msg);
						}
						}
						else
						{
							return msg.wParam;
						}
					}
				}
			}

		// render here, render there - render everywhere :)
		// it's simple - one side to your brain - johnny briggs steps off like joshua!!! 
		// SDS - New stuff to modify the zback value depeding upon the currentcamera's position relative to the track spline
		// *****************************************************************************************************************		
		int campoint;
		rlvector mv;
		racingLine *RacingLine;
		RacingLine	    = racinglineset.getLine(0);
		RacingLine->getNearestRacingLinePointOfAll(0,&boris2->currentcamera->position,&mv,&campoint);

		if (campoint==0) campoint=1;
		if (RacingLine->zinfo[campoint].z2>50)zback=RacingLine->zinfo[campoint].z2;

//frame per second counter

			static long StartTime=timeGetTime();
			static int countl=0;
			static int r[128]={0};
			long OldTime=StartTime;
			StartTime = timeGetTime();
			int fps=0;
			r[countl]=StartTime-OldTime;
			
			long ravg=0;
			for(int abc=0;abc<128;abc++)
				ravg+=r[abc];
			
			if(ravg)
				fps=128000/ravg;
			
#if 1
			if (++countl>127) 
				countl=0;
			if(!(countl&31))
				dprintf("%d fps polys %d, swaps = %d vbufs = %d",fps, polysperframe,texswaps,vbufs);
#endif
			
			// here we modify the lod scaling factor dependent on the frame rate
#define	MAX_LOD_SCALING_FACTOR	3
			
			if(lod_scaling_counter)
				lod_scaling_counter--;
			else
			{
				if(fps < 4)
				{
					boris2->lod_threshold_scale *= 1.1f;
					if(boris2->lod_threshold_scale > MAX_LOD_SCALING_FACTOR)boris2->lod_threshold_scale=MAX_LOD_SCALING_FACTOR;
					//					boris2->currentcamera->setZbackplane(zback * boris2->lod_threshold_scale);
				}
				else if(fps > 30)
				{
					boris2->lod_threshold_scale /= 1.1f;
					if(boris2->lod_threshold_scale < 1/MAX_LOD_SCALING_FACTOR)boris2->lod_threshold_scale=1/MAX_LOD_SCALING_FACTOR;
					//					boris2->currentcamera->setZbackplane(zback * boris2->lod_threshold_scale);
				}
				else
				{
					boris2->lod_threshold_scale=1;
					//					boris2->currentcamera->setZbackplane(zback * boris2->lod_threshold_scale);
				}

				if(raceglobals.state == RACE_BEFORE)boris2->lod_threshold_scale = 1;
			}
			
			
			boris2->currentcamera->setZbackplane(zback,boris2world);
			carsounds.SetExhaustCar(cameracar);
			if(raceglobals.state != RACE_PAPER2)
			{
				boris2->beginFrame( moved, heldlParam );

				overlay.SetJudder();
#if 0
				// Read the "registry" to see if we need the mirrors.
				GetRegistryValue(REGISTRY_DIRECTORY, "RearViewSwitch", buf);
				if (!stricmp(buf, "On"))
				{

					if(cameraview == CAM_INCAR && !rearglance)
					{
						camera *oldcamera = boris2->currentcamera;

						switch(overlay.curdash->mirrortype)
						{
							case 2:
								boris2->setCurrentDisplay(boris2mdisplay);		// associate display to engine
							
								boris2->setCurrentCamera( boris2mirror );		// associate camera to engine
								// render everything
							
								boris2->render();
								if(detaillevel == 2 && overlay.curdash->mirrortype2 == 4)
								{
									boris2->setCurrentDisplay(boris2mdisplay2);		// associate display to engine
								
									boris2->setCurrentCamera( boris2mirror2 );		// associate camera to engine
									// render everything
								
									boris2->render();
								}
								ClearZ();
								EnableZWrite(TRUE);
								int vMirror[8];
								vMirror[0]=overlay.curdash->mirrorpos.x1+overlay.cx; vMirror[1]=overlay.curdash->mirrorpos.y1+overlay.cy;
								vMirror[2]=overlay.curdash->mirrorpos.x2+overlay.cx; vMirror[3]=overlay.curdash->mirrorpos.y2+overlay.cy;
								vMirror[4]=overlay.curdash->mirrorpos.x3+overlay.cx; vMirror[5]=overlay.curdash->mirrorpos.y3+overlay.cy;
								vMirror[6]=overlay.curdash->mirrorpos.x4+overlay.cx; vMirror[7]=overlay.curdash->mirrorpos.y4+overlay.cy;
								MirrorMask(vMirror);
								if(detaillevel == 2 && overlay.curdash->mirrortype2 == 4)
								{
									vMirror[0]=overlay.curdash->mirrorpos2.x1+overlay.cx; vMirror[1]=overlay.curdash->mirrorpos2.y1+overlay.cy;
									vMirror[2]=overlay.curdash->mirrorpos2.x2+overlay.cx; vMirror[3]=overlay.curdash->mirrorpos2.y2+overlay.cy;
									vMirror[4]=overlay.curdash->mirrorpos2.x3+overlay.cx; vMirror[5]=overlay.curdash->mirrorpos2.y3+overlay.cy;
									vMirror[6]=overlay.curdash->mirrorpos2.x4+overlay.cx; vMirror[7]=overlay.curdash->mirrorpos2.y4+overlay.cy;
									MirrorMask(vMirror);
								}
								boris2->setCurrentDisplay(boris2display);		// associate display to engine
								boris2->setCurrentCamera( oldcamera);			// associate camera to engine
								SetCameraView();
								break;
							case 1:
								boris2->setCurrentDisplay(boris2mdisplay);		// associate display to engine
								
								boris2->setCurrentCamera( boris2mirror );		// associate camera to engine
								// render everything
								
								boris2->render();
								
								if(detaillevel == 2 && overlay.curdash->mirrortype2 == 3)
								{
									boris2->setCurrentDisplay(boris2mdisplay2);		// associate display to engine
								
									boris2->setCurrentCamera( boris2mirror2 );		// associate camera to engine
									// render everything
								
									boris2->render();
								}
								ClearZ();
								renderSetCurrentMaterial(&Merc154Mask);
								EnableZWrite(TRUE);

								vert[0].sx=vert[3].sx=overlay.curdash->mirrorpos.x1+overlay.cx;
								vert[1].sx=vert[2].sx=overlay.curdash->mirrorpos.x2+overlay.cx;
								vert[0].sy=vert[1].sy=overlay.curdash->mirrorpos.y1+overlay.cy;
								vert[2].sy=vert[3].sy=overlay.curdash->mirrorpos.y2+overlay.cy;
								renderFan(vert,4);
								if(detaillevel == 2 && overlay.curdash->mirrortype2 == 3)
								{
									vert[0].sx=vert[3].sx=overlay.curdash->mirrorpos2.x1+overlay.cx;
									vert[1].sx=vert[2].sx=overlay.curdash->mirrorpos2.x2+overlay.cx;
									vert[0].sy=vert[1].sy=overlay.curdash->mirrorpos2.y1+overlay.cy;
									vert[2].sy=vert[3].sy=overlay.curdash->mirrorpos2.y2+overlay.cy;
									renderFan(vert,4);
								}
								boris2->setCurrentDisplay(boris2display);		// associate display to engine
								boris2->setCurrentCamera( oldcamera );							// associate camera to engine
								SetCameraView();
						}
					}
				}
#endif

				SetShadowVisibilities();
				rendergrdebug();
				if(kp) {testFont->printf(100+kp,200,ccode);kp--;}
				//grdebug("pits",cameracar->pitstop_on);
				//grdebug("fps",fps);
				extern ulong polygoncounter;
				//grdebug("pols",(int)polygoncounter);
				//grdebug("camera",cameraview);
				//grdebug("replay",(int)replaymemoryused);
#if FRAMEDUMP
				if(!DumpFrames)
#endif
				overlay.Render();

				if (cameracar->CurrentPitstop)
					cameracar->CurrentPitstop->Render((game->flags & GSF_REPLAY) ? false : !Paused);

//				else
#if CARSOUNDPARAMEDITOR
	overlay.DrawDebugWidget(50,100,100,"bassamp",debug_amp_low,0,1);
	overlay.DrawDebugWidget(200,100,100,"midamp",debug_amp_high,0,1);
	overlay.DrawDebugWidget(350,100,100,"topamp",debug_amp_top,0,1);

	overlay.DrawDebugWidget(50,300,100,"bassptc",debug_pitch_low,0,4,1);
	overlay.DrawDebugWidget(200,300,100,"midptc",debug_pitch_high,0,4,1);
	overlay.DrawDebugWidget(350,300,100,"topptc",debug_pitch_top,0,4,1);

	carsounds.ParamEditor(CPEM_DRAW);
#endif

				overlay.showstart=false;	
				overlay.swingflag=false;

				
				if(!Paused && (raceglobals.state == RACE_BEFORE && raceglobals.counter > (startcount-120)))
				{
					overlay.showstart=true;
					//overlay.font4->printf(400,200,"%%c%d",raceglobals.counter);

				}
				if(!Paused && overlay.flagpos<1.0f && raceglobals.state == RACE_DURING)
				{
					overlay.showstart=true;
					overlay.swingflag=true;
					overlay.flagpos+=0.05f;

				}

				ACKNOWLEDGEFRAME();	
				SetTimeStamp();
			
				boris2->setCurrentDisplay(boris2display);		// associate display to engine

				// render everything
				RenderSkids();
#if 0
				drawParticles();
#endif
				boris2->render();

				CheckKeysPressed();	// check global keys, also poll the buffer for the local controller

				// get the control states for each car's controller, ready for the thread to
				// do GetControlPacket.

				for(i=0;i<game->numcars;i++)
				{
					if(!race_begun && positiontable.laps[i]>=0)
					{
						start_of_race = Now();
						race_begun=true;
					}

					if(cararray[i]->shadow_on)
						cararray[i]->mainObject->collisionFlags |= COLLIDE_SHADOW;
					else
						cararray[i]->mainObject->collisionFlags &= (0xffffffff ^ COLLIDE_SHADOW);

					cararray[i]->controller->BuildControlPacket();
//				cararray[i]->updateExhaust();

					if (cararray[i]->CurrentPitstop &&
						cararray[i]->controller->GetControlPacket().accbrake>0.5f &&
						!cararray[i]->CurrentPitstop->tyrechanging)
					{
						cararray[i]->CurrentPitstop->Cancel();
						cameraview=storeview;
						positionCamera(cameracar);
						SetCameraView();
					}


					
				}
			}
			else
			{
#if DEMO
				//overlay.DoPaper(true);
#else
			//	overlay.DoPaper(false);
#endif
			}

			renderEndScene();
			renderFlipSurface();
#if EXIT_STRAIGHT_OUT
			if(do_exit_hack)
			{
				do_exit_hack=false;
				break;
			}
#endif
			// first get the time now, and find out how many physics ticks we need to run.
			// We also use this in the countdown code below; that's why it's all the way up here

			QueryPerformanceCounter((LARGE_INTEGER *)&counter);
			counter -= basecounter;	// how much time has passed since the game start
			tmp64=counter;			// tmp copy for countdown code

			counter /= ticklength;	// how many ticks should have run since game start



			// now for code which deals with the three states of the race. 
			int arkle;
			switch(raceglobals.state)
			{

			case RACE_BEFORE:

				// now work out the countdown time and change state if it drops < 0 

				raceglobals.counter = Now() - start_of_race;
				arkle = startcount-raceglobals.counter;

//				dprintf("Counter : %d   Arkle : %d   Startcount ; %d",raceglobals.counter,arkle,startcount);

				if(arkle <= 0)
				{
					raceglobals.state = RACE_DURING;
					if(playercar->controller->type == CONTROLLER_LOCAL)
					{
						LocalController *lc = (LocalController *)(playercar->controller);
						if(lc->automatic)lc->automatic_gearup=true;
						else lc->automatic_gearup=false;
					}
					start_of_race = (long)current_time_stamp;	// RESETS start of race

				for(i=0;i<game->numcars;i++)
					positiontable.lapbegun[i] = start_of_race;
				}

				SetTimeStamp();

				if(do_exit_hack)
				{
					dprintf("exit caught in before");
					do_exit_hack=false;
					raceglobals.state = RACE_AFTER;
				}
				break;


				break;

			case RACE_DURING:

				CheckFinished();
				if(do_exit_hack)
				{
					do_exit_hack=false;
					raceglobals.state = RACE_AFTER;
				}
				break;

			case RACE_RACEWON:
				// wait for the player to cross over the SF line

				CheckFinished();

				if(positiontable.laps[playercar->id] != raceglobals.playerlastlap)
				{
					raceglobals.state = RACE_AFTER;
				}

				if(do_exit_hack)
				{
					do_exit_hack=false;
					raceglobals.state = RACE_AFTER;
				}
				

				break;

			case RACE_AFTER:
				if (!RacePositionsSet)
				{

					dprintf("entered race after. blowct = %d, engdead = %s fuelct = %d",cameracar->blownct,
						cameracar->engdead ? "t":"f", cameracar->outoffuelct);
#if 0
					for (i=0 ; i<game->numcars;i++)
					{
						char CarText[256], Key[256];
						
						sprintf(CarText, "%d", i);
						sprintf(Key, "Position_%d", positiontable.pos[i]+1);
						SetRegistryValue(REGISTRY_DIRECTORY, Key, CarText);
						
						sprintf(Key, "PositionInRace_%d", i);
						sprintf(CarText, "%d", positiontable.pos[i]+1);
						SetRegistryValue(REGISTRY_DIRECTORY, Key, CarText);
					}
#endif
					RacePositionsSet = true;
				}
//				raceglobals.state = RACE_FINISHED;
				break;
			case RACE_PAPER1:
				if(PaperFlag)
					raceglobals.state = RACE_PAPER2;
				else
					goto out_this_fucking_loop;
				break;
			case RACE_PAPER2:
				break;
			case RACE_FINISHED:
				goto out_this_fucking_loop;
			}

			if(raceglobals.state != RACE_FINISHED)
			{
				// run the physics

				ticksthisframe=0;
#if FRAMEDUMP
				if(DumpFrames)
					ticksrun = counter - 3;		// Run 30 frames per second dump
#endif
				while(ticksrun < counter)	// run that number of ticks
				{
					Tick(ticksrun == counter-1,false);
//					TestParticles();
					ticksrun++;
					ticksthisframe++;
				}
			}
		}			

out_this_fucking_loop:

		dprintf("WinMain: game loop finished, returning to front end");

		// first assemble a possible 'best lap time'

		float foundbest=0;
		bool didplayerfinishlap=false;

		for(i=0;i<game->numcars;i++)
		{
			if(positiontable.lapbest[i])
			{
				if(positiontable.lapbest[i] < foundbest)foundbest=positiontable.lapbest[i];
				break;
			}
		}

		// oops.. none of the other cars have finished either. Cheat horribly.
		if(!foundbest)
		{
			// OK... each RLP is 13m apart
			float estimated_lap_length = (float)(racinglineset.getLine(0)->pointsPerLine) * 13.0f;
			foundbest = estimated_lap_length / 80.0f;	// 80 m/s
			foundbest *= 100;	// hundredths
		}

		// now (in case we need it) work out the finish time of the winner and the last-to-actually-finish.

		unsigned long last_finisher_finish_time;
		int greatest_pos_with_finish = -1,greatest_car_with_finish=-1;

		if(positiontable.lapbest[playercar->id])
			didplayerfinishlap=true;

		for(i=0;i<game->numcars;i++)
		{
			if(positiontable.finishtime[i])
			{
				positiontable.trackpos[i] = 40-positiontable.pos[i];	// we use this to sort with later

				if(positiontable.pos[i] > greatest_pos_with_finish)
				{
					greatest_pos_with_finish = positiontable.pos[i];
					greatest_car_with_finish = i;
				}
			}
		}

		last_finisher_finish_time = positiontable.finishtime[greatest_car_with_finish];

		for(i=0;i<game->numcars;i++)
		{
			unsigned long time_on_this_lap = Now() - positiontable.lapbegun[i];
			car *ccc = cararray[i];

			if(!positiontable.lapbest[i])
			{
				positiontable.lapbest[i] = foundbest * (1.0f + ((float)(positiontable.pos[i]))/1000.0f);
			}

			if(!positiontable.finishtime[i])
			{
				dprintf("car %d not finished yet",i);
				// ah, not finished yet

				dprintf("calculating finish time");

				// should we work out a projected finish time for this car?

				if(ccc->outoffuelct || ccc->engdead)
				{
					// did not finish, grade 1
					positiontable.trackpos[i] = (40-positiontable.pos[i])+100;
				}
				else if(ccc->disqualified)
				{
					// did not finish, grade 1
					positiontable.trackpos[i] = (40-positiontable.pos[i])+1000;
				}
				else if(winner_finish_time)
				{
					// someone finished, and so did this car, so we work out the PFT using the
					// finished car's data

					// we know the the of the last finisher, so we can er.. add stuff to that!

					float remaining_rlps_till_finish = racinglineset.getLine(0)->pointsPerLine;
					remaining_rlps_till_finish *= game->laps;
					dprintf("rlps in race %f, realtrackpos %f",remaining_rlps_till_finish,positiontable.realtrackpos[i]);
					remaining_rlps_till_finish -= positiontable.realtrackpos[i];

					float extra_time = remaining_rlps_till_finish * 13; // assuming 100m/s
					extra_time *= 100;
					extra_time /= 80;

					dprintf("extra time = (remainingrlps (%f) * 13 * 100) / 80 = %f",remaining_rlps_till_finish,extra_time);
					positiontable.finishtime[i] = (last_finisher_finish_time + extra_time);
					dprintf("finish = %lu + %f = %lu",last_finisher_finish_time,extra_time,positiontable.finishtime[i]); 

					positiontable.trackpos[i] = 40-positiontable.pos[i];

				}
				else
				{
					// no-one finished before I quit!

					float remaining_rlps_till_finish = racinglineset.getLine(0)->pointsPerLine;
					remaining_rlps_till_finish *= game->laps;
//					remaining_rlps_till_finish -= game->numcars-positiontable.realtrackpos[i];	// JJS addded

					float extra_time = remaining_rlps_till_finish * 13; // assuming 100m/s
					extra_time *= 100;
					extra_time /= 80;

					dprintf("extra time = (remainingrlps (%f) * 13 * 100) / 80 = %f",remaining_rlps_till_finish,extra_time);
					positiontable.finishtime[i] = (Now () + extra_time) - start_of_race;
					dprintf("finish = %lu + %f - %lu = %lu",Now(),extra_time,start_of_race,positiontable.finishtime[i]); 

					positiontable.trackpos[i] = 40-positiontable.pos[i];
				}
			}
			dprintf("car %d - finished time %ld",i,positiontable.finishtime[i]);

		}

		// now that's done, re-sort the car positions

		positiontable.SortTrackPos(false);

		dprintf("sorted track positions");

		for (i=0 ; i<game->numcars; i++)
		{
			dprintf("Car %d - trackpos %d - finished position %d",i,positiontable.trackpos[i],
				positiontable.pos[i]);

			if(i)
			{
				// find the cars in positions i and i-1

				if(positiontable.finishtime[positiontable.postable[i-1]] >=
					positiontable.finishtime[positiontable.postable[i]])
				{
					positiontable.finishtime[positiontable.postable[i]] =
						positiontable.finishtime[positiontable.postable[i-1]] + (rand()%800);
				}
			}

		}

		if(overlay.qualmode && !didplayerfinishlap)
		{
			positiontable.lapbest[playercar->id]=0;
		}
		


		carsounds.ResetExhaustCar();
		if(!(game->flags & GSF_REPLAY))
			SaveReplayControllers(game->numcars);

		

		// Check if we've done all the laps we need. If not, then we must have quitted,
		// so make us last.
		if (positiontable.laps[0] < GlobalGame.laps)
		{
			for (i=1 ; i<game->numcars ; i++)
			{
				if (positiontable.pos[i] >= positiontable.pos[0])
					positiontable.pos[i]--;
			}
			positiontable.pos[0] = game->numcars-1;
			RacePositionsSet = false;
		}

		// Update race positions.
		char Key[256], CarText[256];
#if 0
		for (i=0 ; i<GlobalGame.numcars ; i++)
		{
			if (!RacePositionsSet)
			{
				sprintf(CarText, "%d", i);
				sprintf(Key, "Position_%d", positiontable.pos[i]+1);
				SetRegistryValue(REGISTRY_DIRECTORY, Key, CarText);
				
				sprintf(Key, "PositionInRace_%d", i);
				sprintf(CarText, "%d", positiontable.pos[i]+1);
				SetRegistryValue(REGISTRY_DIRECTORY, Key, CarText);			
			}

			sprintf(Key, "BestLap_%d", i);
			sprintf(CarText, "%d", positiontable.lapbest[i]);
			SetRegistryValue(REGISTRY_DIRECTORY, Key, CarText);
	
			sprintf(Key, "FinishedTime_%d", i);
			sprintf(CarText, "%d", positiontable.finishtime[i]);
			SetRegistryValue(REGISTRY_DIRECTORY, Key, CarText);

		}
#endif
		RacePositionsSet = true;

		// shutdown the sounds

		carsounds.StopAll();
		StopEffects();

		// remove models and textures

		dprintf("WinMain: removing texture store");
		overlay.Shutdown();
		removeTextureStore();
		dprintf("WinMain: removing model store");
		removeModelStore();
//		removeModelStore7();
		dprintf("deleteFX");
		DeleteFX();

		delete FloatingCamera;
		delete AllCameras;
		delete boris2mirror;
		delete boris2mirror2;
		delete boris2camera;
		for(i=0;i<game->numcars;i++)
		{
			delete cararray[i]->controller;
			delete cararray[i];
		}
		delete boris2sky;
		delete boris2mdisplay;
		delete boris2mdisplay2;
		delete boris2display;
		delete boris2world;
		delete testFont;
		delete boris2;
		dprintf("deinitialise render-3D");
#if !defined(UNDER_CE)
		renderDeInitialise(CLOSE_3D);
#endif
		dprintf("3D shutdown OK");
		// end of loop, return to front end
//		break;

		carsounds.Shutdown();
		shutdownParticles();
		ShutdownDirectInput();
	}
	
	// end of everything!
#if !defined(UNDER_CE)	
	renderDeInitialise(CLOSE_2D);
#endif
	dprintf("WinMain: popping archives");
	arcPop();							// remove the global archive
		  
	// turn the screensaver back on if it was on before.
#if !ALLOWTABBING & !defined(UNDER_CE)
	SystemParametersInfo(SPI_SCREENSAVERRUNNING,FALSE,NULL,0);
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,screen_saver_active,NULL,0);
#endif
	return(0);
}

#if defined(UNDER_CE)
void FatalError(char *s,...)
{
}
void fatalError(char *s,...)
{
}
void NonFatalError(char *s,...)
{
}
#endif
