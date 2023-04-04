/* Camera code.
   Includes trackside cameras and attached camera.
   Ian Gledhill 29/03/1999    */

#include <ctype.h>
#include "Camera.h"
#include "FrontEnd.h"
#include "CarSound.h"
#include "Spline.h"

#include "bs2all.h"

// Only temporary, don`t worry.
#define MAX_CAMERAS 50

extern void dprintf(char *,...);
extern bool UseFloatingCamera;
extern camera *FloatingCamera, *boris2camera, *boris2mirror;
extern class car *cameracar;
extern gamesetup *game;
extern FILE *CameraLocations;
extern struct gamesetup GlobalGame;
extern bool NoPlayers;

int UseOnlyThisCamera = 0;
#define dprintf(x)

ReplayCamera::ReplayCamera(spline *_Spline, float _Speed, int _RacingLinePoint, float _Scale, bool UseCarAsTarget, vector3 *_TargetVector)
{
	// Save the variables.
	CameraSpline = *_Spline;
	Speed = _Speed;
	RacingLinePoint = _RacingLinePoint;
	Scale = _Scale;

	if (_TargetVector)
		TargetVector = *_TargetVector;
	else
		TargetVector = vector3(-1,-1,-1);

	if (!UseCarAsTarget)
		TargetCar = -1;
	else
		TargetCar = 0;

	// Give it a unique ID.
	static int _ID=0;
	ID = _ID++;

	vector3 temp;
	CameraSpline.initSpline();
	CameraSpline.getSplinePos(0.0f, &temp);
	CameraSpline.getSplineDir(&temp);
	SpeedAdjust=temp.Mag();

	// Create the actual camera.
	ActualCamera = new camera(&vector3(30.0f,10.0f,0.0f), 1024.0f, 700.0f); 

	return;
}

void ReplayCamera::StartCamera(int _TargetCar, vector3 *_TargetPosition)
{
	boris2->setCurrentCamera(ActualCamera);
	carsounds.NominateListener(ActualCamera);

	Parameter = 0.0f;
}

void ReplayCamera::UpdateCamera()
{
	// Return if we don`t have a valid car to focus on.
	if (!cameracar)
		return;

//	dprintf("Updating Camera %d", ID);

	if (ID==5)
		bool test = true;

	Parameter += Speed/SpeedAdjust * 5.0f;
	if (Parameter > 1.0f)
		Parameter = 1.0f;

	// Otherwise grab the position.
	vector3 CarVector = cameracar->mainObject->concatenatedTransform.translation;
	
	// Move the camera to the correct position.
	vector3 CameraPosition, TransformedPosition, CameraDirection;
	CameraSpline.getSplinePos(Parameter, &CameraPosition);
	CameraSpline.getSplineDir(&CameraDirection);
	SpeedAdjust=CameraDirection.Mag();

	CameraPosition *= Scale;

	vector3 TransformedTargetVector=vector3(-1.0f,-1.0f,-1.0f);
	// Transform the camera if we're relative to the car and not the world.
	if (TargetCar != -1)
	{
		cameracar->mainObject->concatenatedTransform.multiplyV3By3x3mat(&TransformedPosition,&CameraPosition);
		ActualCamera->position = TransformedPosition + CarVector;

#if defined(UNDER_CE)
		if (fabsf(TargetVector.x+1) >= 0.000001f)
#else
		if (fabs(TargetVector.x+1) >= 0.000001)
#endif
		{
			// We're targetting somewhere else.
			cameracar->mainObject->concatenatedTransform.multiplyV3By3x4mat(&TransformedTargetVector,&TargetVector);
		}
	}
	else 
	{
		ActualCamera->position = CameraPosition;
	}

#if defined(UNDER_CE)
	if (fabsf(TargetVector.x + 1) < 0.000001f)
#else
	if (fabs(TargetVector.x + 1) < 0.000001)
#endif
		ActualCamera->faceTowardPoint(&CarVector, true);
	else
	{
#if defined(UNDER_CE)
		if (fabsf(TransformedTargetVector.x + 1) < 0.000001f)
#else
		if (fabs(TransformedTargetVector.x + 1) < 0.000001)
#endif
			ActualCamera->faceTowardPoint(&TargetVector, true);
		else
			ActualCamera->faceTowardPoint(&TransformedTargetVector, true);
	}

	return;
}

bool ReplayCamera::IsTriggered(int CurrentPoint)
{
	if (RacingLinePoint <= CurrentPoint)
		return true;
	else 
		return false;
}

CameraBank::CameraBank()
{
	
	UsedZeroLine=0;
	// Push the cameras onto the stack.
	arcPush("archives\\cameras.tc");
	
	// Use a test camera.
	CameraList = (ReplayCamera **)malloc(MAX_CAMERAS * sizeof(ReplayCamera **));

	// Create a new camera.
	vector3 *SplineData[10];

	// Allocate some data for the splines.
	char SplineText[10000];
	char SplineName[256];

	// Set this to true when we're out of cameras.
	bool DoneAllCameras = false;
	
	// The current spline to read.
	int SplineNumber = 0;

	char *NumberStart;
	int NumberLength;
	char NumberText[100];

	CurrentCamera = 0;

	// Create the filename and increment the counter.
	sprintf(SplineName, "Spline%d", SplineNumber++);
	int FileLength = archivestack.ExtractIntoArea(SplineName,FMT_TXT_PARAM,SplineText,10000);

	// No more cameras, exit the loop.
	if (!FileLength)
		DoneAllCameras = true;

	while (!DoneAllCameras)
	{
		int SplinePoint = 0;
		float x,y,z;

		NumberStart = SplineText;

		do 
		{
			// Find three variables: x, y and z.
			// Set the length as the position of the space minus the start of the string.
			NumberLength = (long)strchr(NumberStart, ' ') - (long)NumberStart;

			// Copy the number to a buffer to call atof() on.
			strncpy(NumberText, NumberStart, NumberLength);
			x = (float)atof(NumberText);

			// Increment the pointer and find the position and length of z. (not Y 'cos
			// artists do it the wrong way round.
			NumberStart += NumberLength + 1;
			NumberLength = strchr(NumberStart, ' ') - NumberStart;
			strncpy(NumberText, NumberStart, NumberLength);
			z = (float)atof(NumberText);

			// And now y.
			// This time we look for a terminating CR.
			NumberStart += NumberLength + 1;
			NumberLength = strchr(NumberStart, 13) - NumberStart;
			strncpy(NumberText, NumberStart, NumberLength);
			y = (float)atof(NumberText);

			// Create the Spline Node;
			SplineData[SplinePoint] = new vector3(x,y,z);
			dprintf("N: %d  X: %f  Y: %f  Z: %f", SplinePoint, x,y,z);

			// Increment the pointer, bypassing the CR/LF.
			NumberStart += NumberLength + 2;

			// Look for the next spline point now.
			SplinePoint++;
		} while (*NumberStart != '*');	// Loop until we hit a *.
		
		SplineList[SplineNumber-1] = new spline(SplineData, SplinePoint);

		// Read the next spline.
		sprintf(SplineName, "Spline%d", SplineNumber++);
		dprintf("Name: %s", SplineName);
		FileLength = archivestack.ExtractIntoArea(SplineName,FMT_TXT_PARAM,SplineText,10000);

		// No more cameras, exit the loop.
		if (!FileLength)
			DoneAllCameras = true;
	
	}

	// Load in the track splines.
	FileLength = archivestack.ExtractIntoArea("TrackSplines",FMT_TXT_PARAM,SplineText,10000);

	if (FileLength)
	{
		char *SplinePointer = SplineText;
		float x, y, z, x2, y2, z2;

		SplineNumber = 0;

		while (strncmp(SplinePointer, "end", 3))
		{
			sscanf(SplinePointer,"%f %f %f %f %f %f %f",&x,&z,&y,&x2,&z2,&y2,&ZEyeList[SplineNumber]);

			// Create a 'spline' which has two points the same.
			SplineData[0] = new vector3(x,y,z);
			SplineData[1] = new vector3(x,y,z);

			TrackSplineList[SplineNumber] = new spline(SplineData, 2);

			TrackSplineTargets[SplineNumber] = new vector3(x2,y2,z2);

			SplinePointer = strchr(SplinePointer, 13)+2;

			dprintf("track spline %d OK",SplineNumber);
			SplineNumber++;
		}
	}

	// We now have all the splines loaded.
	// Time to load the camera positions.

	// Assume only one racing line for now.

	char CamerasText[2048];
	FileLength = archivestack.ExtractIntoArea("Cameras-line0",FMT_TXT_PARAM,CamerasText,2048);

#if !defined(UNDER_CE)
	if (!FileLength) FatalError("No cameras in archive!");
#endif
	char *TextPointer = CamerasText;

	int CameraNumber = 0;

	while (strncmp(TextPointer, "end",3))
	{
		int ActivatePoint, Spline, Target;
		float Speed, Scale;

		ActivatePoint = atoi(TextPointer);
		TextPointer = strchr(TextPointer, ' ') + 1;
		Speed = (float) atof(TextPointer);
		TextPointer = strchr(TextPointer, ' ') + 1;
		Spline = atoi(TextPointer);
		TextPointer = strchr(TextPointer, ' ') + 1;
		Scale = (float) atof(TextPointer);
		TextPointer = strchr(TextPointer, ' ') + 1;
		Target = atoi(TextPointer);
		TextPointer = strchr(TextPointer, 13) + 2;

		if (Target >= 1)
		{
			dprintf("constructing car spline %d",Spline);

			vector3 TargetVector, TransformedVector;
			if (Target == 2)
			{
				TargetVector = vector3(0.0f,0.0f,100.0f);
				CameraList[CameraNumber++] = new ReplayCamera(SplineList[Spline], Speed * 0.001f, ActivatePoint, Scale, true, &TargetVector);
			}
			else
				CameraList[CameraNumber++] = new ReplayCamera(SplineList[Spline], Speed * 0.001f, ActivatePoint, Scale, true);
		}
		else
		{
			dprintf("constructing track spline %d",Spline);
			CameraList[CameraNumber++] = new ReplayCamera(TrackSplineList[Spline], 0, ActivatePoint, 1.0f, false, TrackSplineTargets[Spline]);
		}
	}
	NumberOfCameras = CameraNumber;
	NumCams1 = CameraNumber;
	// We start with a car view.

	UsingTrackside = false;

	// SDS - Gotta write a new loader to load in cameras-line1
	// Damn hacks....see above for the classic line> "// Assume only one racing line for now." :)

	char CamerasText2[2048];
	FileLength = archivestack.ExtractIntoArea("Cameras-line1",FMT_TXT_PARAM,CamerasText2,2048);
	if (FileLength!=0) 
		{
		CameraList1 = (ReplayCamera **)malloc(MAX_CAMERAS * sizeof(ReplayCamera **));
		char *TextPointer = CamerasText2;

		int CameraNumber = 0;

		while (strncmp(TextPointer, "end",3))
		{
		int ActivatePoint, Spline, Target;
		float Speed, Scale;

		ActivatePoint = atoi(TextPointer);
		TextPointer = strchr(TextPointer, ' ') + 1;
		Speed = (float) atof(TextPointer);
		TextPointer = strchr(TextPointer, ' ') + 1;
		Spline = atoi(TextPointer);
		TextPointer = strchr(TextPointer, ' ') + 1;
		Scale = (float) atof(TextPointer);
		TextPointer = strchr(TextPointer, ' ') + 1;
		Target = atoi(TextPointer);
		TextPointer = strchr(TextPointer, 13) + 2;

		if (Target >= 1)
			{
			vector3 TargetVector, TransformedVector;
			if (Target == 2)
			{
				TargetVector = vector3(0.0f,0.0f,100.0f);
				CameraList1[CameraNumber++] = new ReplayCamera(SplineList[Spline], Speed * 0.001f, ActivatePoint, Scale, true, &TargetVector);
			}
			else
				CameraList1[CameraNumber++] = new ReplayCamera(SplineList[Spline], Speed * 0.001f, ActivatePoint, Scale, true);
			}
			else
			{
			dprintf("constructing track spline %d",Spline);
			CameraList1[CameraNumber++] = new ReplayCamera(TrackSplineList[Spline], 0, ActivatePoint, 1.0f, false, TrackSplineTargets[Spline]);
			}
		}
		NumCams2 = CameraNumber;
		//NumCams2 = 10;
		}

			
		


}

void CameraBank::QueueCamera(camera *NewCamera)
{
}

void CameraBank::UseTrackside(bool use, bool Snazzy)
{
	// Set the correct flag so we know to use the trackside stuff.
	UsingTrackside = use;

	CameraList[0]->StartCamera(cameracar->id, NULL);
}

void CameraBank::TargetNextCar()
{
}

bool CameraBank::UpdateCameras()
{
	int Point;

	// Only update if there's a car to target!
	if (!cameracar)
		return false;

	if (!UsingTrackside)
		return true;
	
	// Check that the racingline point is valid.
	Point = positiontable.rlp1[cameracar->id];
	if (Point == 100000) return false;
	
	// Find the appropriate camera.

	int PreviousCamera = CurrentCamera;
	CurrentCamera = FindNextCamera(positiontable.rlp1[cameracar->id]);

	// Hack for brooklands
	// This works by using the positiontable.curline[] array
	// which seems not to work....
	// Gonna have to put in a real ugly hack for brooklands!!
	// Must remeber to come up wiv a slicker way for doing this
	//dprintf("Update Cameras called");
	racingLine *RacingLine;
	RacingLine = racinglineset.getLine(0);

	if (RacingLine->pointsPerLine==475)
	{
	if ((positiontable.laps[cameracar->id]<=0 && positiontable.rlp1[cameracar->id]>=470 && UsedZeroLine==0) ||
		(positiontable.laps[cameracar->id]<=0 && positiontable.rlp1[cameracar->id]<=60) || 
		(positiontable.laps[cameracar->id]>=(positiontable.nlaps-1) && positiontable.rlp1[cameracar->id]>=374))
		{
		if (PreviousCamera != CurrentCamera) CameraList[CurrentCamera]->StartCamera(cameracar->id, NULL);
	CameraList[CurrentCamera]->UpdateCamera();
		//dprintf("Line2 ss %d %d",NumCams1,NumCams2);
		}
	else
		{
		if (PreviousCamera != CurrentCamera) CameraList1[CurrentCamera]->StartCamera(cameracar->id, NULL);
		CameraList1[CurrentCamera]->UpdateCamera();
		UsedZeroLine=1;
		//dprintf("Line1 ss %d %d",NumCams1,NumCams2);
		}
	}
	else
	{
		if (PreviousCamera != CurrentCamera) CameraList[CurrentCamera]->StartCamera(cameracar->id, NULL);
		CameraList[CurrentCamera]->UpdateCamera();
		//dprintf("Line1 other case.)...";
	}


	return false;
}

int CameraBank::FindNextCamera(int Point)
{
	int NextCamera;
	
		// This is the active camera, so return it.
		// If the next camera is the first camera, then it will always
		// be triggered, so use this camera.
		//if (!(CameraList[NextCamera]->IsTriggered(Point)) || NextCamera == 0) return CurrentCamera;

	racingLine *RacingLine;
	RacingLine = racinglineset.getLine(0);

	if (RacingLine->pointsPerLine==475)
	{
	if ((positiontable.laps[cameracar->id]<=0 && positiontable.rlp1[cameracar->id]>=470 && UsedZeroLine==0) ||
		(positiontable.laps[cameracar->id]<=0 && positiontable.rlp1[cameracar->id]<=60) || 
		(positiontable.laps[cameracar->id]>=(positiontable.nlaps-1) && positiontable.rlp1[cameracar->id]>=374))
		{
		//dprintf("CurrentCam %d",CurrentCamera);
		if (CurrentCamera>NumCams1-1) CurrentCamera=0;
		while (CameraList[CurrentCamera]->IsTriggered(Point))
			{
			NextCamera = CurrentCamera +1;
			NumberOfCameras=NumCams1;
			if (NextCamera > NumberOfCameras-1)	NextCamera = 0;
			if (!(CameraList[NextCamera]->IsTriggered(Point)) || NextCamera == 0) return CurrentCamera;
			CurrentCamera = NextCamera;
			};
		}
	else
		{
		while (CameraList1[CurrentCamera]->IsTriggered(Point))
			{
			NextCamera = CurrentCamera +1;
			NumberOfCameras=NumCams2;
			if (NextCamera > NumberOfCameras-1)	NextCamera = 0;
			if (!(CameraList1[NextCamera]->IsTriggered(Point)) || NextCamera == 0) return CurrentCamera;
			CurrentCamera = NextCamera;
			};
		}
	}
	else
	{
	while (CameraList[CurrentCamera]->IsTriggered(Point))
		{
		NextCamera = CurrentCamera +1;
		if (NextCamera > NumberOfCameras-1)	NextCamera = 0;
		if (!(CameraList[NextCamera]->IsTriggered(Point)) || NextCamera == 0) return CurrentCamera;
		CurrentCamera = NextCamera;
		};
	}
		// Otherwise, go to the next camera.
	// If we've not found a camera, set the camera to 0.

	return 0;
}

camera *CameraBank::GetCamera()
{
	return CameraList[CurrentCamera]->ActualCamera;
}

CameraBank::~CameraBank()
{
	archivestack.Pop();
}