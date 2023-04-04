
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "BS2all.h"
#include "ALLcar.h"
#include "PositionTable.h"
#include "controller.h"
#include "FrontEnd.h"
//#include "main.cpp"
//#include "racingLine.cpp"

extern car *cararray[];
extern gamesetup *game;
extern struct raceglobalstruct raceglobals;
extern PositionTable positiontable;
extern void dprintf(char *,...);
extern car *playercar;
extern bool PauseHack;

#define ssToTheFloor    0x01 // Normal mode - as fast as possible
#define ssParkAndRide   0x02 // Stuck - try a \ point turn :)
#define ssStopTheBus    0x0  // Slam on the brakes
#define ssIntoThePit    0x04 // lets try and drive into the pits
#define ssWaitForIt	    0x05
#define ssOuttaHere	    0x06		
#define ssStage1	    0x01 // Sub stage 1 for a three point turn
#define ssStage2	    0x02 // Sub stage 2 for a three point turn
#define ssStage3        0x03 // Sub stage 3 for a three point turn
#define ssStage1A	    0x04 // Sub stage 1 for a three point turn
#define ssStage2A	    0x05 // Sub stage 2 for a three point turn
#define ssStage3A       0x06 // Sub stage 3 for a three point turn
#define ssqr(x)		   (x*x)
#define ssReturnToTrack 0x08 // Smoothing for return to track spline
#define ssWaitForTrack  0x09 // Return The Car to The Track
#define ssInUse 0x01;
#define ssFree  0x02;
#define pitconst 20 		// The percentage constant used to decide when to take a pitstop
#define AIHintOvertake -1
#define TriggerPoint 172
#define or ||
#define and &&

float CarAngs[20];
float CarOffsets[20];
float carpospos[20];
float PitsUsed[20];  // The pit Stops that are being used
int	  carpospoint[20];
int	  guid;
float pitloc[3][2];
float dvt[20];
int   CarType[20];
int   OverTake[20]   = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int   strand[20]     = {10,20,30,40,50,60,70,80,90,100,110,120,0,0,0,0,0,0,0,0};
float prepos[20];
char StartAccl[20] = {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127};


float BrookFix[30][2]={
413.813f, -274.633f, 422.441f, -272.075f, 431.072f, -269.522f, 
439.702f, -266.969f, 448.318f, -264.367f, 456.938f, -261.780f, 
465.558f, -259.193f, 474.127f, -256.439f, 482.718f, -253.756f,  
491.251f, -250.896f, 499.786f, -248.038f, 508.320f, -245.180f, 
516.802f, -242.170f, 525.296f, -239.195f, 533.790f, -236.220f,  
542.251f, -233.152f, 550.724f, -230.118f, 559.150f, -226.952f,
567.596f, -223.843f, 575.930f, -220.441f, 584.215f, -216.923f,  
592.514f, -213.441f, 600.636f, -209.557f, 608.806f, -205.780f,
616.697f, -201.437f, 624.690f, -197.299f, 632.295f, -192.458f,  
640.069f, -187.923f, 647.353f, -182.596f, 654.550f, -177.187f}; 

class ctrlpckt AIController::GetControlPacket()
{

	vector3 CLoc,LTC,LCP,CarLocation, Destination, CarVelocity, TargetPoint, TargetNextPoint, TargetPrevPoint;
	vector3 CarInFrontLocation, ClosestPoint, ThirdPoint, FirstPoint, BodyDirection;
	vector3 Colp,Le1,Le2,Re1,Re2; // SDS - Left and right edges
	float CurrentVelocity;		
	float OvertakeDelta;
	float DistanceToCarInFront=10000.0f;
	float LCIFV = 300;
	float lcifd = 10000;
	float SteerMult;
 	int   CarID=999;
	int   carPos;	
	int   Revs;
	int   CurrentPoint,CarInFrontPos;	
	int   NumPoints;
	int   Stay;
	class car *CarInFront;
	unsigned char keys[256];


	// - C A U T I O N - \\
	// This line *must* be commented out in the final build - don't forget to do this
	
	//GetKeyboardState(keys); if (keys[66]>10) {MaxSpeed=14;} else MaxSpeed=200;

	ctrlpckt Packet;
	CarID           = car->id;
	MaxSpeed=200.0f;
	//MaxSpeed=15.0f;
	if (car->engdead || car->outoffuelct) { Packet.flags=0; Packet.accbrake=-127; Record(Packet);	return Packet;}

	if (raceglobals.state==RACE_BEFORE)
		{
  			Packet.flags=CF_GEARUP;
			Packet.accbrake=-127.0f;
		}
	else
	{
	//=================================================================================================================================
	// (1) First Time Code
	//=================================================================================================================================
	if (FirstTime==NULL)
		{
		FirstTime = (char*)1;
		if (strncmp(game->cars[car->id].carname,"AlfaP3",5)==0)   	{CarSkidVelocity=45.0f; BrakePointMult=1.0f;}
		if (strncmp(game->cars[car->id].carname,"Alfa12C",5)==0) 	    {CarSkidVelocity=40.0f; BrakePointMult=1.0f;}
		if (strncmp(game->cars[car->id].carname,"AlfaBimotore",5)==0) {CarSkidVelocity=40.0f; BrakePointMult=0.5f;}
		if (strncmp(game->cars[car->id].carname,"AutoUnionC",10)==0) 	{CarSkidVelocity=60.0f; BrakePointMult=0.85f;}
		if (strncmp(game->cars[car->id].carname,"AutoUnionD",10)==0) 	{CarSkidVelocity=45.0f; BrakePointMult=0.95f;}
		if (strncmp(game->cars[car->id].carname,"Bugatti35",9)==0) 	{CarSkidVelocity=45.0f; BrakePointMult=1.0f;}
		if (strncmp(game->cars[car->id].carname,"Bugatti59",9)==0) 	{CarSkidVelocity=40.0f; BrakePointMult=1.0f;}
		if (strncmp(game->cars[car->id].carname,"Dusenberg",9)==0) 	{CarSkidVelocity=40.0f; BrakePointMult=1.0f;}
		if (strncmp(game->cars[car->id].carname,"ERAremus",8)==0) 	{CarSkidVelocity=45.0f; BrakePointMult=1.0f;}
		if (strncmp(game->cars[car->id].carname,"Maserati8cm",9)==0) 	{CarSkidVelocity=45.0f; BrakePointMult=1.0f;}
		if (strncmp(game->cars[car->id].carname,"Maserativ8ri",9)==0)	{CarSkidVelocity=45.0f; BrakePointMult=1.0f;}
		if (strncmp(game->cars[car->id].carname,"Mercedes125",9)==0) 	{CarSkidVelocity=50.0f; BrakePointMult=1.0f;}
		if (strncmp(game->cars[car->id].carname,"Mercedes154",9)==0) 	{CarSkidVelocity=45.0f; BrakePointMult=1.0f;}
		if (strncmp(game->cars[car->id].carname,"Miller",6)==0) 		{CarSkidVelocity=45.0f; BrakePointMult=1.0f;}
		if (strncmp(game->cars[car->id].carname,"NapierRailton",13)==0){CarSkidVelocity=55.0f; BrakePointMult=0.8f;}
		if (!((car->id+StartMod) & 1)) {lastcardir=StartOffset; overtake=480;}
		Packet.flags=CF_GEARUP;
		Packet.accbrake=0.0f;
		}
	else
	{
	racingLine *RacingLine;
	RacingLine	    = racinglineset.getLine(brookhack);
	NumPoints		= RacingLine->pointsPerLine;	

	CarID           = car->id;
	CarLocation     = car->mainObject->concatenatedTransform.translation;
	carPos          = positiontable.pos[CarID];
	CurrentPoint	= positiontable.rlp1[CarID];
	CurrentPoint    = RacingLine->increment(CurrentPoint, 1, positiontable.subline[CarID]);


	if (NumPoints==475 && CurrentPoint>55 && CurrentPoint<65 && brookhack==0) {brookhack=1;}
	if (positiontable.laps[CarID]+1==positiontable.nlaps && NumPoints==475 && CurrentPoint>380 && CurrentPoint<390 && brookhack==1) {brookhack=0;}

	//=================================================================================================================================
	// (2) Calculate the Car Offsets
	//=================================================================================================================================
	// This loop only needs to be called once per tick - this little test should ensure that

if (CarID==1) 
	{
	for (int i=0;i<game->numcars;i++)
		{
		int cpoint = positiontable.rlp1[i];
		CLoc	    = cararray[i]->mainObject->concatenatedTransform.translation;
		Le1.x	    = (RacingLine->racingLinePoints[(cpoint)].ex1   + RacingLine->racingLinePoints[(cpoint)].ex2   )/2;     
		Le1.z	    = (RacingLine->racingLinePoints[(cpoint)].ez1   + RacingLine->racingLinePoints[(cpoint)].ez2   )/2;
		Re1.x	    = (RacingLine->racingLinePoints[(cpoint+1)].ex1 + RacingLine->racingLinePoints[(cpoint+1)].ex2 )/2;     
		Re1.z	    = (RacingLine->racingLinePoints[(cpoint+1)].ez1 + RacingLine->racingLinePoints[(cpoint+1)].ez2 )/2;
		float gdx1  = (Re1.x-Le1.x);
		float gdy1  = (Re1.x-Le1.x);
		float gdx2  = (Le1.x-CLoc.x);
		float gdy2  = (Le1.z-CLoc.z);
		float gr1,gr2;
		if (gdy1==0) gr1=0; else gr1=gdx1/gdy1;
		if (gdy2==0) gr2=0; else gr2=gdx2/gdy2;
		float hyp   = sqrt(((CLoc.x-Le1.x)*(CLoc.x-Le1.x))+((CLoc.z-Le1.z)*(CLoc.z-Le1.z)));
		float ang   = (atan2(gdx2,gdy2))-(atan2(gdx1,gdy1));
		float dx    = (Re1.x-Le1.x);
		float dy    = (Re1.z-Le1.z);
		float norm  = sqrt((dx*dx)+(dy*dy));
		dx=dx/norm;
		dy=dy/norm;
		float ad    = ((hyp*cos(ang)));
		float npx   = Le1.x+(-dx*ad);
		float npy   = Le1.z+(-dy*ad);
		prepos[i]   = sqrt(((Re1.x-npx)*(Re1.x-npx))+((Re1.z-npy)*(Re1.z-npy)));
		float Ndist = sqrt(((CLoc.x-npx)*(CLoc.x-npx))+((CLoc.z-npy)*(CLoc.z-npy)));
		float ax    = CLoc.x-npx;
		float az	= CLoc.z-npy;
		float bx	= npx-Re1.x;
		float bz	= npy-Re1.z;
		float sign  = ((az*bx)-(az*bz));
		if (sign>=0) Ndist=-Ndist;
		CarOffsets[i]=Ndist;
		}
	}

	// 4 sqrts 2 atan2's 10 mults 2 divides 27 addtions 1 cos

	TurnOffset=0;
	CurrentVelocity=car->velocity.Mag()*2.25f;

	if (State==ssToTheFloor or State==ssIntoThePit)
		{
			vector3 zdir = vector3 (0.0f,0.0f,1.0f);
			car->mainObject->transform.multiplyV3By3x3mat(&BodyDirection, &zdir);
		}

	//=================================================================================================================================
	// (3) Is there a car in the way
	//=================================================================================================================================

		int ccn=-1;
		float cmd=1000000.0f;
		float no=0;
		float cifv=1000;
		int nop=-1;

		for (int ocp=0;ocp<game->numcars;ocp++)
			{
				{
					{
					nop=ocp;
					no=CarOffsets[ocp];
					int carpos=positiontable.rlp1[ocp];
					int mypos=positiontable.rlp1[CarID];
					int oldcarpos=carpos;
					int oldmypos=mypos;

					if (mypos>(carpos+(NumPoints/2))) carpos+=NumPoints;
					if (carpos>(mypos+(NumPoints/2))) mypos+=NumPoints;
						{
							CarInFront           = cararray[ocp];
							CarInFrontLocation	 = CarInFront->mainObject->concatenatedTransform.translation;
							vector3 CIP			 = RacingLine->racingLinePoints[oldmypos];
							vector3 CIP2		 = RacingLine->racingLinePoints[oldcarpos];
							float dcftp			 = (CIP2-CarInFrontLocation).Mag();
							float Mdtp			 = (CIP-CarLocation).Mag();
							float md=-(((mypos*100)+Mdtp)-((carpos*100)+dcftp));
							carpospos[ocp]=((carpos*100)+prepos[ocp]);  //((carpos*100)+dcftp)
							carpospoint[ocp]=oldcarpos;
							if (md<cmd && md>0)
								{
									cmd=md;
									ccn=ocp;
									cifv=CarInFront->velocity.Mag()*2.25f;
									OvertakeDelta=CurrentVelocity-cifv;
									lcifd = (CarInFrontLocation - CarLocation).Mag();
								}
						}

					}
				}
			}	

	//=================================================================================================================================
	// (4) Should I Overtake
	//=================================================================================================================================


if (overtake==0)
	{
	if (CurrentVelocity>cifv)
	{
		 if ((CarOffsets[ccn]<(CarOffsets[CarID]+carwidth)) and (CarOffsets[ccn]>(CarOffsets[CarID]-carwidth)))
			{
			bool doit=false;
			if (OvertakeDelta>40)
				{
				if (lcifd<(OvertakeDelta/1))
					{
					doit=true;
					}
				}
			// Slow Overtake
			if (OvertakeDelta<=40)
				{
				if (lcifd<20)
					{
					doit=true;
					}
				}
				
			if (doit==true)
					{
				MaxSpeed=200.0f;
							{
							if (CarOffsets[ccn]<=0)
								{
									lastcardir=-carwidth;
									LastCarIndex=ccn;
									TurnOffset=-carwidth; 
									overtake=60;
									LastOffset=TurnOffset;
								}
								else 
								{
									lastcardir=+carwidth;
									LastCarIndex=ccn;
									TurnOffset=+carwidth;
									overtake=60;
									LastOffset=TurnOffset;
								}
							}

			if (ccn>-1 && ccn<game->numcars)
			{
			if (OverTake[ccn]>1) 
				{
				overtake=0;
				MaxSpeed=cifv;

				}
			}
		}
							 
			}
		}	
	}

	//=================================================================================================================================
	// (5) Should I Return
	//=================================================================================================================================

	bool noover=false;

	if (overtake!=0) 
		{
		overtake--; 
		TurnOffset=LastOffset;
		TurnOffset=lastcardir;
		guid++;
		if (overtake==0)
			{
			for (int i=0;i<game->numcars;i++)
				{
				if (i!=CarID)
					{
					if ((abs(carpospos[i]-carpospos[CarID]))<120)
						{
						noover=true;
						}
					}
				}
			}
		}

	if (noover) overtake=120;

	//=================================================================================================================================
	// (6) No Overtaking Allowed
	//=================================================================================================================================

			
		if (RacingLine->racingLinePoints[CurrentPoint].speed!=AIHintOvertake) 
			{
			if (overtake!=0)
				{
				TurnOffset=0.0f;
				if (lcifd<30) MaxSpeed=cifv-1;
				guid++;
				overtake=0;
				}
			}

	//=================================================================================================================================
	// (6a) Anti Humping!! 
	//=================================================================================================================================
		
		if (car->AIcarcollided) {guid++;  SlowEmDown=120;}
		
	//=================================================================================================================================
	// (7) Navigation System
	//=================================================================================================================================

		if (abs(CurrentPoint-LastPoint)>5 && LastPoint<=NumPoints-5) CurrentPoint=LastPoint;
		LastPoint=CurrentPoint;
		MyPoint = CurrentPoint; // SDS - Lets store this for later on
		//ALP = ( MyPoint + NumPoints-1)% NumPoints;
		int MyNextPoint  = (CurrentPoint+1) % NumPoints;
		int MyNNextPoint = (CurrentPoint+2) % NumPoints;
		if(MyPoint < 10) BrakeHelpPointer = 0;  // Fix to help the brake helpers!!
		//float TD=0.0f;
		int SLH=(int)(CurrentVelocity)/TLBD;
/*		LCP.x = RacingLine->racingLinePoints[(CurrentPoint+SLH) % NumPoints].x;
		LCP.z = RacingLine->racingLinePoints[(CurrentPoint+SLH) % NumPoints].z;
		LTC.x = 0;
		LTC.z = 0;

		for (int k=SLH+1;k<(SLH+5);k++)
			{
			int XSS          = (CurrentPoint+k) % NumPoints;
			TargetPoint.x    = RacingLine->racingLinePoints[XSS].x;
			TargetPoint.y    = 0;									
			TargetPoint.z    = RacingLine->racingLinePoints[XSS].z;
			TargetVelocity   = TargetPoint - LCP;
			TargetVelocity.Normalise();
			LCP              = TargetPoint;
			float TurnAmount = TargetVelocity.z * LTC.x - TargetVelocity.x * LTC.z;
			TD              += TurnAmount*(1-fabs(RacingLine->racingLinePoints[XSS].speed));
			LTC              = TargetVelocity;
			}
*/
//		if (fabs(TD)>TLS) {SlowEmDown+=1;}
//
//		LDT=TD;
//		lastTD=fabs(TD);

		float Direction;
		float Turncoff=(200-car->velocity.Mag());

		SLH=(int)(CurrentVelocity)/SLHD; // Turn ahead based on car velocity
		int ReallyOldCurrentPoint;
		ReallyOldCurrentPoint=CurrentPoint;
		CurrentPoint=(CurrentPoint+SLH+tul) % NumPoints; // Look ahead based on speed
		ReallyOldCurrentPoint=CurrentPoint;

		Le1.x = RacingLine->racingLinePoints[(CurrentPoint)].ex1;     
		Le1.z = RacingLine->racingLinePoints[(CurrentPoint)].ez1;
		Re1.x = RacingLine->racingLinePoints[(CurrentPoint)].ex2;	  
		Re1.z = RacingLine->racingLinePoints[(CurrentPoint)].ez2;

		vector3 ModVec = vector3 (Re1.x-Le1.x,0,Re1.z-Le1.z);
		ModVec.Normalise(); // Urghh!!7
		TargetPoint    = RacingLine->racingLinePoints[CurrentPoint];
		vector3 ntp = RacingLine->racingLinePoints[CurrentPoint+1];

		ntp=ntp-TargetPoint;

		ntp.Normalise();
		// - C A U T I O N - \\
		// This line *must* be commented out in the final build - don't forget to do this

		//if (keys[66]>50) { car->setBodyPosition(&TargetPoint); car->setBodyAngle(&ntp);}
		//if (keys[67]>50) { car->setBodyPosition(&TargetPoint); car->setBodyAngle(&ntp);}

	if (car->wpolytype[0]==MATTYPE_GRAVEL || car->wpolytype[1]==MATTYPE_GRAVEL || car->wpolytype[2]==MATTYPE_GRAVEL || car->wpolytype[3]==MATTYPE_GRAVEL)
			{
			WheelCount++;	 } else WheelCount=0;

		SteerMult=1;
		if (WheelCount>75) { SteerMult=WheelCount/60; TurnOffset=0;}

		//TurnOffset=0;
		TargetPoint.x     = TargetPoint.x-(ModVec.x*TurnOffset);
		TargetPoint.z     = TargetPoint.z-(ModVec.z*TurnOffset);
		TargetVelocity    = TargetPoint - CarLocation; 

		TargetVelocity.Normalise();
		BodyDirection.Normalise();
		Direction         = TargetVelocity.z * BodyDirection.x - TargetVelocity.x * BodyDirection.z;

		int TempDir = Direction * Turncoff;
		if (TempDir>127) TempDir=127;
		if (TempDir<-127) TempDir=-127;

		// Must put in some code to handle the 2 wheels off the track problem
		//"""""""""""""""""""	""""""""""""""""""""""""""""""""""""""""""""""""
		//if (car->wpolytype[0]==MATTYPE_SHORTGRASS && car->wpolytype[1]!=MATTYPE_SHORTGRASS && car->wpolytype[2]==MATTYPE_SHORTGRASS && car->wpolytype[3]!=MATTYPE_SHORTGRASS)
		
		//if (car->wpolytype[0]==car->wpolytype[2] && car->wpolytype[1]==car->wpolytype[3] && car->wpolytype[0]!=car->wpolytype[1])

		Packet.steer      = TempDir*SteerMult;
		Packet.accbrake   = 127.0f;

//		if (lastTD<TD)                          { SlowEmDown=0; } // If we're coming out of a bend start to accelerate now!!
		//if (SlowEmDown>100)                     { SlowEmDown=100; }
		//if (SlowEmDown>3 && CurrentVelocity>75) { Packet.accbrake = TurnBrake ; SlowEmDown-=BSS; }

		if (Breaking>0 && car->curgear>2)       { Breaking--; Packet.accbrake=-50.0f; } 

		Revs = car->revs;

		if(++lastspinctr>CarSkidVelocity)
			{
			if (Revs>(car->uprevlim[car->curgear]))                     { lastspinctr=0;Packet.flags = CF_GEARUP; }
			if (car->curgear>2 and Revs<car->lorevlim[car->curgear])    { lastspinctr=0;Packet.flags = CF_GEARDN; }
			}

	if (car->wskid[2]>0.5f || car->wskid[3]>0.5f)
		{
			if (CurrentVelocity>CarSkidVelocity)
				{
				Packet.steer = 0;
				Packet.accbrake=0.0f;

				}
				else 
				if(car->curgear==2)
				{
				Packet.flags=0;
				}
		}
	
	if (RacingLine->numBrakes>0)
		{
		if (MyPoint>RacingLine->brakePoints[BrakeHelpPointer].point) 
			{
			BrakeOverride=RacingLine->brakePoints[BrakeHelpPointer].speed*BrakePointMult;
			BrakeHelpPointer++;
			}
		}


	if (CurrentVelocity<40) SlowEmDown=1;

	//=================================================================================================================================
	// (8) Park and Ride Detection
	//=================================================================================================================================

	if (LastTrackPoint==CurrentPoint) ParkTick++; else ParkTick=0;
	LastTrackPoint=CurrentPoint;


	if (TCounter>900 && ParkTick>90 && State==ssToTheFloor) 
		{
			TargetPoint       = RacingLine->racingLinePoints[LastPoint+3];
			TargetVelocity    = TargetPoint - CarLocation; 
			TargetVelocity.Normalise();
			RRS				  = -TargetVelocity.z * BodyDirection.x - TargetVelocity.x * BodyDirection.z;		
			RRS= -Packet.steer;
			State			  = ssParkAndRide; 
			PState = ssStage1;
			PARcount		  = 0;
		}
	
	
	//=================================================================================================================================
	// (9) Pitstop Code
	//=================================================================================================================================
	//
	// Handle fuel / tire wear e.t.c.................
	// Parameters for pitstop decision
	//
	// Fuel       - percentage  10% = 1 lap around montlhery
	// Oil		  - percentage  ?
	// Temp		  - percentage	>110 =  blown engine
	// TyresFront - percentage   <40 change
	// Tyresback  - percentage   <40 change

	// Here's the decision module :)

	// Gotta find out how much fuel used in 1 lap......
	// Maybe we should calculate the fuel used per lap

	if (positiontable.laps[CarID]==1 && LastLap==0) {FuelPerLap=100-car->fuel;}


	LastLap=positiontable.laps[CarID];
	
	// This puts the cars in the pits after they've finished the race...
	Stay=0;

	if (positiontable.laps[CarID]>=positiontable.nlaps) {pitsub=ssIntoThePit; Stay=1;}

	if (pitsub!=ssIntoThePit)
	{
		// NEED TO ADD SUM CLEVER STUFF TO HANDLE THE LAST LAP SITUATION
		if (car->fuel       < (FuelPerLap+(FuelPerLap/10))) { pitsub=ssIntoThePit; }
		if (car->oil        < pitconst) { pitsub=ssIntoThePit; }
		if (car->tyresfront < pitconst) { pitsub=ssIntoThePit; }
		if (car->tyresback  < pitconst) { pitsub=ssIntoThePit; }
		// And here is the so called "CLEVER" code
		// Needs to be alot more clever than this
		//
		//if (positiontable.laps[CarID]==positiontable.nlaps-2 && CurrentPoint>=NumPoints-200) pitsub=ssToTheFloor;
		//if (positiontable.laps[CarID]==positiontable.nlaps-1 ) pitsub=ssToTheFloor;
		// Make the cars go into the pits....


		// Do I Have Enought Fuel for 3 laps? if not go for a pitstop

	//	if (positiontable.laps[CarID]==positiontable.nlaps-4 ) pitsub=ssIntoThePit;
	//	if (car->fuel>(3*FuelPerLap) && positiontable.laps[CarID]>positiontable.nlaps-4) pitsub==ssToTheFloor;
	}

	if (pitsub==ssIntoThePit)
	{
	if (MyPoint>=(pitstart) && MyPoint<=(pitstart+5))
		{
		State=ssIntoThePit;
		}

	if (MyPoint>=(pitstart-30) && MyPoint<=(pitstart+5))
		{
		if (CurrentVelocity>pitslowdown)	Packet.accbrake=-127.0f;
		}	
	}

	if (State==ssIntoThePit) 
		{
		guid++;
		if (car->fuel<0.5f) car->fuel=0.5f;
		if (CurrentVelocity>pitslowdown) Packet.accbrake=-127.0f; else Packet.accbrake=127;

		TargetPoint.x     = pitloc[pitpos][0];
		TargetPoint.z     = pitloc[pitpos][1];
		TargetVelocity    = TargetPoint - CarLocation; 

		TargetVelocity.Normalise();
		BodyDirection.Normalise();

		float Direction         = (TargetVelocity.z * BodyDirection.x - TargetVelocity.x * BodyDirection.z)*200;

		if (Direction>127)  Direction=127;
		if (Direction<-127) Direction=-127;


		Packet.steer=Direction;
		float Pdist = sqrt(((CarLocation.x-TargetPoint.x)*(CarLocation.x-TargetPoint.x))+((CarLocation.z-TargetPoint.z)*(CarLocation.z-TargetPoint.z)));
		if (Pdist<15)	{
						Packet.accbrake=-127;
						pitpos =(pitpos+1);

						if (pitpos>=pitcount) 
							{
							pitpos=0;
							State = ssToTheFloor;
							}
						if (pitpos==2) 	
							{ 
								PitCounter=0; State=ssWaitForIt;
								pitstaytime=0;
								// Fill up the car !!
								if (car->fuel       < 60)   { car->fuel=99;       pitstaytime+=7*60;  }
								if (car->oil        < 60)   { car->oil=99;        pitstaytime+=7*60;  }
								if (car->tyresfront < 60)   { car->tyresfront=99; pitstaytime+=14*60; }
								if (car->tyresback  < 60)   { car->tyresback=99;  pitstaytime+=14*60; }
								/*
								car->fuel=99;
								car->oil=99;
								car->tyresback=99;
								car->tyresfront=99;
								*/
								if (pitstaytime==0) {car->fuel=99; pitstaytime+=7*60; } 
							}
						if (pitpos>=3)	{State=ssToTheFloor; pitsub=ssToTheFloor; PitsUsed[CarID]=0;}  // Lets Get out
						//LastPitPos=car->mainObject->concatenatedTransform.translation;
						}
		

		}

	// New hack to make the cars stay in the pit after the race is over.......

	if (State==ssWaitForIt)
		{
		if (CurrentVelocity>5) LastPitPos=car->mainObject->concatenatedTransform.translation; else	car->setBodyPosition(&LastPitPos);
		car->outoffuelct=0;
		
		guid++;
		PitCounter++;
		if (PitCounter>(pitstaytime)) { State=ssIntoThePit;	pitsub=ssToTheFloor; }
		Packet.steer=0;
		Packet.accbrake=-127;
		}



	//=================================================================================================================================
	// (10) Park and Ride 
	//=================================================================================================================================

	if (State==ssWaitForTrack)
		{

		if (car->wpolytype[0]!=MATTYPE_SHORTGRASS && car->wpolytype[1]!=MATTYPE_SHORTGRASS && car->wpolytype[2]!=MATTYPE_SHORTGRASS && car->wpolytype[3]!=MATTYPE_SHORTGRASS)
		{
			State=ssToTheFloor;
		}
		else
		{
			Packet.steer = 0;
			Packet.accbrake = 60.0f;
		}

		}

	if (State==ssParkAndRide)
		{

			CurrentPoint	  = positiontable.rlp1[CarID];
			CurrentPoint      = RacingLine->increment(CurrentPoint, 1, positiontable.subline[CarID]);
			TargetPoint       = RacingLine->racingLinePoints[(CurrentPoint+3) % NumPoints];  // Doh - forgot to mod this addition!!
			CarLocation       = car->mainObject->concatenatedTransform.translation;
			TargetVelocity    = TargetPoint - CarLocation; 
			TargetVelocity.Normalise();
			vector3 zdir = vector3 (0.0f,0.0f,1.0f);
			car->mainObject->transform.multiplyV3By3x3mat(&BodyDirection, &zdir);
			BodyDirection.Normalise();
			
			RRS	 = TargetVelocity.z * BodyDirection.x - TargetVelocity.x * BodyDirection.z;		
			RRS *= 100;

			if (RRS>127) RRS=127;
			if (RRS<-127) RRS=-127;

			CurrentVelocity=car->velocity.Mag()*2.25f;

			// Check for the 90 degree problem
			//********************************
			
			if (TargetVelocity.DotProduct(&BodyDirection)<0)
				{
				if (RRS>0) {RRS=127;} else {RRS=-127;}
				}
		//
		if (PState==ssStage2A)
			{
			Packet.steer=RRS;
			Packet.accbrake=-127;
			if (CurrentVelocity<5) PState = ssStage1;
			PARcount=0;
			}
		// Lets Reverse for a bit..............
		if (PState==ssStage1)
			{
				Packet.flags     = 0;
				Packet.steer     = -RRS;
				Packet.accbrake  = 85;
				if (car->curgear>0) Packet.flags=CF_GEARDN;
				if ((PARcount>60*10 && CurrentVelocity<2) || (abs(RRS)<30)) 
					{
					PState		 = ssStage1A; 
					ParkTick	 = 0;
					Packet.flags = CF_GEARUP;
					PARcount=0;
					}
			if (abs(RRS)<30) {State	 = ssToTheFloor; ParkTick=0;}
			}

		if (PState==ssStage1A)
			{
			Packet.steer=-RRS;
			Packet.accbrake=-127;
			if (CurrentVelocity<5) PState = ssStage2;
			PARcount=0;
			}
		// Go Forward for a bit..............

		if (PState==ssStage2)
			{
				Packet.flags     = 0;
				Packet.steer     = RRS;
				Packet.accbrake  = 115;
				if (car->curgear<2) {Packet.flags=CF_GEARUP; }
				if (car->curgear>2) {Packet.flags=CF_GEARDN; }
				if ((PARcount>60*10 && CurrentVelocity<2) || (abs(RRS)<30)) 
					{
					PState=ssStage2A;
				//	if (car->wpolytype[0]!=MATTYPE_SHORTGRASS && car->wpolytype[1]!=MATTYPE_SHORTGRASS && car->wpolytype[2]!=MATTYPE_SHORTGRASS && car->wpolytype[3]!=MATTYPE_SHORTGRASS)
					{
						if (abs(RRS)<30) {State	 = ssToTheFloor; ParkTick=0;}
					}
					ParkTick	 = 0;
					}
			}

		PARcount++;
		}

	//=================================================================================================================================
	// (11) Other Stuff
	//=================================================================================================================================


	if (BrakeOverride!=-1)
		{
		if (CurrentVelocity>(BrakeOverride*CatchUpMult)) Packet.accbrake=-127.0f; else {BrakeOverride=-1; SlowEmDown = BrakeHelpFix;}
		}
	TCounter++;
		
	}
	if (CurrentVelocity>(MaxSpeed) && State!=ssParkAndRide) {Packet.accbrake=-127.0f;}
	LastVelocity=0.0f;
	}

	int cp=positiontable.pos[0];
	if (cp>positiontable.pos[CarID])
	{
	if (abs(positiontable.rlp1[CarID]-positiontable.rlp1[0])>50)
		{
		//CatchUpMult=0.5f;
		}
	}
	else
		CatchUpMult=1.0f;


//	if (SlowEmDown>0) {Packet.accbrake=-127.0f; SlowEmDown--;}
	OverTake[CarID]=overtake; // store the current overtake status for each AI car
	Record(Packet);
	return Packet;
}

AIController::AIController()
{
	SpeedModifier   = 1.00f + rand() / 20000;
	LastSpeed		= 300*300;
	PreviousSteer	= 0;
	HoldZero		= 0;
	HoldSteer		= 0;
	LimitAccel		= 0;
	LimitBrake		= 0;
	SkidDirection	= 0;
	SigmaError		= 0.0f;
	TickError		= 0.0f;
	LastAngle		= 0.0f;
	Oscillating		= false;
	LastDirection	= 0.0f;
	Tester			= 0;
	LastRLP			= 0;
	// SDS - new stuff i've added
	//***************************
	TurningSpeed	= 1.0f;
	Gearing			= 1.0f;
	Speed			= 1.0f;
	Avoidence		= 1.0f;
	State			= ssToTheFloor;
	PState		    = ssStage1;
	SteerAhead		= 1.0f;
	SlowEmDown		= 0;
	LDT				= 0.0f;
	AvgSpeed		= 0.0f;
	TCounter		= 0;
	MyPoint			= -1;
	FreezeDir		= 0;
	Breaking		= 0;
	ParkTick		= 0;
	TCounter		= 0;
	// SDS - Init AI parameters....must store these somewhere later
	//"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
	//float MaxSpeed;		// Limit max speed
	//float TurnBrake;		// Breaking Turn look ahead
	//float TLS;			// Turn Look ahead Sensitivity
	//float SLHD;
	//int	BSS;			// Brake system subtract
	//*************************************************************
	SlowEmDown		= 0;
	tul				 = 1;
	MaxSpeed		 = 80.0f;
	TurnBrake		 = -30.0f;
	TLS				 = 0.20f;
	SLHD			 = 68.0f;
	TLBD			 = 15.0f;
	BSS				 = 3;
	OTC				 = 15;
	LLTD			 = 0;
	TurnOffset		 = 0.0f;
	BrakeHelpPointer = 0;
	BrakeOverride	 = -1;
	BrakeHelpCount	 = 0;
	FirstTime		 = NULL;
	TurnCount		 = 0;
	BrakePointMult   = 1;
	LastPoint        = -1;
	overtake		 = 0;
	LastOffset		 = 0;
	carwidth		 = 4.5f;
	lastspinctr      = 0;
	LastCarIndex	 = 0;
	CarSkidVelocity  = 40;
	LLTD			 = 0.0f;
	LastOffset		 = 0.0f;
	difficulty		 = 1;
	brookhack=0;
	pitcount		 = 3;
	pitpos			 = 0;
	pitsub			 = ssToTheFloor;
	CatchUpMult = 1.0f;
	StartOffset		= carwidth;
	StartMod =0;
	LastLap =0 ;
	FuelPerLap=0;
	if (strcmp(game->trackname,"montana")==0   ) {
	    //float pitloc[3][2] = {319,68,118,68,-290,68};        // Pit stop co-ords for montana
		pitloc[0][0]=319;
		pitloc[0][1]=68;
		pitloc[1][0]=10;
		pitloc[1][1]=62;
		pitloc[2][0]=-290;
		pitloc[2][1]=68;
		pitslowdown =75;
		pitstart = 182;  carwidth = 3.0f; TLS =0.45f ; TurnBrake = -0.0f;  MaxSpeed=200.0f; tul=2;
		StartOffset=-3.0f;}
	if (strcmp(game->trackname,"roosevelt")==0 ) 
	{	
        //float pitloc[3][2] = {-195,-215,-116,-215,-58,-215};   // Pit stop co-ords for roosevelt
		pitloc[0][0]=-195;
		pitloc[0][1]=-215;
		pitloc[1][0]=-116;
		pitloc[1][1]=-215;
		pitloc[2][0]=-58;
		pitloc[2][1]=-215;
		pitslowdown =75;
		pitstart = 589;  carwidth = 3.0f; OTC=5.0f;						   MaxSpeed=180.0f; tul=1;
	StartOffset=-3.0f;}
	if (strcmp(game->trackname,"montlhery")==0 || strcmp(game->trackname,"france")==0 ) 
	{	
	    //float pitloc[3][2] = {1783,-282,1730,-282,1683,-282};   // Pit stop co-ords for montlhery		
		pitloc[0][0]=1783;
		pitloc[0][1]=-282;
		pitloc[1][0]=1730;
		pitloc[1][1]=-282;
		pitloc[2][0]=1683;
		pitloc[2][1]=-282;
		pitslowdown =75;
		pitstart = 1338; carwidth = 3.5f; BSS = 2;	   TLBD = 25.0f;       TLS = 0.15f; SLHD = 68.0f; TurnBrake = -127.0f;MaxSpeed = 200.0f; tul = 1;BrakeHelpFix = -150; 
	StartOffset=3.0f;}

	if (strcmp(game->trackname,"tripoli")==0)	  { 
		pitloc[0][0]=1169;
		pitloc[0][1]=-555;
		pitloc[1][0]=1136;
		pitloc[1][1]=-548;
		pitloc[2][0]=1060;
		pitloc[2][1]=-544;
		pitslowdown =45;
		pitstart = 1134; carwidth = 3.0f; SLHD=100.0f; TurnBrake = 0.0f;   MaxSpeed=200.0f; tul=2;}
	if (strcmp(game->trackname,"donington")==0)  
	{ 
	    //float pitloc[3][2] = {-525,-284,-453,-267,-411,-255};   // Pit stop co-ords for donington
		pitloc[0][0]=285;
		pitloc[0][1]=-524;
		pitloc[1][0]=274;
		pitloc[1][1]=-479;
		pitloc[2][0]=241;
		pitloc[2][1]=-378;
		pitslowdown =75;
		pitstart = 465;  carwidth = 3.0f; SLHD=100.0f; TurnBrake = 0.0f;   MaxSpeed=200.0f; tul=2;
	StartOffset=-3.0f;}
	if (strcmp(game->trackname,"pau")==0)        
	{ 
		//float pitloc[3][2] = {237,-30,238,-84,235,-164};   // Pit stop co-ords for pau
		pitloc[0][0]=237;
		pitloc[0][1]=-30;
		pitloc[1][0]=238;
		pitloc[1][1]=-84;
		pitloc[2][0]=235;
		pitloc[2][1]=-164;
		pitslowdown =75;
		pitstart = 305;  carwidth = 2.5f; SLHD=100.0f; TurnBrake = 0.0f;   MaxSpeed=200.0f; tul=2;
	StartOffset=-3.0f;}
	if (strcmp(game->trackname,"avus")==0)       {	
		pitloc[0][0]=-1939;
		pitloc[0][1]=-84;
		pitloc[1][0]=-1883;
		pitloc[1][1]=-87;
		pitloc[2][0]=-1831;
		pitloc[2][1]=-80;		
		pitslowdown =55;
		pitstart = 1303;	carwidth = 2.75f; SLHD=100.0f; TurnBrake = 0.0f;   MaxSpeed=200.0f; tul=2;
		StartOffset=2.95;}
	if (strcmp(game->trackname,"brooklands")==0) 
	{
        //float pitloc[3][2] = {108,-327,483,-7,714,208};   // Pit stop co-ords for brooklands

		for (int a=0;a<30;a++)
			{
			racingLine *RacingLine;
			RacingLine = racinglineset.getLine(1);
			RacingLine->racingLinePoints[474-30+a].x=BrookFix[a][0];
			RacingLine->racingLinePoints[474-30+a].z=BrookFix[a][1];
			}


		pitloc[0][0]=108;
		pitloc[0][1]=-327;
		pitloc[1][0]=476;
		pitloc[1][1]=0;
		pitloc[2][0]=713;
		pitloc[2][1]=222;		
		pitslowdown =75;
		pitstart = 390;  carwidth = 3.0f; SLHD=100.0f; TurnBrake = 0.0f;   MaxSpeed=200.0f; tul=4;}
	if (strcmp(game->trackname,"monza")==0)      
	{
        //float pitloc[3][2] = {321,446,348,425,409,391};   // Pit stop co-ords for monza
		pitloc[0][0]=309;
		pitloc[0][1]=453;
		pitloc[1][0]=375;
		pitloc[1][1]=407.286;
		pitloc[2][0]=439;
		pitloc[2][1]=371;
		pitslowdown =75;
		pitstart = 766;	 carwidth = 3.0f; TurnBrake = -127.0f;MaxSpeed=200.0f; tul=1; 
		StartOffset=-3.0f;
		StartMod=1;
	}
	type = CONTROLLER_AI;
}