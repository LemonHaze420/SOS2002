// $Header$

// $Log$


#ifndef _CARENTITIY_H_
#define _CARENTITIY_H_

#include "CarAndTrackData.h"
#include "AllCar.h"

// ----------------------------------------------------------------------------
// A structure used to store the data about the car's components.
struct Component
{
	int		Grade;		// 1 - 5 (one being best and -1 for not present).
	float	OE;			// Operational Effectiveness of the component
};

// ------------------------------------------------------------------------------------------
// A CarEntity is used to encapsulate all the data the in-game engine needs to 
// know about the car and also makes provision for the engine to pass info back.
class CarEntity
{
public:
	// --------------------------------------------------------------------------------------
	// Constructor / De-constructor

	// ------------------------------------------------------------------------
	// Default constructor
	CarEntity();

	// ------------------------------------------------------------------------
	// Over loaded constructor.
	CarEntity(	enum CarType	_Model,
				enum CarColour	_Colour);

	// ------------------------------------------------------------------------
	// De constructor
	~CarEntity(){}

	// End Section
	// --------------------------------------------------------------------------------------


	// - Selectors --------------------------------------------------------------------------
	// Retrieve data from the class.
	
	// ----------------------------------------------------------------------------
	// Returns a pointer to the parent object, if set. (Will return NULL if not set).
	void* GetParent()
	{
		return PointerToParentObject;
	}

	// ------------------------------------------------------------------------
	// Returns the make of the car
	enum CarType	GetModel()
	{
		return CarModel;
	}

	// ------------------------------------------------------------------------
	// Returns the car colour
	enum CarColour	GetColour()
	{
		return Colour;
	}

	// ------------------------------------------------------------------------
	// Get activity of the object
	bool GetActivity()
	{
		return Active;
	}

	// ------------------------------------------------------------------------
	// Returns the pointer to the car data. (3D model data etc...)
	car* GetCarData()
	{
		return CarData;
	}

	// ------------------------------------------------------------------------
	// Get the class of the car
	int GetCarClass()
	{
		return CarClass;
	}

	// ------------------------------------------------------------------------
	// Get the grid position
	int GetStartingGridPosition()
	{
		return StartingPosition;
	}

	// ------------------------------------------------------------------------
	// Get the finishing position (negative one to indicate failed to complete race)
	int GetFinishingPosition()
	{
		return FinishingPosition;
	}

	// ------------------------------------------------------------------------
	// Returns the number of laps completed.
	int GetNumberOfLapsCompleted();

	// ------------------------------------------------------------------------
	// Get the time it took to complete the specified lap.
	float GetLapTime(int _LapNumber)
	{
		return LapTimes[_LapNumber];
	}

	// ------------------------------------------------------------------------
	// Get the time it took for you to complete the race.
	float GetRaceTime();

	// ------------------------------------------------------------------------
	// A racing style indicator.
	int GetRaceStyleInformation()
	{
		return RaceStyleInformation;
	}

	// ------------------------------------------------------------------------
	// Returns the component's grade.
	int GetComponentGrade(int _Component)
	{
		return Components[_Component].Grade;
	}

	// ------------------------------------------------------------------------
	// Gets the operational effectiveness for components to the given values.
	// Operational effectiveness goes from 0.0(trashed) -> 1.0(perfect)
	float GetComponentOperationalEffectiveness(int _Component)
	{
		return Components[_Component].OE;
	}

	// ------------------------------------------------------------------------
	// Get the pointer to the list of components
	// NOTE:	This method can also be considered a manipulator as 
	//			the user is free to modify the content of the list.
	Component* GetComponentsPtr()
	{
		return Components;
	}

	// ------------------------------------------------------------------------
	// Get the pointer to the gear ratios
	// NOTE:	This method can also be considered a manipulator as 
	//			the user is free to modify the content of the list.
	float* GetGearRatiosPtr()
	{
		return GearRatios;
	}

	// ------------------------------------------------------------------------
	// Get the fuel load
	float GetFuel()
	{
		return Fuel;
	}

	// ------------------------------------------------------------------------
	// Get the amount of engine oil
	float GetEngineOil()
	{
		return EngineOil;
	}

	// ------------------------------------------------------------------------
	// Get the Name of the car
	char *GetName()
	{
		return CarName;
	}

	// End section
	// --------------------------------------------------------------------------------------

	
	// - Manipulators -----------------------------------------------------------------------
	// Methods that manipulate the data held within an instantiation of the CarEntity class

	// ------------------------------------------------------------------------
	// Set pointer to parent object (Not very Java-like).
	void SetParent(void* _ParentPtr)
	{
		PointerToParentObject = _ParentPtr;
	}

	// ------------------------------------------------------------------------
	// Copies the data from one object to another. (Very Java-like).
	void Clone(CarEntity* _To);

	// ------------------------------------------------------------------------
	// Set activity of the object
	void SetActivity(bool _Active)
	{
		Active = _Active;
	}

	// ------------------------------------------------------------------------
	// Set the car model
	void SetModel(enum CarType _Model)
	{
		CarModel = _Model;
	}

	// ------------------------------------------------------------------------
	// Move one position forward in the model enumeration.
	// The parameter, which is by default the last car in the enum,
	// describes the model it will wrap around to when the user 
	// requests to move off the end of the enum.
	void NextModel(enum CarType _Model = (enum CarType)(NUMBER_OF_CARS - 1));

	// ------------------------------------------------------------------------
	// Move one position back in the model enumeration.
	// The parameter, which is by default the fist car in the enum,
	// describes the model it will wrap around to when the user 
	// requests to move off the front of the enum.
	void PreviousModel(enum CarType _Model = (enum CarType)(0));

	// ------------------------------------------------------------------------
	// Set the colour
	void SetColour(enum CarColour _Colour)
	{
		Colour = _Colour;
	}

	// ------------------------------------------------------------------------
	// Set the class of the car
	void SetCarClass(int _CarClass)
	{
		CarClass = _CarClass;
	}

	// ------------------------------------------------------------------------
	// Set the grid position
	void SetStartingGridPosition(int	_StartingPosition)
	{
		StartingPosition	=	_StartingPosition;
	}

	// ------------------------------------------------------------------------
	// Set the finishing position (negative one to indicate failed to complete race)
	void SetFinishingPosition(int	_FinishingPosition)
	{
		FinishingPosition	=	_FinishingPosition;
	}

	// ------------------------------------------------------------------------
	// Set the time it took to complete the lap
	void SetLapTime(float _Seconds);

	// ------------------------------------------------------------------------
	// A racing style indicator.
	// Right, setting a particular value will indicate to the FrontEnd that the
	// driver performed in a particular way. 
	// So, it could be the value one(1), to indicate that the driver was fast 
	// and accurately took corners, etc... ...And six(6) could represent that
	// the driver crashed the car in to a wall and totalled the car.
	void SetRaceStyleInformation(int	_Summary)
	{
		RaceStyleInformation = _Summary;
	}

	// ------------------------------------------------------------------------
	// Sets the component's grade.
	void SetComponentGrade(	int _Component, 
							int _Grade)
	{
		Components[_Component].Grade = _Grade;
	}

	// ------------------------------------------------------------------------
	// Sets the operational effectiveness for components to the given values.
	// Operational effectiveness goes from 0.0(trashed) -> 1.0(perfect)
	void SetComponentOperationalEffectiveness(	int _Component, 
												float _OperationalEffectiveness)
	{
		Components[_Component].OE = _OperationalEffectiveness;
	}

	// ------------------------------------------------------------------------
	// Modify the operational effectiveness for a component to the given value.
	// Operational effectiveness goes from 0.0(trashed) -> 1.0(perfect).
	// Passing in values from -1.0 -> 1.0 will modify the component's OE by 
	// that value. NOTE: Does error checking to maintain OE bounds.
	void ModifyComponentOperationalEffectiveness(	int _Component, 
													float _OperationalEffectiveness);

	// ------------------------------------------------------------------------
	// Load up the 3D model. 
	bool LoadModel();

	// ------------------------------------------------------------------------
	// Free the car model data
	bool FreeModel();

	// ------------------------------------------------------------------------
	// SetControlType sets data that is used to create the 
	// appropriate controller data when the cars are loaded.
	void SetControlType(int _ControlType)
	{
		ControllerType = _ControlType;
	}

	// ------------------------------------------------------------------------
	// Set the fuel load
	void SetFuel(float _Fuel)
	{
		Fuel = _Fuel;
	}

	// ------------------------------------------------------------------------
	// Set the amount of engine oil
	void SetEngineOil(float _EngineOil)
	{
		EngineOil = _EngineOil;
	}

	// ------------------------------------------------------------------------
	// Set the Name of the car
	void SetName(char *NewName)
	{
		strcpy(CarName, NewName);
		return;
	}
	
	
	// End section
	// --------------------------------------------------------------------------------------


private:

	// Link back to the parent object.
	void*	PointerToParentObject;

	// - GENERAL DATA ---------------------------------------------------------
	bool		Active;
	CarType		CarModel;			// Bugatti?
	CarColour	Colour;				// Yellow?
	int			CarClass;			// C-class

	#define		HUMAN		0
	#define		AI			1

	int			ControllerType;

	
	// - REPORT DATA ----------------------------------------------------------
	int			StartingPosition;
	int			FinishingPosition;
	float		LapTimes[64];			// A maximum of 64 laps
	int			NumberOfLapsCompleted;
	int			RaceStyleInformation;

	// - MECHANICAL DATA ------------------------------------------------------
	// The number for the Components list.
	#define	C_ENGINE		0
	#define	C_TYRE			1
	#define	C_BRAKE			2
	#define	C_SUSPENSION	3
	#define	C_SUPERCHARGER	4
	#define	C_GEARBOX		5
	#define	C_BODYWORK		6
	#define	C_WHEEL			7

	Component	Components[8];	// A list of components

	float		GearRatios[8];		// Ratios of gears - Up to eight, how generous
	float		Fuel;				// Kg or Litres, etc
	float		EngineOil;			// "	"	"	 "

	char		CarName[50];

	// - GRAPHICAL DATA -------------------------------------------------------
	car*		CarData;
	spriteFX*	sparksFX;
};
#endif
// End Section
// ------------------------------------------------------------------------------------------