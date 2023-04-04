// $Header

// $Log

#define CCS_CHOOSETYPE	0
#define CCS_CONFIRM		1

class ChooseClassScreen : public Screen
{
public:
	ChooseClassScreen(Alphabet *);
	virtual ~ChooseClassScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	void DrawOneOfXString(int Current, int Selection);
	void LoadModels();
	void MoveVehicle(FrontEndObject *ThisObject);

	SpriteList *AllSprites;

	// That wretched curve in the background!
	int GreyedBox[4], GreyedBoxLid;
	int GreyedVehicles[4], WhiteVehicles[4];
	int WhiteBox;
	Vector VehiclePositions[4];
	int OrangeFade;

	int ChooseClassText;
	int ModeText;
	int VehicleText[4];

	int SelectionY;
	int CyclePeriod;
	CycledBox *SelectionCycler, *RedCycler;

	int SlideCounter, MaxSlideCounter;

	WordList *AllWords;
	int VehicleName, VehicleString, VehicleSelectionString;
	int LiveryString, LiveryChoices[5];
	int ColourString, ColourChoices[5];
	int OKString;

	int HighlightBox;

	int Mode;
	int SelectedVehicleType;
	int MaxOfType[4];

	FrontEndObject *VehicleObject[4]; 

	int CurrentSelection;
	int Counter, LetterCounter;
	
	Alphabet *MainAlphabet;
};