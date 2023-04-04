// $Header

// $Log

// Default screen class

#include "../ConsoleFrontLib.h"

extern Archive *OptionsTitlesArchive;

OptionsScreen::OptionsScreen(Alphabet *_MainAlphabet)
{
	MainAlphabet = _MainAlphabet;

	AllWords = new WordList();

	Options[0] = AllWords->AddWord(MiddleFont, "Graphics", &Vector(264.0f, 186.0f, 1.0f), NULL, 0xFFA0A0A0);
	Options[1] = AllWords->AddWord(MiddleFont, "Sound", &Vector(264.0f, 210.0f, 1.0f), NULL, 0xFFA0A0A0);
	Options[2] = AllWords->AddWord(MiddleFont, "Controller", &Vector(264.0f, 234.0f, 1.0f), NULL, 0xFFA0A0A0);
	Options[3] = AllWords->AddWord(MiddleFont, "High Score", &Vector(264.0f, 258.0f, 1.0f), NULL, 0xFFA0A0A0);

	for (int i=0 ; i<4 ; i++)
		AllWords->Hide(Options[i]);

	for (i=0 ; i<8 ; i+=2)
	{
		LocalHorizLines[i] = GlobalSprites->Clone(HorizontalLines[3]);
		LocalHorizLines[i+1] = GlobalSprites->Clone(HorizontalLines[3]);
		GlobalSprites->SetPosition(LocalHorizLines[i], &Vector(713.0f, 200.0f+i*12.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f), NULL);
		GlobalSprites->Justify(LocalHorizLines[i+1], 6);
		GlobalSprites->SetPosition(LocalHorizLines[i+1], &Vector(713.0f, 200.0f+i*12.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f), NULL, 2);
	}

	LocalVertLines[0] = GlobalSprites->Clone(VerticalLines[3]);
	LocalVertLines[1] = GlobalSprites->Clone(VerticalLines[2]);
	GlobalSprites->SetPosition(LocalVertLines[0], &Vector(254.0f, 626.0f, 1.0f), &Vector(64.0f, 256.0f, 1.0f), NULL);
	GlobalSprites->Justify(LocalVertLines[1], 1);
	GlobalSprites->SetPosition(LocalVertLines[1], &Vector(254.0f, 625.0f, 1.0f), &Vector(64.0f, 256.0f, 1.0f), NULL, 2);

	GlobalSprites->SetPosition(PSButtons[3], &Vector(436.0f,402.0f, 1.0f), &Vector(64.0f, 64.0f, 1.0f));
	GlobalSprites->SetPosition(PSButtons[1], &Vector(538.0f,402.0f, 1.0f), &Vector(64.0f, 64.0f, 1.0f));
	GlobalSprites->Hide(PSButtons[1]);
	GlobalSprites->Hide(PSButtons[3]);

	HelpTexts[0] = AllWords->AddWord(SmallFont, "Select", &Vector(473.0f, 409.0f, 1.0f), NULL, 0xFFA0A0A0);
	HelpTexts[1] = AllWords->AddWord(SmallFont, "Back", &Vector(576.0f, 409.0f, 1.0f), NULL, 0xFFA0A0A0);
	AllWords->Hide(HelpTexts[0]);
	AllWords->Hide(HelpTexts[1]);

	CurrentSelection = 0;

	SelectionCycler = new CycledBox(0xFFFFFFFF, 0xFF444444, 0x00A0A040, 0xFFFFFFFF);

	GlobalSprites->Show(Line);
	for (i=0 ; i<3 ; i++) GlobalSprites->Show(Banner[i]);
	for (i=0 ; i<3 ; i++) GlobalSprites->Show(CurveSections[i]);


	// Load in the sprites.
	AllSprites = new SpriteList;

	Render2D ThisMaterial;
	AllSprites->LoadTexture(&ThisMaterial, NULL, "OptionsTitles.3df");
	OptionWord = AllSprites->Add(&ThisMaterial, 2,2, 150,32);

	// Reset the counter to zero.
	Counter = 0;
	SlideCounter = MaxSlideCounter = 20;
	CyclePeriod = 30;
	
	// Use the default background.
	UseDefaultBackdrop = true;
}

OptionsScreen::~OptionsScreen()
{
}

void OptionsScreen::Update(struct Instruction *ScreenCommand)
{
	if (QueuedCommand == 'S')
	{
		ScreenCommand->Command = QueuedCommand;
		ScreenCommand->Value = QueuedValue;
		QueuedCommand = 0;
		return;
	}

	if (!AllWords)
	{
		ScreenCommand->Command = 'D';
		ScreenCommand->Value = NULL;
		return;
	}

	float NewCounter;
	NewCounter = (float)(Counter%(2*CyclePeriod));
	if (NewCounter > CyclePeriod) NewCounter = (float)(2*CyclePeriod - NewCounter);
	NewCounter = NewCounter/(float)CyclePeriod;

	if (Delay)
	{
		if (FadeOut)
		{
			ScreenCommand->Command = QueuedCommand;
			ScreenCommand->Value = QueuedValue;
			return;
		}
		Delay--;
	}
	else if (SlideCounter && AllWords)
	{
		float NewPosition = 526.0f-300.0f*(float)sin(((float)MaxSlideCounter-(float)SlideCounter)/(float)MaxSlideCounter*PI/2.0f);
		GlobalSprites->SetPosition(LocalVertLines[0], &Vector(254.0f, NewPosition, 1.0f));
		GlobalSprites->SetPosition(LocalVertLines[1], &Vector(254.0f, NewPosition-1, 1.0f), NULL, NULL, 2);

		NewPosition = 713.0f-400.0f*(float)sin(((float)MaxSlideCounter-(float)SlideCounter)/(float)MaxSlideCounter*PI/2.0f);
		for (int i=0 ; i<8 ; i+=2)
		{
			GlobalSprites->SetPosition(LocalHorizLines[i], &Vector(NewPosition, 200.0f+i*12.0f, 1.0f));
			GlobalSprites->SetPosition(LocalHorizLines[i+1], &Vector(NewPosition, 200.0f+i*12.0f, 1.0f), NULL, NULL, 2);
		}
		if (!FadeOut)
			SlideCounter--;

		if (!SlideCounter)
		{
			for (int i=0 ; i<4 ; i++)
				AllWords->Show(Options[i]);
			GlobalSprites->Show(PSButtons[3]);
			GlobalSprites->Show(PSButtons[1]);
			AllWords->Show(HelpTexts[0]);
			AllWords->Show(HelpTexts[1]);
		}
	}

	if (AllWords)
	{
		float NewPosition;
		if (SlideCounter < 20)
			NewPosition = -270.0f+302.0f*(float)sin((20.0f-(float)SlideCounter)/20.0f*PI/2.0f);
		else
			NewPosition = -300.0f;
		
		AllSprites->SetPosition(OptionWord, &Vector(NewPosition, 52.0f, 1.0f), &Vector(256.0f, 128.0f, 1.0f));

		AllWords->SetColour(Options[CurrentSelection], SelectionCycler->CurrentColour(NewCounter));

		AllSprites->DrawAll();
	}
	if (FadeOut)
	{
		SlideCounter++;
		
		if (SlideCounter >= MaxSlideCounter)
		{
			Destroy();
		
			QueuedCommand = 'D';
			QueuedValue = 0;
		}
	}
	else
	{
		AllWords->DrawAll();
	}
	Counter++;

	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int OptionsScreen::ControlPressed(int ControlPacket)
{
	int OldCurrentSelection = CurrentSelection;
	
	if (ControlPacket & 1 << CON_DOWN)
	{
		if (CurrentSelection < 3) CurrentSelection++;
	}

	if (ControlPacket & 1 << CON_UP)
	{
		if (CurrentSelection) CurrentSelection--;
	}

	if (CurrentSelection != OldCurrentSelection)
	{
		AllWords->SetColour(Options[OldCurrentSelection], 0xFFA0A0A0);
	}

	if (ControlPacket & 1 << CON_A)
	{
		GlobalSprites->Hide(PSButtons[3]);
		GlobalSprites->Hide(PSButtons[1]);

		QueuedCommand = 'S';
		FadeOut = true;

		switch (CurrentSelection)
		{
		case 0:
			QueuedValue = (Screen *)new GraphicsOptionsScreen(NULL);
			((GraphicsOptionsScreen *)QueuedValue)->SetDelay(10);
			break;
		case 1:
			QueuedValue = (Screen *)new SoundOptionsScreen(NULL);
			((SoundOptionsScreen *)QueuedValue)->SetDelay(10);
			break;
		case 2:
			QueuedValue = (Screen *)new ControllerScreen(NULL);
			((ControllerScreen *)QueuedValue)->SetDelay(10);
			break;
		case 3:
			QueuedValue = (Screen *)new HighScoreScreen(NULL);
			((HighScoreScreen *)QueuedValue)->SetDelay(10);
			break;
		}

		if (Delay)
			Destroy();
	}

	if (ControlPacket & 1 << CON_Y)
	{
		GlobalSprites->Hide(PSButtons[3]);
		GlobalSprites->Hide(PSButtons[1]);
		
		QueuedCommand = 'S';
		FadeOut = true;

		// Pop off the options titles
#if defined(IGOR)
		FreeArchive(OptionsTitlesArchive);
#else
		arcPop();
#endif
		QueuedValue = (Screen *)new MainScreen(NULL);
		((MainScreen *)QueuedValue)->SetDelay(10);

		if (Delay)
			Destroy();

	}

	return 0;
}

void OptionsScreen::DrawBackground()
{
}

void OptionsScreen::Destroy()
{
	delete AllWords; AllWords = NULL;
	delete AllSprites;
	for (int i=0 ; i<8 ; i++)
		GlobalSprites->Delete(LocalHorizLines[i]);
	for (i=0 ; i<2 ; i++)
		GlobalSprites->Delete(LocalVertLines[i]);
}
