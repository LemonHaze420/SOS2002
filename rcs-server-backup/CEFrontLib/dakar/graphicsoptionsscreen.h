// $Header

// $Log

class GraphicsOptionsScreen : public Screen
{
public:
	GraphicsOptionsScreen(Alphabet *);
	virtual ~GraphicsOptionsScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	int Counter, SlideCounter, MaxSlideCounter, CurrentSelection;
	int CyclePeriod;

	int LocalHorizLines[6], LocalVertLines[2], HelpTexts[2], Options[5], Values[6];

	int GraphicsWord;

	CycledBox *SelectionCycler;

	WordList *AllWords;
	SpriteList *AllSprites;

	Alphabet *MainAlphabet;
};