/*
	$Header$
	$Log$
	Revision 1.0  2000-02-21 12:34:16+00  jjs
	Initial revision

	Revision 1.1  1999/03/29 09:23:49  jcf
	Initial revision



*/
#ifndef __FRONTEND_H__
#define __FRONTEND_H__ 

/****Mon Mar 15 13:45:17 1999*************(C)*Broadsword*Interactive****
 *
 * Front end data structures. LIABLE TO CHANGE WITHOUT NOTICE.
 *
 ***********************************************************************/

#define	NUMCARS	16	// maximum number of cars

struct carsetup	// used within 'gamesetup' to describe each car
{
	char carname[32];	// name of car archive
	bool isplayer;		// true if player, false if AI (NETWORK NOT DONE YET)
};
 
struct gamesetup
{
	char trackname[32];	// name of track archive
	int numcars;		// number of cars in race
	int laps;		// number of laps in race
	unsigned long flags;	// to be defined (GSF_*)

	carsetup cars[NUMCARS];	// describes the cars
};

#define	GSF_REPLAY	1



/// this structure indicates the current game state - it contains the pre- and post-race countdown
/// timer, among other things. Just think of it as "race-related globals".

typedef enum { RACE_BEFORE,		// the countdown is in progress. No car can change out of neutral
				RACE_DURING,	// the race is in progress
				RACE_RACEWON,	// *someone* has one the race.. wait for the player to cross the line next
				RACE_AFTER,		// one or more cars have finished
				RACE_FINISHED,	// No rendering spin the paper
				RACE_PAPER1,
				RACE_PAPER2
} racestate;


struct raceglobalstruct
{
	racestate state;
	int counter;	// counts down from N to zero, when the race starts
	int playerlastlap;	// used to transition from RACE_RACEWON to RACE_AFTER
	int lastcounter;
};

extern raceglobalstruct raceglobals;
#endif
