// $Header

// $Log

#define TIME_ATTACK 0
#define ARCADE		1
#define CAMPAIGN	2
#define OPTIONS		3
#define LOAD		4

class MainScreen : public Screen
{
public:
	MainScreen(Alphabet *);
	virtual ~MainScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	SpriteList *AllSprites;
	int Buttons[5];
	int Borders[5];
	int Texts[5];
	int SelectionBorder;
	CycledBox *SelectionBox;

	int CurrentSelection, ShownSelection;
	int BoxCurrentX;

	int TextSpin, ButtonSpin, MaxTextSpin, MaxButtonSpin;

	int Counter, CyclePeriod;
	Alphabet *MainAlphabet;

	Word *TestWord;
};