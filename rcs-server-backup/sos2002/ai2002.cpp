// $Header

// $Log

// OK, this is actually going to be _good_ AI. No, really. I mean it.

#include <windows.h>
#include "BS2All.h"
#include "ALLCar.h"
#include "controller.h"

#include "AI_Spline.h"

AI_SplineManager *gSplineManager;

#include "AILines/DoningtonData.h"
// The hack starts here. Just a testing function.
extern car *cameracar;

#define IAN_DEBUG
char Buffer[200];

AIController::AIController()
{
	int raceLineSize,i;
	SplineID centreSplineID;

	gSplineManager = new AI_SplineManager;
	
	raceLineSize = sizeof(CentreRaceLine) / 3 / sizeof(double);
	centreSplineID = gSplineManager->LoadSpline( "Squidgy", CentreRaceLine, raceLineSize);
	carSplineInterface = gSplineManager->AddInterface( centreSplineID, vector3((float)CentreRaceLine[0], (float)CentreRaceLine[2], (float)CentreRaceLine[1]));

#if defined IAN_DEBUG
	extern char WheelBuffer[];
	model *DebugModel;
	vector3 wayPoint;

	DebugModel = getModelPtr( WheelBuffer, 10.0f, false, NULL, true);
	for (i=0 ; i<10 ; i++)
	{
		debugTyreObject[i] = new object( DebugModel, 0, false );
		debugTyreObject[i]->setAngle( &vector3(0, 0, 0 ));
		boris2->insertObject(debugTyreObject[i]);
		debugTyreObject[i]->visible = true;
	}
#endif IAN_DEBUG
}

class ctrlpckt AIController::GetControlPacket()
{
	ctrlpckt controlPacket;
	int i;

	controlPacket.zero();

	matrix3x4 *thisMatrix = &car->mainObject->transform;
	vector3 direction = vector3(thisMatrix->E02, thisMatrix->E12, thisMatrix->E22);
	vector3 position = car->mainObject->transform.translation;
	vector3 turningVector, *nextPoint;
	vector3 approachingPoint[10], normalToApproachingPoint[10];
	float angleToApproachingPoint[10];
	float turningRequired;
	float speed;
	static float distancesAhead[10]= {   2,    20,    40,   60,   80,   100,   130,  210,   220,  230};
	static float distanceWeights[10]={1.0f, 11.0f, 15.5f, 17.5f,  18.0f, 22.5f,  9.0f, 2.5f,  2.7f, 3.5f};
//	static float speedModifiers[10]= {1.0f, 2.0f,   1.8f, 1.6f,  0.6f, 0.4f,  5.0f, 5.0f, 20.0f, 5.0f};
	static float speedModifiers[10]= {1.0f, 20.0f, 80.0f, 40.0f, 25.0f, 20.0f,  70.0f, 75.0f, 20.0f, 5.0f};
	int distanceToCurve=0, foundCurve=0;
	float curveTightness=0.0f, brakeFactor=0.0f, targetSpeed = 1000.0f;

	gSplineManager->MoveTo(carSplineInterface, position);
	gSplineManager->ClosestSplinePoint( carSplineInterface );

	// OK, let's see if we have a curve approaching by looking at x m along the spline
	// where x is defined according to the array above.

	direction.Normalise();
	for (i=0 ; i<10 ; i++)
	{
		approachingPoint[i] = gSplineManager->PointAhead( carSplineInterface, direction, distancesAhead[i] );
#if defined IAN_DEBUG
		debugTyreObject[i]->setPosition( &approachingPoint[i]);
		debugTyreObject[i]->inheritTransformations();
		debugTyreObject[i]->changed = true;
#endif IAN_DEBUG
		approachingPoint[i] -= position;
		normalToApproachingPoint[i] = approachingPoint[i];
		normalToApproachingPoint[i].Normalise();
		angleToApproachingPoint[i] = normalToApproachingPoint[i].DotProduct( &direction );
	}

	if (car->curgear < 2 )
		controlPacket.flags |= CF_GEARUP;

	controlPacket.accbrake = 127;
	if (car->wskid[2]>0.5f || car->wskid[3]>0.5f)
	{
		controlPacket.steer = 0;
		controlPacket.accbrake = controlPacket.accbrake >> 1;
	}

	nextPoint = &approachingPoint[1];
	turningVector = normalToApproachingPoint[1] - direction;
	turningRequired = angleToApproachingPoint[1];

	speed = car->velocity.Mag();

#if defined IAN_DEBUG
	for (i=0 ; i<10 ; i++)
	{
		sprintf(Buffer, "Point %d: ", i);
		grdebug(Buffer, angleToApproachingPoint[i]);
	}
#endif IAN_DEBUG

	int tempSteer;
	int leftOrRight=0;

//	controlPacket.steer = (char)(200.0f * (nextPoint.z < 0.0f ? -1.0f : 1.0f) * (turningVector.x < 0.0f ? 1.0f : 1.0f) * (turningVector.z > 0.0f ? 1.0f : 1.0f) * (1.0f-turningRequired*turningRequired)*fabs(turningVector.x));
	if (nextPoint->z > 0.0f && direction.z < 0.0f || nextPoint->z < 0.0f && direction.z > 0.0f)
	{
		if (direction.x > 0.0f)
			turningVector.x = -2.0f + turningVector.x;
		else
			turningVector.x = 2.0f - turningVector.x;
	}
		
	if (turningVector.x < -0.0001f)
		leftOrRight = +1;	// left
	else if (turningVector.x > 0.0001f)
		leftOrRight = -1;	// right

	turningVector.x = (float)fabs(turningVector.x);

	if ((nextPoint->z < 0.0f) || turningRequired < 0.0f || fabs(turningVector.z) > 1.0f)
		leftOrRight = -leftOrRight;

	tempSteer = (int)(900.0f * turningVector.x * leftOrRight);

	if ( leftOrRight == 1)
	{
		if (tempSteer > 127)
			tempSteer = 127;
	}
	else if ( leftOrRight == -1)
	{
		if (tempSteer < -127)
			tempSteer = -127;
	}
	else
		tempSteer = 0;

	controlPacket.steer = tempSteer;

	for (i=1 ; i<6+(speed > 40.0f ? (int)((speed-20.0f) / 10.0f) : 0) ; i++)
	{
		if (angleToApproachingPoint[i] < 0.97f || angleToApproachingPoint[i] < 0.985f && i == 7)
		{
			curveTightness = -(angleToApproachingPoint[i]*angleToApproachingPoint[i]);// - (angleToApproachingPoint[i+1] * distanceWeights[i] * 2) - (angleToApproachingPoint[i+2] * distanceWeights[i] * 4);
			if ((brakeFactor = curveTightness) < 0.0f)
			{
				distanceToCurve = i;
				foundCurve = i;
				i=11;
			}
		}
	}

	grdebug("Curve", foundCurve);
	grdebug("Distance", distanceToCurve);
	grdebug("Tightness", curveTightness);
/*
	if (angleToApproachingPoint[2] < 0.95f && speed > 20.0f)
		controlPacket.accbrake = (char)((0.75f - angleToApproachingPoint[2] * angleToApproachingPoint[2] * angleToApproachingPoint[2] * angleToApproachingPoint[2]) * -100 * ((speed-20.0f) * 0.1f));
	else if (angleToApproachingPoint[6] < 0.85f && speed > (angleToApproachingPoint[6] < 0.5f ? 32.0f : 40.0f))
	{
		controlPacket.accbrake = -127;//(0.80f - angleToDistantPoint) * -150 * ((speed-15.0f) * 0.1f);
	}
*/
	if (foundCurve)
	{
		targetSpeed = 100.0f + (curveTightness * speedModifiers[foundCurve]);
		if (speed > targetSpeed)
			controlPacket.accbrake = -100;//(0.80f - angleToDistantPoint) * -150 * ((speed-15.0f) * 0.1f);
	}

	grdebug("Brake factor", brakeFactor);
	grdebug("Target", targetSpeed);

	if (car->revs > car->uprevlim[car->curgear] && car->wskid[2] < 0.1f)
		controlPacket.flags |= CF_GEARUP;
	if (car->curgear > 2 && car->revs < car->lorevlim[car->curgear])
		controlPacket.flags |= CF_GEARDN;

	if (abs(tempSteer) > 30 && controlPacket.accbrake < 0)
		controlPacket.accbrake = 0;
	grdebug("Speed", speed);

	return controlPacket;
}

void DisplayAILines()
{
	extern char WheelBuffer[];
	model *DebugModel;
	object **DebugObject, **DebugLeftObject;
	vector3 wayPoint;
	int raceLineSize, leftLineSize;

	raceLineSize = sizeof(CentreRaceLine) / 3 / sizeof(double);
	DebugModel = getModelPtr( WheelBuffer, 10.0f, false, NULL, true);
	DebugObject = (object **)malloc(raceLineSize * sizeof(object *));
	leftLineSize = sizeof(LeftSideRaceLine) / 3 / sizeof(double);
	DebugLeftObject = (object **)malloc(leftLineSize * sizeof(object *));

	for (int i=0; i<raceLineSize ; i++)
	{
		DebugObject[i] = new object( DebugModel, 0, false );
		DebugObject[i]->setAngle( &vector3(0, 0, 0 ));

		wayPoint = vector3((float)CentreRaceLine[i*3], (float)CentreRaceLine[i*3+2], (float)CentreRaceLine[i*3+1]);
		DebugObject[i]->setPosition( &wayPoint);
		DebugObject[i]->visible = true;
		DebugObject[i]->changed = true;
		DebugObject[i]->inheritTransformations();
		boris2->insertObject(DebugObject[i]);
		DebugObject[i]->changed = true;
	}
	for (i=0; i<leftLineSize ; i++)
	{
		DebugLeftObject[i] = new object( DebugModel, 0, false );
		DebugLeftObject[i]->setAngle( &vector3(0, 0, 0 ));

		wayPoint = vector3((float)LeftSideRaceLine[i*3], (float)LeftSideRaceLine[i*3+2], (float)LeftSideRaceLine[i*3+1]);
		DebugLeftObject[i]->setPosition( &wayPoint);
		DebugLeftObject[i]->visible = true;
		DebugLeftObject[i]->changed = true;
		DebugLeftObject[i]->inheritTransformations();
		boris2->insertObject(DebugLeftObject[i]);
		DebugLeftObject[i]->changed = true;
	}
}