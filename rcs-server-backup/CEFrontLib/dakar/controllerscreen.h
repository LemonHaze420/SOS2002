// $Header

// $Log

class ControllerScreen : public Screen
{
public:
	ControllerScreen(Alphabet *);
	virtual ~ControllerScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	int Counter, CurrentSelection, SlideCounter, MaxSlideCounter, CyclePeriod;

	int LocalHorizLines[6], LocalVertLines[2], HelpTexts[2], Options[5], Values[6];
	int ControlWords[3][8];
	int ControllerWord;

	CycledBox *SelectionCycler;
	WordList *AllWords;
	SpriteList *AllSprites;

	int JoyPad;

	Alphabet *MainAlphabet;
};