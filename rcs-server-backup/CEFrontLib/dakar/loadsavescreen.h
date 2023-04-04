// $Header

// $Log

class LoadSaveScreen : public Screen
{
public:
	LoadSaveScreen(Alphabet *);
	virtual ~LoadSaveScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	int Counter, SlideCounter, MaxSlideCounter;
	int CurrentSelection;
	int CyclePeriod;

	CycledBox *SelectionCycler;

	WordList *AllWords;
	int Options[2];

	Alphabet *MainAlphabet;
};