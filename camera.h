/* Camera code.
   Includes trackside cameras and attached camera.
   Ian Gledhill 29/03/1999    */

#ifndef __CAMERA_H__

#include <stdlib.h>

#include "BS2all.h"
#include "ALLcar.h"
#include "Spline.h"

#include "PositionTable.h"


#define MAXCAMERAS 50

#define CAMERA_TRACKING 0
#define CAMERA_STATIC	1
#define CAMERA_SWOOP	2
#define CAMERA_REVERSE	3
#define CAMERA_DOLLY	4
#define CAMERA_FORWARD_DOLLY	5
#define CAMERA_LOW_DOLLY	6
#define CAMERA_TYRE	7

class ReplayCamera
{
public:
	// Create the replay camera. The spline and the speed are needed.
	ReplayCamera(spline *_Spline, float _Speed, int _RacingLinePoint, float _Scale, bool UseCarAsTarget, vector3 *_TargetVector=NULL);

	// Start the camera.
	// The camera looks at _TargetCar unless _TargetCar == -1, in which
	// case it looks at _TargetPosition;
	void StartCamera(int _TargetCar, vector3 *_TargetPosition);

	// Update the position of the camera.
	// Moves the camera along the spline by Speed.
	// If the parameter is equal to 1, the function returns
	// without moving *relative to the car*.
	void UpdateCamera();

	// Get/Set the active flag.
	void SetActive(bool _Active);
	bool GetActive();

	// IsTriggered(rlp) returns true if rlp > RacingLinePoint.
	bool IsTriggered(int CurrentPoint);

	
	// The actual camera!
	camera *ActualCamera;

private:

	// The identifier for the camera.
	int ID;

	// The starting position of the camera.
	vector3 Position;

	// The racing line point which triggers this camera.
	int RacingLinePoint;

	// The scale of the spline.
	float Scale;

	// The target of the camera.
	// This should be the number of the car targetted or -1 if the 
	// camera is focussing on the track.
	int TargetCar;

	// If looking at the track, use this vector3.
	vector3 TargetVector;

	// The speed of the camera in distance/tick.
	float Speed;

	// If this is false, the camera won`t be used.
	bool Active;
	
	// The spline used by the car.
	spline CameraSpline;

	// The current position (normalised) along the spline.
	float Parameter;

	// The adjusted speed (according to the velocity of the spline)
	float SpeedAdjust;
};

#if 1
class CameraBank
{
public:
	CameraBank();
	~CameraBank();

	// Update the current camera.
	bool UpdateCameras();

	// If "use" is true, trackside cameras are activated.
	// Only bog-standard panning manoeuvres if Snazzy is false.
	void UseTrackside(bool use, bool Snazzy);

	// Queue a camera position to be used on its own.
	void QueueCamera(camera *);

	// UsingTrackside is false if the camera is the standard rear view.
	bool UsingTrackside;

	// Increment the targetted car by one.
	void TargetNextCar();

	// Find the next camera for a particular racing line point.
	int FindNextCamera(int Point);

	// Get the boris2camera structure of the currently used trackside camera.
	camera *GetCamera();
	int NumCams1,NumCams2; // Hack for brooklands
	int UsedZeroLine;

private:
	// All the cameras on the track.
	ReplayCamera **CameraList;
	ReplayCamera **CameraList1; // Hack for brooklands.....
	// The current camera.
	int CurrentCamera;

	// Guess what.
	int NumberOfCameras;

	// Do tricks with the cameras?
	bool SnazzyMode;

	// The list of splines available, say 30 for now.
	spline *SplineList[30];
	spline *TrackSplineList[50];
	vector3 *TrackSplineTargets[50];
	float ZEyeList[50];
};

#else

class CameraBank
{
private:
	// maximum MAXCAMERAS cameras at the moment.
	camera *CameraList0[MAXCAMERAS];
	camera *CameraList1[MAXCAMERAS];
	car **CarList;
	int CameraRange0[MAXCAMERAS], CameraRange1[MAXCAMERAS];
	vector3 CameraPosition0[MAXCAMERAS], CameraPosition1[MAXCAMERAS];
	int ProhibitedViews0[MAXCAMERAS], ProhibitedViews1[MAXCAMERAS];
	int CurrentCamera;
	int CameraTime, StaticLimit;

	// The number of the last camera on the track.
	int LastCamera0, LastCamera1;
	// Parameters to pass to the new camera constructor.
	float Param1, Param2;

	bool UsingOverheadCamera;

	// The car currently viewed.
	int CurrentCar;

	// Do tricks with the cameras?
	bool SnazzyMode;

	// Current Modifier for the camera (i.e. x mod for dollying
	int XModifier;

public:
	// The current mode of the camera.
	int CameraMode;
	
	bool UsingTrackside;
	CameraBank(car **AllCars, float NewParam1, float NewParam2);

	void View(int NewCamera);
	camera *GetCamera();
	bool UpdateCameras();

	void UseTrackside(bool use, bool Snazzy);

	// Create the cameras from the text file pointed to.
	void CreateCameras();

	// View the next camera now.
	void NextCamera();

	// Use a camera over the car.
	void UseOverheadCamera();

	// Queue a camera position to be used on its own.
	void QueueCamera(camera *);

	// Return the camera to its original position.
	void RestoreCameraPosition(int CameraNumber, int ListNumber=0);

	// Execute any special effects on the camera.
	void CameraEffect(camera *ThisCamera, int Prohibited, vector3 Target);
};
#endif

#endif __CAMERA_H__