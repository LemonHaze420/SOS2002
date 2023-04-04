// $Header

// $Log

// Default screen class

#include "../ConsoleFrontLib.h"

LoadSaveScreen::LoadSaveScreen(Alphabet *_MainAlphabet)
{
	MainAlphabet = _MainAlphabet;

	AllWords = new WordList();

	AllWords->AddWord(LargeFont, "Load or Save Screen", &Vector(32.0f, 52.0f, 1.0f), NULL, 0xFFA0A0A0);

	Options[0] = AllWords->AddWord(LargeFont, "Load Game", &Vector(200.0f, 120.0f, 1.0f), NULL, 0xFFA0A0A0);
	Options[1] = AllWords->AddWord(LargeFont, "Save Game", &Vector(200.0f, 150.0f, 1.0f), NULL, 0xFFA0A0A0);

	SelectionCycler = new CycledBox(0xFFFFFFFF, 0xFF444444, 0x00A0A040, 0xFFFFFFFF);

	// Reset the counter to zero.
	Counter = 0;
	SlideCounter = MaxSlideCounter = 0;
	CyclePeriod = 30;
	CurrentSelection = 0;

	// Use the default background.
	UseDefaultBackdrop = true;
}

LoadSaveScreen::~LoadSaveScreen()
{
}

void LoadSaveScreen::Update(struct Instruction *ScreenCommand)
{
	float NewCounter;
	NewCounter = (float)(Counter%(2*CyclePeriod));
	if (NewCounter > CyclePeriod) NewCounter = (float)(2*CyclePeriod - NewCounter);
	NewCounter = NewCounter/(float)CyclePeriod;

	AllWords->SetColour(Options[CurrentSelection], SelectionCycler->CurrentColour(NewCounter));
	if (FadeOut)
	{
		SlideCounter++;
		
		if (SlideCounter > 1)
		{
			Destroy();
		
			QueuedCommand = 'D';
			QueuedValue = 0;
		}
	}
	else
		AllWords->DrawAll();

	Counter++;

	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int LoadSaveScreen::ControlPressed(int ControlPacket)
{
	int OldCurrentSelection = CurrentSelection;
	
	if (ControlPacket & 1 << CON_DOWN)
	{
		if (CurrentSelection < 1) CurrentSelection++;
	}

	if (ControlPacket & 1 << CON_UP)
	{
		if (CurrentSelection) CurrentSelection--;
	}

	if (CurrentSelection != OldCurrentSelection)
	{
		AllWords->SetColour(Options[OldCurrentSelection], 0xFFA0A0A0);
	}

	if (ControlPacket & 1 << CON_Y)
	{
		QueuedCommand = 'S';
		FadeOut = true;

		QueuedValue = (Screen *)new MainScreen(NULL);
		((MainScreen *)QueuedValue)->SetDelay(10);
	}

	return 0;
}

void LoadSaveScreen::DrawBackground()
{
}

void LoadSaveScreen::Destroy()
{
	delete AllWords;
}
