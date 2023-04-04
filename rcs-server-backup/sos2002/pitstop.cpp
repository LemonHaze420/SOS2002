/* Pitstop.cpp

	Routines to govern the pitstops, viz. starting, stopping and
	updating them.

	Ian Gledhill   13/07/1999

	Changed for new graphics and methods
	Andy Nicholas  21/09/1999

	Broadsword Interactive Ltd */

#include <windows.h>

#include "ALLcar.h"
#include <rgraphics.h>

#include "controller.h"
#include "Pitstop.h"

extern car *cararray[];

extern int ticksthisframe,resX;

Pitstop::Pitstop(int _CarNo, PitstopGraphics *_Graphics)
{
	// Save the variables.
	CarNo = _CarNo;
	Graphics = _Graphics;
	tyrechanging=false;

	// 10 seconds for this pitstop.
	TimeRemaining = TICKRATE * 10.0f;
}

bool Pitstop::Update()
{
	if (TimeRemaining)
		return true;
	else
		return false;
}

void Pitstop::Render(bool dorender)
{
	// Shade the screen.

	// quick hack 
	if(!dorender)return;

	if(dorender)
	{
#if 0
		if(resX == 512)
			DarkenScreen(115,115,280,180);	
		else
			DarkenScreen(180,180,440,280);	
#endif
		//Fuel, Oil, Car
		Graphics->RenderBitmap(230,195,3,0);
		Graphics->RenderBitmap(334,230,2,0);
		Graphics->RenderBitmap(480,218,9,0);
		
		//F5, F6, F7, F8
		Graphics->RenderBitmap(248,420,4,0);
		Graphics->RenderBitmap(352,420,5,0);
		
		Graphics->RenderBitmap(567,226,6,0);
		Graphics->RenderBitmap(567,368,10,0);


		Graphics->RenderPercent(487,196,cararray[CarNo]->tyresfront);
		Graphics->RenderPercent(487,430,cararray[CarNo]->tyresback);
	}		

	if(tyrechanging)
	{
		if((cararray[CarNo]->tyresfront>=100.0f) && frontchange) tyrechanging=false;
		if((cararray[CarNo]->tyresback>=100.0f) && !frontchange) tyrechanging=false;
	}

	int tyrestate[4];
	for(int i=0;i<2;i++)
	{
		if (tyrechanging && frontchange)
		{
			if ((cararray[CarNo]->tyresfront*0.02f - 1)>i) tyrestate[i]=1;
			else if ( ((cararray[CarNo]->tyresfront*0.02f)>i) && ((tyrechcount%29)<15) ) tyrestate[i]=1;
			else tyrestate[i]=0;
			tyrechcount++;
			cararray[CarNo]->tyresfront += (100 / 15 / TICKRATE) * ticksthisframe;
		}
		else if(cararray[CarNo]->tyresfront>=100.0f) tyrestate[i]=1;
		else tyrestate[i]=0;
	}

	for(i=2;i<4;i++)
	{
		if (tyrechanging && !frontchange)
		{
			if ((cararray[CarNo]->tyresback*0.02f - 1)>(i-2)) tyrestate[i]=1;
			else if ( ((cararray[CarNo]->tyresback*0.02f)>(i-2)) && ((tyrechcount%29)<15) ) tyrestate[i]=1;
			else tyrestate[i]=0;
			tyrechcount++;
			cararray[CarNo]->tyresback += (100 / 15 / TICKRATE)*ticksthisframe;
		}
		else if(cararray[CarNo]->tyresback>=100.0f) tyrestate[i]=1;
		else tyrestate[i]=0;
	}
	
	//Tyres, TL, TR, BL, BR
	if(dorender)
	{
		Graphics->RenderBitmap(458,210,tyrestate[0],0);
		Graphics->RenderBitmap(534,210,tyrestate[1],0);
		Graphics->RenderBitmap(452,350,tyrestate[2],0);
		Graphics->RenderBitmap(540,350,tyrestate[3],0);
	}


	long pitkey=0;

	if(cararray[CarNo]->controller->type == CONTROLLER_REPLAY)
	{
		pitkey = ((ReplayController *)(cararray[CarNo]->controller))->GetPitKeys();
	}
	
	if ((cararray[CarNo]->fuel < 100.0f) && ((GetKeyState(VK_F5) & 0x8000) || (pitkey&1)) )
	{
		pitkey |=1;
		cararray[CarNo]->fuel += (100 / 15 / TICKRATE)*ticksthisframe;
		cararray[CarNo]->outoffuelct=0;
	}
	if (cararray[CarNo]->fuel > 100.0f)
		cararray[CarNo]->fuel = 100.0f;

	if ((cararray[CarNo]->oil < 100.0f) && ((GetKeyState(VK_F6) & 0x8000) || (pitkey&2)))
	{
		pitkey|=2;
		cararray[CarNo]->oil += (100 / 4 / TICKRATE)*ticksthisframe;
	}
	if (cararray[CarNo]->oil > 100.0f)
		cararray[CarNo]->oil = 100.0f;

	if ((cararray[CarNo]->tyresfront < 100.0f) && ((GetKeyState(VK_F7) & 0x8000) || (pitkey&4)) && !tyrechanging)
	{
		pitkey|=4;
		tyrechanging=true;
		frontchange=true;
		tyrechcount=0;
		cararray[CarNo]->tyresfront=0.0f;
	}

	if ((cararray[CarNo]->tyresback < 100.0f) && ((GetKeyState(VK_F8) & 0x8000) || (pitkey&8)) && !tyrechanging)
	{
		pitkey|=8;
		tyrechanging=true;
		frontchange=false;
		tyrechcount=0;
		cararray[CarNo]->tyresback=0.0f;
	}

	cararray[CarNo]->controller->SetPitKeys(pitkey);

	//	if ((cararray[CarNo]->tyres < 100.0f) && (GetKeyState(VK_F7) & 0x8000))
//		cararray[CarNo]->tyres += 100 / 4 / TICKRATE;
	if (cararray[CarNo]->tyresfront > 100.0f)
		cararray[CarNo]->tyresfront = 100.0f;

	if (cararray[CarNo]->tyresback > 100.0f)
		cararray[CarNo]->tyresback = 100.0f;

/*
	float TimeSpent = ((10*TICKRATE) - TimeRemaining) / TICKRATE;

	if (TimeSpent < 2.5f)
	{
		Fuel = TimeSpent * 40;
	}
	else if (TimeSpent < 5.0f)
	{
		Fuel = 100;
		Water = (TimeSpent-2.5f) * 40;
	}
	else if (TimeSpent < 7.5f)
	{
		Fuel = 100;
		Water = 100;
		Oil = (TimeSpent-5.0f) * 40;
	}
	else if (TimeSpent < 10.0f)
	{
		Fuel = 100;
		Water = 100;
		Oil = 100;
		Tyres = (TimeSpent-7.5f) * 40;
	}
*/
	
	float Fuel = cararray[CarNo]->fuel;
	float Tyresf = cararray[CarNo]->tyresfront;
	float Tyresb = cararray[CarNo]->tyresback;
	float Oil = cararray[CarNo]->oil;

	if (Fuel >= 100.0f && Tyresf >= 100.0f && Tyresb >= 100.0f && Oil >= 100.0f)
		TimeRemaining = 0;

	if(dorender)
	{
		Graphics->RenderGauge(247,281,cararray[CarNo]->fuel);
		Graphics->RenderGauge(352,281,cararray[CarNo]->oil);
	}
}

void Pitstop::Cancel()
{
	TimeRemaining = 0;
}

PitstopGraphics::PitstopGraphics(char *ArchiveName)
{
	// Push the archive onto the stack.
	arcPush(ArchiveName);

	// Load the actual bitmaps into memory.
	Bitmap1 = getTexturePtr("Pitstop01",false,false)->textureHandle;
	Bitmap2 = getTexturePtr("Pitstop02",false,false)->textureHandle;

	// Don`t need this archive any more.
	archivestack.Pop();
}

void PitstopGraphics::RenderBitmap(int x, int y, int Icon, float pcnt)
{
	float val1=106.0f  - 0.88f*pcnt;
	
	float val2=242.0f - 0.88f*pcnt;
	float val3=0.88f*pcnt + 9.0f;
	float val4=y+val1;

	switch(Icon)
	{
	case 0: //Tyre Empty
		Render1to1(x,y,0,1,20,56,Bitmap1,0.0078125f,0.0078125f);
		break;	

	case 1: //Tyre Full
		Render1to1(x,y,20,1,20,56,Bitmap1,0.0078125f,0.0078125f);
		break;	

	case 2: //Oil Can
		Render1to1(x,y,1,79,85,38,Bitmap1,0.0078125f,0.0078125f);
		break;	

	case 3: //Fuel Pump
		Render1to1(x,y,42,1,50,73,Bitmap1,0.0078125f,0.0078125f);
		break;	

	case 4: //F5
		Render1to1(x,y,95,98,31,25,Bitmap1,0.0078125f,0.0078125f);
		break;	

	case 5: //F6
		Render1to1(x,y,95,71,31,25,Bitmap1,0.0078125f,0.0078125f);
		break;	

	case 6: //F7
		Render1to1(x,y,95,44,30,24,Bitmap1,0.0078125f,0.0078125f);
		break;	

	case 7: //Gauge Empty
		Render1to1(x,y,90,15,31,val1,Bitmap2,0.0078125f,0.00390625f);
		break;	

	case 8: //Gauge Full
		Render1to1(x,val4,90,val2,31,val3,Bitmap2,0.0078125f,0.00390625f);
		break;	

	case 9: //Car
		Render1to1(x,y,24,25,61,207,Bitmap2,0.0078125f,0.00390625f);
		break;	

	case 10: //F8
		Render1to1(x,y,95,17,30,24,Bitmap1,0.0078125f,0.0078125f);
		break;	

	case 11: //Number 0
		Render1to1(x,y,6,173,9,13,Bitmap2,0.0078125f,0.00390625f);
		break;	

	case 12: //Number 1
		Render1to1(x,y,6,11,9,13,Bitmap2,0.0078125f,0.00390625f);
		break;	

	case 13: //Number 2
		Render1to1(x,y,6,29,9,13,Bitmap2,0.0078125f,0.00390625f);
		break;	

	case 14: //Number 3
		Render1to1(x,y,6,47,9,13,Bitmap2,0.0078125f,0.00390625f);
		break;	

	case 15: //Number 4
		Render1to1(x,y,6,65,9,13,Bitmap2,0.0078125f,0.00390625f);
		break;	

	case 16: //Number 5
		Render1to1(x,y,6,83,9,13,Bitmap2,0.0078125f,0.00390625f);
		break;	

	case 17: //Number 6
		Render1to1(x,y,6,101,9,13,Bitmap2,0.0078125f,0.00390625f);
		break;	

	case 18: //Number 7
		Render1to1(x,y,6,119,9,13,Bitmap2,0.0078125f,0.00390625f);
		break;	

	case 19: //Number 8
		Render1to1(x,y,6,137,9,13,Bitmap2,0.0078125f,0.00390625f);
		break;	

	case 20: //Number 9
		Render1to1(x,y,6,155,9,13,Bitmap2,0.0078125f,0.00390625f);
		break;	

	case 21: //Percent
		Render1to1(x,y,2,211,18,18,Bitmap2,0.0078125f,0.00390625f);
		break;	
	}
}

void PitstopGraphics::RenderGauge(int x, int y, float Fullness)
{
	RenderBitmap(x, y, 7, Fullness);
	RenderBitmap(x, y, 8, Fullness);

}

void PitstopGraphics::RenderPercent(int x, int y, float pcnt)
{
	int digit1,digit2,digit3;
	float pc;

	if(pcnt>100.0f) pcnt=100.0f;
	if(pcnt<0.0f) pcnt=0.0f;

	pc=pcnt;
	
	digit1=(int)floor(pc/100.0f);
	pc-=100.0f*digit1;

	digit2=(int)floor(pc/10.0f);
	pc-=10.0f*digit2;

	digit3=pc;

	if(resX==512)
	{
		if(digit1>0) RenderBitmap(x-5,y-3,digit1+11,0);
		if(digit2>0 || digit1>0) RenderBitmap(x+9,y-3,digit2+11,0);
		RenderBitmap(x+23,y-3,digit3+11,0);
	
		RenderBitmap(x+40,y-5,21,0);
	}
	else
	{
		if(digit1>0) RenderBitmap(x,y,digit1+11,0);
		if(digit2>0 || digit1>0) RenderBitmap(x+9,y,digit2+11,0);
		RenderBitmap(x+18,y,digit3+11,0);
	
		RenderBitmap(x+29,y-2,21,0);
	}
}

void PitstopGraphics::Render1to1(int x, int y,int xp,int yp, int w, int h, int BitmapNumber, float ibx, float iby)
{
	
	float ux=xp*ibx;
	float uy=yp*iby;
	float uw=w*ibx;
	float uh=h*iby;

	if(resX==512)
	{
		if((w==9 && h == 13) || (w == 18 && h == 18))
		{
			w=w*800/512;
			h=h*800/512;
		}
	}
	boris2->addTo2Dlist(x,y,w,h,ux,uy,uw,uh,BitmapNumber);

}


