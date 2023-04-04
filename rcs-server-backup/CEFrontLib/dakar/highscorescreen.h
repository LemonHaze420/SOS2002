// $Header

// $Log

class HighScoreScreen : public Screen
{
public:
	HighScoreScreen(Alphabet *);
	virtual ~HighScoreScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	int Counter;

	// Orange Arrows rule! ;-)
	int OrangeArrow;
	int OrangeFade, HiScoreWord;

	LineList *AllLines;
	int HorizontalLines[11];

	int PositionWord, PlayerWord, TotalTimeWord, TracksWord;
	int PositionWords[10], PlayerWords[10], TotalTimeWords[10], TracksWords[10];
	int ModeWord, ExitWord, ChosenModeWord[3];

	int CurrentMode;
	char ModeWords[3][20];

	int CurrentSelection;

	SpriteList *AllSprites;
	WordList *AllWords;

	int SlideCounter, MaxSlideCounter;

	Alphabet *MainAlphabet;
};