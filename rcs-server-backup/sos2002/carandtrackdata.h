// $Header$

// $Log$

#ifndef _CARANDTRACKDATA_H_
#define _CARANDTRACKDATA_H_

// - Car --------------------------------------------------------------------------
// Enumerated and defined car data

// ----------------------------------------------------------------------------
enum	CarColour
{
	Black,
	White,
	Grey,			// Gray
	Red,
	Yellow,
	Green,
	RacingGreen,	// British racing green
	NavyBlue,
	BlueBlue,		// Blue
	SkyBlue,
	Metallic,		// Highly reflective.
};

// ----------------------------------------------------------------------------
// An enumerated type for the car
enum	CarType 
{
	Alfa,			// Known cars
	AmilCar,
	AutoUnion,
	Bugatti,
	Napier
};

// ----------------------------------------------------------------------------
// For getting the right archive
static struct CANames
{	
	char*	_Name_;
	float	_Cost_;
}

CarArchiveNames[] = 
{	
	"Alfa",			144.99f,
	"AmilCar",		179.99f,
	"AutoUnion",	189.95f,
	"Bugatti",		168.95f,
	"Napier",		199.99f//,
};

#define NUMBER_OF_CARS	5

// End car data
// --------------------------------------------------------------------------------


// - Track ------------------------------------------------------------------------
// Track data.

// ----------------------------------------------------------------------------
// List of tracks
enum TrackID
{
	AmericanOval,
	Avus,
//	Bridgehampton,
	Brooklands,
	Donington,
//	IsleOfMan,
	Montlhery,
	Monza,
	Pau,
	Roosevelt,
//	Sitges,
	Spa,
	Tripoli
};

// For getting the right archive
static struct TANames
{	
	char*	_Name_;
	long	_ValidYearsCode_;
	int		_RecommendedLength_;
	float	_QualifyingTime_;
}

// The name of the archive, and a code that defines the years for which it is valid.
TrackArchiveNames[] = 
{	
	"American Oval",	0,	10,	70.0,
	"Avus",				0,	2,	300.0,
//	"Bridgehampton",
	"Brooklands",		0,	10,	100.0,
	"Donington",		0,	6,	180.0,
//	"Isle Of Man",
	"Montlhery",		0,	6,	180.0,
	"Monza",			0,	4,	180.0,
//	"Pau",				0,	4,	120.0,
	"Roosevelt",		0,	6,	180.0,
//	"Sitges",			0,	6,	120.0,
	"Spa",				0,	10,	90.0,
	"Tripoli",			0,	2,	300.0
};

const int NUMBER_OF_TRACKS = sizeof(TrackArchiveNames) / sizeof(TANames);

// Race type enumeration.
enum RaceType
{
	DecadeChallenge,
	Demo,
	GrandPrix,
	HeadToHead,
	QuickRace,
	Practice,
	SingleSeason,
	Qualify,
	TimeTrial,
	NetworkClient,
	NetworkServer
};

// End of track data
// --------------------------------------------------------------------------------

#endif
