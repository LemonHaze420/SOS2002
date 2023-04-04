// PositionTable.h: interface for the PositionTable class.
//
// This class holds position data for the cars, and other information
// about the race as a whole.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POSITIONTABLE_H__A512DBA0_FFAC_11D2_8608_0080C86B40F3__INCLUDED_)
#define AFX_POSITIONTABLE_H__A512DBA0_FFAC_11D2_8608_0080C86B40F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PositionTable  
{
public:
	PositionTable();
	int postable[16]; // SDS - we want to be free
	int laps[16];		// the lap the car is on (starting at zero)

	float trackpos[16];	// the distance round the circuit. This is the floating point RLP + laps*numpointsinline
	float realtrackpos[16];

	int rlp1[16];		// the index of the nearest rlp
	int rlp2[16];		// the index of the second nearest rlp
	float dist1[16];		// the distance of the car from its nearest rlp
	float dist2[16];	// the distance of the car from the 2nd nearest rlp
	rlvector rlp1pos[16];	// positions of the nearest and second nearest rlps
	rlvector rlp2pos[16];
	bool oktoadvance[16];
	int pos[16];	// the positions of the car. The front car is position zero.
	int curline[16];	// the racing line the car is using.
	int subline[16];	// the subline being used.

	unsigned long lapbegun[16];	// the time at which the lap started
	unsigned long lapbest[16];
	unsigned long finishtime[16];

	int rlfirst,rlmain,rllast;	// the racing line used for the first lap, middle laps, and last lap
	int nlaps;					// number of laps in the race

	int SplitTime[3][16];		// three splits: track divided into 3, 16 cars.
	
	void Update(int ncars);
	void Initialise(int _rlfirst,int _rlmain,int _rllast,int _nlaps);
	void SortTrackPos(bool copytorealtrackpos=true);

	int SplitDifferenceAhead[16], SplitDifferenceBehind[16];
	bool UpdateLapBegun[16];

	// The split time to display, and the time (in ticks) for which to display it.
	int DisplaySplit[16], DisplaySplitTime[16];
	// The time for which the split time should be displayed for the car AHEAD of this one.
	int DisplaySplitAheadTime[16];

};

extern PositionTable positiontable;

#endif // !defined(AFX_POSITIONTABLE_H__A512DBA0_FFAC_11D2_8608_0080C86B40F3__INCLUDED_)
