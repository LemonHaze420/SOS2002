// PositionTable.cpp: implementation of the PositionTable class.
//
//////////////////////////////////////////////////////////////////////

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
#include "carsound.h"
#include "param.h"
#include "PositionTable.h"


extern car *cararray[];
extern gamesetup *game;

extern void dprintf(char *,...);
PositionTable positiontable;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//
//	it might look a little odd that the initial lap is set to -2 for each
//	car. Let me explain. When the game is started, the last rlp1 is so far
//	away from the current rlp1 (cos rlp1 is set to a silly value) that
//	the lap counter immediately increments to -1. Then, when we cross the
//	start/finish line at the start of the race, it goes up to 0.
//
//

extern unsigned long Now();
extern unsigned long start_of_race;
extern car *cameracar;

PositionTable::PositionTable()
{
	for(int i=0;i<16;i++)
	{
		rlp1[i]=100000;
		laps[i]=-2;
		lapbegun[i]=Now();
		lapbest[i]=0;
		trackpos[i]=0;
		pos[i]=0;
		oktoadvance[i]=true;	// so we can start the race
	}

	for (int j=0 ; j<16 ; j++)
	{
		for (i=0 ; i<3 ; i++)
		{
			SplitTime[i][j] = -1;
		}
		SplitDifferenceAhead[j] = -1;
		SplitDifferenceBehind[j] = -1;
		UpdateLapBegun[j] = false;

		DisplaySplit[j] = -1;
		DisplaySplitTime[j] = 0;
		DisplaySplitAheadTime[j] = 0;
	}
}

void PositionTable::Initialise(int rlf,int rlm,int rll,int nl)
{
	rlfirst = rlf;
	rlmain = rlm;
	rllast = rll;
	nlaps = nl;

	for(int i=0;i<16;i++)
	{
		lapbest[i]=0;
		rlp1[i]=100000;
		laps[i]=-2;
		trackpos[i]=0;
		finishtime[i]=0;
		lapbegun[i]=Now();
		pos[i]=0;
		curline[i] = rlfirst;
		subline[i] = 0;
		oktoadvance[i]=true;	// so we can start the race
	}

}

static int __cdecl sortpositions(const void *a,const void *b)
{
	return positiontable.trackpos[*(const int *)a] < positiontable.trackpos[*(const int *)b];
}

void PositionTable::SortTrackPos(bool copytorealtrackpos)
{
	// note - this will destroy the trackpos data

	int i,fnd;

	if(copytorealtrackpos)
	{
		for(i=0;i<game->numcars;i++)
		{
			realtrackpos[i] = trackpos[i];	// these values get lost later
			postable[i]=i;
		}
	}

	for(i=0;i<game->numcars;i++)
	{
		float largestpos=-99999.0f;
		for(int j=0;j<game->numcars;j++)
		{
			if(trackpos[j]>largestpos)
			{
				largestpos=trackpos[j];
				fnd=j;
			}
		}
		trackpos[fnd]=-100000.0;
		postable[i]=fnd;
	}

	for(i=0;i<game->numcars;i++)
		pos[postable[i]] = i;
}

void PositionTable::Update(int ncars)
{

	extern bool practice_mode;

	// update the positional data for each car.

	//int postable[16];
	for(int i=0;i<ncars;i++)
	{
		int line;
//		int subline=0;
		int lastrlp = rlp1[i];

		// work out which line to use
		line = curline[i];		

		int pointsperline = racinglineset.getLine(line)->pointsPerLine;

		float rlpf = 
			racinglineset.getLine(line)->getNearest2RacingLinePoints(subline[i],
				&(cararray[i]->mainObject->transform.translation),
				rlp1[i],
				&rlp1pos[i],
				&rlp2pos[i],
				&rlp1[i],
				&rlp2[i],
				&dist1[i],
				&dist2[i]);

		// determine if we're going the wrong way!#

		if(abs(rlp1[i]-rlp2[i]) < 5)
		{
			vector3 zaxis(0,0,1),carvec,rlv;
			cararray[i]->mainObject->transform.multiplyV3By3x3mat(&carvec,&zaxis);	// get zaxis of car
			rlv = rlp2pos[i]-rlp1pos[i];

			float dotprodthing = rlv.DotProduct(&carvec);
			if(rlp1[i] < rlp2[i])
				dotprodthing *= -1;

			if(cararray[i]->mph<0)
				dotprodthing *= -1;

			if(dotprodthing>0)
				cararray[i]->wrongwayct++;
			else
				cararray[i]->wrongwayct=0;
		}



		if (rlfirst != rlmain)
		{
			if (nlaps != 1)
			{
				if (laps[i] <= 0 && rlp1[i] < 31 && curline[i] == rlmain)
				{
					rlp1[i] += 5;
					rlp2[i] += 5;
				}
				if (laps[i] >= nlaps-1 && rlp1[i] >= 115 && curline[i] == rlmain)
				{
					rlp1[i] -= 0;
					rlp2[i] -= 0;
					curline[i] = rllast;
				}
			}
			
			if (nlaps == 1)
				curline[i] = rlfirst;
			else if (laps[i]==0 && rlp1[i] > 30)
				curline[i] = rlmain;
			else if (laps[i]==0 && rlp1[i] <=40 || laps[i] < 0)
			{
				curline[i] = rlfirst;
			}
			else if (laps[i] >= nlaps-1 && rlp1[i] >= 105 || curline[i] == rllast && rlp1[i] > 80 || laps[i] >= nlaps)
			{
				curline[i] = rllast;
			}
			else
				curline[i] = rlmain;
		}

		else
			curline[i] = rlmain;


		// if we've gone over the halfway mark, it's ok to increment the lap counter when
		// we cross the line in the +ve direction

		if((rlp1[i] > pointsperline/2-10) && (rlp1[i]<pointsperline/2+10))
			oktoadvance[i]=true;

		// work out how far this has moved, so we can update the lap counter.

		lastrlp -= rlp1[i];

		if(lastrlp>=0)
		{
			if(oktoadvance[i] && (lastrlp >pointsperline/2))
			{
				unsigned long tt;
				tt = Now() - lapbegun[i];
				UpdateLapBegun[i] = true;
				SplitTime[0][i] = -1;
				SplitTime[1][i] = -1;
				SplitTime[2][i] = -1;

				if(laps[i]>=0 && (!lapbest[i] || tt < lapbest[i]))lapbest[i]=tt;
				laps[i]++;
				if(laps[i]>=0)oktoadvance[i] = false;
			}
		}

		trackpos[i] = (float)(laps[i]*pointsperline) + rlpf;

	}

	positiontable.SortTrackPos();

	// ############################################
	// Split Time code

	int pointsperline = racinglineset.getLine(0)->pointsPerLine;
	int CarInFront;

	for (i=0 ; i<ncars ; i++)
	{
		if (DisplaySplitTime[i])
			DisplaySplitTime[i]--;
		if (DisplaySplitAheadTime[i])
			DisplaySplitAheadTime[i]--;

		// Do 3 loops or 2 if we're on the first lap.

		for (int j=0 ; j<3 ; j++)
		{
			if (j || laps[i])
			{
				// If the raceline point of the car in the position in front of the cameracar is 1/3rd of the way 
				// through the track, and we don`t have a split time already.
				if (rlp1[i] == (int)((float)pointsperline * 0.3333333f*(float)j) && SplitTime[j][i] == -1)
				{
					SplitTime[j][i] = Now();//	 - lapbegun[i];
					
					DisplaySplit[i] = SplitTime[j][i] - lapbegun[i];
					// 4 seconds
					DisplaySplitTime[i] = TICKRATE * 4;
					SplitDifferenceAhead[i] = -1;

					// Now check if the car in front has already passed the split line.
					if (pos[i] > 0) // if not the first car
					{
						CarInFront = postable[pos[i]-1];
						// There's a car in front - it must have passed the split line.
						// Take the differences and set the useful variables.

						// ONLY IF the car in front is on the same lap. The first split is a special
						// case, because it's the start of the lap - the car in front should be one
						// lap in front.

						if( (!j && (laps[i]==laps[CarInFront]-1)) || (j && (laps[i]==laps[CarInFront])))
						{
							if (SplitTime[j][CarInFront] != -1)
							{
								SplitDifferenceBehind[i] = SplitTime[j][i] - SplitTime[j][CarInFront];

								SplitDifferenceAhead[CarInFront] = SplitDifferenceBehind[i];
								DisplaySplitAheadTime[CarInFront] = TICKRATE * 4;
							}
						}
					}
					else
						SplitDifferenceBehind[i] = -1;
					
				}
			}
		}
		if (UpdateLapBegun[i])
		{
			lapbegun[i] = Now();
			UpdateLapBegun[i] = false;
		}
	}
}
