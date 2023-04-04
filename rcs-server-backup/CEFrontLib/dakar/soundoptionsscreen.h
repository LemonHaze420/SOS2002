// $Header

// $Log

class SoundOptionsScreen : public Screen
{
public:
	SoundOptionsScreen(Alphabet *);
	virtual ~SoundOptionsScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	int Counter, SlideCounter, MaxSlideCounter, CurrentSelection;
	int CyclePeriod;
	int Options[5], Values[4];

	int LocalHorizLines[10], LocalVertLines[2], HelpTexts[2];
	int Bars[30];

	int SoundWord;

	CycledBox *SelectionCycler;

	WordList *AllWords;
	SpriteList *AllSprites;

	Alphabet *MainAlphabet;
};