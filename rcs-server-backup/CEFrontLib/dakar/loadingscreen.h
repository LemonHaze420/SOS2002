// $Header

// $Log

class LoadingScreen : public Screen
{
public:
	LoadingScreen(Alphabet *);
	virtual ~LoadingScreen();

	virtual void Update(struct Instruction *);
	virtual int ControlPressed(int ControlPacket);

	virtual void DrawBackground();

	virtual void Destroy();

private:
	int Counter, SlideCounter, MaxSlideCounter;
	int CyclePeriod;
	
	int Chevrons[12], LoadingText, BlackBar, ProgressBar, Borders[2];

	SpriteList *AllSprites;

	Alphabet *MainAlphabet;
};