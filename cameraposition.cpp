// $Header$

// $Log$
// Revision 1.1  2002-07-22 10:07:04+01  jjs
// Test version of SOS2002
//
// Revision 1.0  2000-02-21 12:34:09+00  jjs
// Initial revision
//

#include <windows.h>
#include <stdio.h>
#if !defined(UNDER_CE)
#include <process.h>
#include <new.h>
#endif
#include <stdlib.h>

#include "BS2all.h"
#include "ALLcar.h"
#include "controller.h"
#include "frontend.h"
#include "overlay.h"	

//
//	Camera positioning code for car-attached cameras
//

extern bool detachmode;
extern bool lagmode,rearglance;
extern void dprintf(char *,...);
extern camera* boris2mirror,*boris2mirror2;

void positionCamera(class car *c)
{
	camera *cam = c->mainObject->attached;
	static vector3 temp,mvel,desvec,temp2,lookat;

	static vector3 pos=vector3(0.0f,0.0f,0.0f);
	static bool lastview=false;
	static bool saveview=false;
	static vector3 oldview;

	vector3 oldlookat,wcofg,rvect;
	//Set damping coefficients and break off point for when car spins
	float angstart=3.0f;
	float anglen=0.3f;
	static float morph,newmorph,tempmorph;


	if(cam)
	{
		
		if(rearglance)
		{
			saveview=true;
			oldview=cam->position;
			desvec=vector3(0.0f,0.0f,-2.0f);
			c->mainObject->transform.multiplyV3By3x4mat(&rvect,&desvec);
			cam->position=rvect;
			desvec=vector3(0.0f,0.0f,-30.0f);
			c->mainObject->transform.multiplyV3By3x4mat(&rvect,&desvec);
			cam->faceTowardPoint(&rvect);
			lastview=false;
			return;
		}
		
		if(saveview)
		{
			cam->position=oldview;
		}


		cam->minheight = 1.0;

		// the 'lookat' point is set to 4m in front of the car.
		desvec=vector3(0.0,0.0,4.0f);
		
		oldlookat=lookat;

		c->mainObject->transform.multiplyV3By3x4mat(&lookat,&desvec);

		c->mainObject->transform.multiplyV3By3x4mat(&wcofg,&c->CofG);

		if(!lagmode)
		{
#if defined(UNDER_CE)
			newmorph=(fabsf(c->angvel)-angstart)*anglen;
#else
			newmorph=(fabs(c->angvel)-angstart)*anglen;
#endif
			if (newmorph>morph) morph=newmorph; else morph*=0.99f;

			//grdebug("newmorph",newmorph);
			//grdebug("morph",morph);
			
			
			if (morph>1.0f)morph=1.0f;
			if (morph<0.0f)morph=0.0f;

			lookat=wcofg*morph+lookat*(1.0f-morph);

			

			tempmorph=morph*0.5f;
			
		} 
		else
		{
			tempmorph=0.5f;
		}
		
		if(saveview)
		{
			tempmorph=morph=0.0f;
		}

		lookat=oldlookat*(tempmorph+0.2f)+lookat*(0.8f-tempmorph);


		// the temp2 vector is set to 5m behind the car.
		desvec=vector3(0.0f,0.0f,-5.0f);
		c->mainObject->transform.multiplyV3By3x4mat(&temp2,&desvec);

		// it's then damped from the last camera position, to give a bit of smoothness.

		if(!saveview)tempmorph*=0.3f;
		temp = cam->position*(0.85f+tempmorph) + temp2*(0.15f-tempmorph);

		// then we get the line between the two and normalising it,
		// giving us a unit vector along the line from point-behind to point-in-front

		temp2=lookat - temp;
		temp2.Normalise();

		// now we work out the camera position. It's a point which is n metres behind the lookat point
		// on along the above unit vector, and m metres above the car.

		if(!detachmode)
		{
			switch (cameraview)
			{
			case CAM_INCAR:
				lastview=false;
				break;
			case CAM_OUT1:
				lastview=false;
				cam->position=lookat-temp2*12.0f;
				cam->position.y=c->mainObject->transform.translation.y+1.0f;
				break;
			case CAM_OUT2:
				lastview=false;
				cam->position=lookat-temp2*14.0f;
				cam->position.y=c->mainObject->transform.translation.y+0.5f;
				break;
			case CAM_OUT3:
				lastview=false;
				cam->position=lookat-temp2*14.0f;
				cam->position.y=c->mainObject->transform.translation.y+2.0f;
				break;
			case CAM_OUT4:
				lastview=false;
				cam->position=lookat-temp2*20.0f;
				cam->position.y=c->mainObject->transform.translation.y+1.0f;
				break;

			case CAM_BUMPER:
				lastview=false;
				desvec=vector3(0,0.2f,0.8f);
				cam->minheight=1.8f;
				c->mainObject->transform.multiplyV3By3x4mat(&temp2,&desvec);
				cam->position=temp2;
				break;

			case CAM_PITSTOP:
				lastview=false;
				desvec=vector3(0,0.3f,1);
				cam->minheight=0.8f;
				c->mainObject->transform.multiplyV3By3x4mat(&temp2,&desvec);
				cam->position=temp2;
				break;

			default:
				break;
			}
		}
		else
		{
			if (!lastview) pos=cam->position;
			cam->position=pos;
			lastview=true;
		}

		
		// modify the lookat point according to the view..

		switch (cameraview)
		{

		case CAM_INCAR:
			temp = c->mainObject->transformedattachmentPoint - c->mainObject->concatenatedTransform.translation;
			
			c->mainObject->attached->position = 
				(c->mainObject->concatenatedTransform.translation - temp*3 ) * ( 1.0f - c->SpinCam) + 
				c->mainObject->attached->position * c->SpinCam;

			temp = c->mainObject->transformedattachmentPoint * ( 1.0f - c->SpinCam) +
				c->mainObject->concatenatedTransform.translation * c->SpinCam;
				
			cam->inverseRotMatrix = c->mainObject->transform;
			cam->inverseRotMatrix.translation=vector3(0,0,0);
			cam->RotMatrix.Transpose3x4(&cam->inverseRotMatrix);
			break;
		case CAM_OUT1:
		case CAM_OUT2:
		case CAM_OUT3:
		case CAM_OUT4:
			cam->faceTowardPoint(&lookat);
			break;

		case CAM_BUMPER:
		case CAM_PITSTOP:
			cam->inverseRotMatrix = c->mainObject->transform;
			cam->inverseRotMatrix.translation=vector3(0,0,0);
			cam->RotMatrix.Transpose3x4(&cam->inverseRotMatrix);
			break;
		default:break;
		}

		if(saveview)
		{
			saveview=false;
		}

	}	
	else fatalError("oops! No cam!");
}

void positionMirrorCamera(class car *c)
{
	static vector3 rvect,desvec;

	desvec=vector3(-2.0f,0.0f,-2.0f);
	c->mainObject->transform.multiplyV3By3x4mat(&rvect,&desvec);
	boris2mirror->position=rvect;
	desvec=vector3(-5.0f,0.0f,-30.0f);
	c->mainObject->transform.multiplyV3By3x4mat(&rvect,&desvec);
	boris2mirror->faceTowardPoint(&rvect);

	desvec=vector3(2.0f,0.0f,-2.0f);
	c->mainObject->transform.multiplyV3By3x4mat(&rvect,&desvec);
	boris2mirror2->position=rvect;
	desvec=vector3(5.0f,0.0f,-30.0f);
	c->mainObject->transform.multiplyV3By3x4mat(&rvect,&desvec);
	boris2mirror2->faceTowardPoint(&rvect);
}

