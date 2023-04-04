// $Header

// $Log

class OptionsScreen : public Screen
{
public:
	OptionsScreen(Alphabet *);
	virtual ~OptionsScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	int Counter, SlideCounter, MaxSlideCounter, CurrentSelection;
	int CyclePeriod;
	int Options[4];
	int OptionWord;

	int LocalHorizLines[8], LocalVertLines[2], HelpTexts[2];

	CycledBox *SelectionCycler;

	WordList *AllWords;
	SpriteList *AllSprites;

	Alphabet *MainAlphabet;
};