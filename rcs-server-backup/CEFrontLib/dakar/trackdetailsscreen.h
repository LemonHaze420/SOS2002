// $Header

// $Log

class TrackDetailsScreen : public Screen
{
public:
	TrackDetailsScreen(Alphabet *);
	virtual ~TrackDetailsScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	void HideDetails(int SubScreen);
	void ShowDetails(int SubScreen);

	void DrawTrack();
	void SetUpRouteDefaults();
	void ColourRoute();

	int FindJunctionNumber(int Spline);
	void ChooseSpline(int ControlPacket);

	SpriteList *AllSprites;
	WordList *AllWords;

	int Panel[3];
	int RaceModeWord;

	int Counter;
	int MaxSlideCounter, SlideCounter, CyclePeriod;
	int CurrentSelection, SelectionY;

	int WhiteIcons[4], GreyedIcons[4], GreyedBox[4], WhiteBox, OrangeBox[4];
	int SelectionTexts[4];
	Vector IconPositions[4];

	int NameText, StageText;
	int DescriptionTexts[3], DescriptionValues[3];
	int EditText;

	// Car config
	int OldPrefs[4];
	int LocalHorizLines[8], LocalVertLines[4];
	int Titles[2], Headers[4], Values[12];
	int DoneText, CancelText;
	int LocalRedShades[2];

	// Route config
	int RouteWord, NextWord, BackWord, ConfirmWord, CancelWord;


	int DisplayedSubScreen;

	int MapMockup[2];
	SplineSet *TrackSplines;
	int StartingSpline;
	int *SplineColours;
	unsigned char TemporaryRoute[40];

	Vector ViewPortPosition, ViewPortSize;
	Vector ViewedPosition, TargetViewedPosition, ViewedSize;
	Vector2 MapOffset, MapScale;
	int CounterStart;

	CycledBox *SelectionCycler;

	Alphabet *MainAlphabet;
};