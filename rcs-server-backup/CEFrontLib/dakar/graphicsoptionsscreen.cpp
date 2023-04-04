// $Header

// $Log

// Default screen class

#include "../ConsoleFrontLib.h"

GraphicsOptionsScreen::GraphicsOptionsScreen(Alphabet *_MainAlphabet)
{
	MainAlphabet = _MainAlphabet;

	AllWords = new WordList();

	Options[0] = AllWords->AddWord(MiddleFont, "Split Screen", &Vector(165.0f, 164.0f, 1.0f), NULL, 0xFFA0A0A0);
	Options[1] = AllWords->AddWord(MiddleFont, "Speed Units", &Vector(165.0f, 188.0f, 1.0f), NULL, 0xFFA0A0A0);
	Options[2] = AllWords->AddWord(MiddleFont, "Aspect Ratio", &Vector(165.0f, 212.0f, 1.0f), NULL, 0xFFA0A0A0);
	Options[3] = AllWords->AddWord(MiddleFont, "Screen Position", &Vector(165.0f, 266.0f, 1.0f), NULL, 0xFFA0A0A0);
	Options[4] = AllWords->AddWord(MiddleFont, "Configure HUD", &Vector(165.0f, 290.0f, 1.0f), NULL, 0xFFA0A0A0);

	Values[0] = AllWords->AddWord(SmallFont, "horizontal", &Vector(332.0f, 167.0f, 1.0f), NULL, 0xFFA0A0A0);
	Values[1] = AllWords->AddWord(SmallFont, "vertical", &Vector(404.0f, 167.0f, 1.0f), NULL, 0xFFA0A0A0);
	Values[2] = AllWords->AddWord(SmallFont, "KPH", &Vector(332.0f, 191.0f, 1.0f), NULL, 0xFFA0A0A0);
	Values[3] = AllWords->AddWord(SmallFont, "MPH", &Vector(404.0f, 191.0f, 1.0f), NULL, 0xFFA0A0A0);
	Values[4] = AllWords->AddWord(SmallFont, "4 : 3", &Vector(332.0f, 215.0f, 1.0f), NULL, 0xFFA0A0A0);
	Values[5] = AllWords->AddWord(SmallFont, "16 : 9", &Vector(404.0f, 215.0f, 1.0f), NULL, 0xFFA0A0A0);

	for (int i=0 ; i<5 ; i++)
		AllWords->Hide(Options[i]);
	for (i=0 ; i<6 ; i++)
		AllWords->Hide(Values[i]);

	for (i=0 ; i<6 ; i+=2)
	{
		LocalHorizLines[i] = GlobalSprites->Clone(HorizontalLines[2]);
		LocalHorizLines[i+1] = GlobalSprites->Clone(HorizontalLines[4]);
		GlobalSprites->SetPosition(LocalHorizLines[i], &Vector(776.0f, 180.0f+i*12.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f), &Vector(10.0f, 1.0f, 1.0f));
		GlobalSprites->Justify(LocalHorizLines[i+1], 6);
		GlobalSprites->SetPosition(LocalHorizLines[i+1], &Vector(777.0f, 180.0f+i*12.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f), NULL, 2);
	}

	LocalVertLines[0] = GlobalSprites->Clone(VerticalLines[3]);
	LocalVertLines[1] = GlobalSprites->Clone(VerticalLines[3]);
	GlobalSprites->SetPosition(LocalVertLines[0], &Vector(154.0f, 620.0f, 1.0f), &Vector(64.0f, 256.0f, 1.0f), NULL);
	GlobalSprites->Justify(LocalVertLines[1], 1);
	GlobalSprites->SetPosition(LocalVertLines[1], &Vector(154.0f, 621.0f, 1.0f), &Vector(64.0f, 256.0f, 1.0f), NULL, 2);

	HelpTexts[0] = AllWords->AddWord(SmallFont, "Select", &Vector(473.0f, 409.0f, 1.0f), NULL, 0xFFA0A0A0);
	HelpTexts[1] = AllWords->AddWord(SmallFont, "Back", &Vector(576.0f, 409.0f, 1.0f), NULL, 0xFFA0A0A0);
	AllWords->Hide(HelpTexts[0]); AllWords->Hide(HelpTexts[1]);

	CurrentSelection = 0;
	AllWords->SetColour(Options[CurrentSelection], 0xFFFFFFFF);

	SelectionCycler = new CycledBox(0xFFFFFFFF, 0xFF444444, 0x00A0A040, 0xFFFFFFFF);

	AllWords->SetColour(Values[0], !MainGameInfo.SplitScreen ? 0xFFFFFFFF : 0xFFA0A0A0);
	AllWords->SetColour(Values[1], !MainGameInfo.SplitScreen ? 0xFFA0A0A0 : 0xFFFFFFFF);
	AllWords->SetColour(Values[2], !MainGameInfo.SpeedUnits ? 0xFFFFFFFF : 0xFFA0A0A0);
	AllWords->SetColour(Values[3], !MainGameInfo.SpeedUnits ? 0xFFA0A0A0 : 0xFFFFFFFF);
	AllWords->SetColour(Values[4], !MainGameInfo.AspectRatio ? 0xFFFFFFFF : 0xFFA0A0A0);
	AllWords->SetColour(Values[5], !MainGameInfo.AspectRatio ? 0xFFA0A0A0 : 0xFFFFFFFF);

	GlobalSprites->SetPosition(RedShade, &Vector(154.0f, 131.0f, 1.0f), NULL, &Vector(146.0f/256.0f, 163.0f/256.0f, 4.1f));
	GlobalSprites->Hide(RedShade);

	AllSprites = new SpriteList;
	
	Render2D ThisMaterial;
	AllSprites->LoadTexture(&ThisMaterial, NULL, "OptionsTitles.3df");
	GraphicsWord = AllSprites->Add(&ThisMaterial, 3,36, 171,30);

	// Reset the counter to zero.
	Counter = 0;
	SlideCounter = MaxSlideCounter = 20;
	CyclePeriod = 30;
	
	// Use the default background.
	UseDefaultBackdrop = true;
}

GraphicsOptionsScreen::~GraphicsOptionsScreen()
{
}

void GraphicsOptionsScreen::Update(struct Instruction *ScreenCommand)
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
		Delay--;
	}
	else if (SlideCounter)
	{
		float NewPosition = 520.0f-300.0f*(float)sin(((float)MaxSlideCounter-(float)SlideCounter)/(float)MaxSlideCounter*PI/2.0f);
		GlobalSprites->SetPosition(LocalVertLines[0], &Vector(154.0f, NewPosition, 1.0f));
		GlobalSprites->SetPosition(LocalVertLines[1], &Vector(154.0f, NewPosition-1, 1.0f), NULL, NULL, 2);

		NewPosition = 777.0f-600.0f*(float)sin(((float)MaxSlideCounter-(float)SlideCounter)/(float)MaxSlideCounter*PI/2.0f);
		for (int i=0 ; i<6 ; i+=2)
		{
			GlobalSprites->SetPosition(LocalHorizLines[i], &Vector(NewPosition, 180.0f+i*12.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f), &Vector(2.0f, 1.0f, 4.1f));
			GlobalSprites->SetPosition(LocalHorizLines[i+1], &Vector(NewPosition, 180.0f+i*12.0f, 1.0f), NULL, NULL, 2);
		}
		if (!FadeOut)
			SlideCounter--;

		if (!SlideCounter)
		{
			for (int i=0 ; i<4 ; i++)
				AllWords->Show(Options[i]);
			GlobalSprites->Show(PSButtons[3]);
			GlobalSprites->Show(PSButtons[1]);
			AllWords->Show(HelpTexts[0]); AllWords->Show(HelpTexts[1]);
			GlobalSprites->Show(RedShade);

			for (i=0 ; i<5 ; i++)
				AllWords->Show(Options[i]);
			for (i=0 ; i<6 ; i++)
				AllWords->Show(Values[i]);

		}
	}

	switch (CurrentSelection)
	{
	case 0:
		AllWords->SetColour(Values[CurrentSelection*2+MainGameInfo.SplitScreen], SelectionCycler->CurrentColour(NewCounter));
		break;
	case 1:
		AllWords->SetColour(Values[CurrentSelection*2+MainGameInfo.SpeedUnits], SelectionCycler->CurrentColour(NewCounter));
		break;
	case 2:
		AllWords->SetColour(Values[CurrentSelection*2+MainGameInfo.AspectRatio], SelectionCycler->CurrentColour(NewCounter));
		break;
	case 3:
	case 4:
		AllWords->SetColour(Options[CurrentSelection], SelectionCycler->CurrentColour(NewCounter));
		break;
	}
	
	if (FadeOut)
	{
		SlideCounter++;
		
		if (SlideCounter == MaxSlideCounter)
		{
			Destroy();
		
			QueuedCommand = 'D';
			QueuedValue = 0;
		}
	}
	else
		AllWords->DrawAll();

	if (AllSprites)
	{
		float NewPosition;
		if (SlideCounter < 20)
			NewPosition = -270.0f+302.0f*(float)sin((20.0f-(float)SlideCounter)/20.0f*PI/2.0f);
		else
			NewPosition = -300.0f;
		
		AllSprites->SetPosition(GraphicsWord, &Vector(NewPosition, 52.0f, 1.0f), &Vector(256.0f, 128.0f, 1.0f));

		AllSprites->DrawAll();
	}

	Counter++;

	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int GraphicsOptionsScreen::ControlPressed(int ControlPacket)
{
	int OldCurrentSelection = CurrentSelection;
	
	if (ControlPacket & 1 << CON_DOWN)
	{
		switch (CurrentSelection)
		{
		case 0:
			AllWords->SetColour(Values[CurrentSelection*2], !MainGameInfo.SplitScreen ? 0xFFFFFFFF : 0xFFA0A0A0);
			AllWords->SetColour(Values[CurrentSelection*2+1], !MainGameInfo.SplitScreen ? 0xFFA0A0A0 : 0xFFFFFFFF);
			break;
		case 1:
			AllWords->SetColour(Values[CurrentSelection*2], !MainGameInfo.SpeedUnits ? 0xFFFFFFFF : 0xFFA0A0A0);
			AllWords->SetColour(Values[CurrentSelection*2+1], !MainGameInfo.SpeedUnits ? 0xFFA0A0A0 : 0xFFFFFFFF);
			break;
		case 2:
			AllWords->SetColour(Values[CurrentSelection*2], !MainGameInfo.AspectRatio ? 0xFFFFFFFF : 0xFFA0A0A0);
			AllWords->SetColour(Values[CurrentSelection*2+1], !MainGameInfo.AspectRatio ? 0xFFA0A0A0 : 0xFFFFFFFF);
			break;
		}

		AllWords->SetColour(Options[CurrentSelection], 0xFFA0A0A0);
		if (CurrentSelection < 4) CurrentSelection++;
		AllWords->SetColour(Options[CurrentSelection], 0xFFFFFFFF);
	}

	if (ControlPacket & 1 << CON_UP)
	{
		AllWords->SetColour(Options[CurrentSelection], 0xFFA0A0A0);
		switch (CurrentSelection)
		{
		case 0:
			AllWords->SetColour(Values[CurrentSelection*2], !MainGameInfo.SplitScreen ? 0xFFFFFFFF : 0xFFA0A0A0);
			AllWords->SetColour(Values[CurrentSelection*2+1], !MainGameInfo.SplitScreen ? 0xFFA0A0A0 : 0xFFFFFFFF);
			break;
		case 1:
			AllWords->SetColour(Values[CurrentSelection*2], !MainGameInfo.SpeedUnits ? 0xFFFFFFFF : 0xFFA0A0A0);
			AllWords->SetColour(Values[CurrentSelection*2+1], !MainGameInfo.SpeedUnits ? 0xFFA0A0A0 : 0xFFFFFFFF);
			break;
		case 2:
			AllWords->SetColour(Values[CurrentSelection*2], !MainGameInfo.AspectRatio ? 0xFFFFFFFF : 0xFFA0A0A0);
			AllWords->SetColour(Values[CurrentSelection*2+1], !MainGameInfo.AspectRatio ? 0xFFA0A0A0 : 0xFFFFFFFF);
			break;
		}

		if (CurrentSelection) CurrentSelection--;
		AllWords->SetColour(Options[CurrentSelection], 0xFFFFFFFF);
	}

	if (ControlPacket & 1 << CON_LEFT)
	{
		AllWords->SetColour(Values[CurrentSelection*2+1], 0xFFA0A0A0);
		switch (CurrentSelection)
		{
		case 0:
			if (MainGameInfo.SplitScreen) MainGameInfo.SplitScreen = 0;
			break;
		case 1:
			if (MainGameInfo.SpeedUnits) MainGameInfo.SpeedUnits = 0;
			break;
		case 2:
			if (MainGameInfo.AspectRatio) MainGameInfo.AspectRatio = 0;
			break;
		}
	}

	if (ControlPacket & 1 << CON_RIGHT)
	{
		AllWords->SetColour(Values[CurrentSelection*2], 0xFFA0A0A0);
		switch (CurrentSelection)
		{
		case 0:
			if (!MainGameInfo.SplitScreen) MainGameInfo.SplitScreen = 1;
			break;
		case 1:
			if (!MainGameInfo.SpeedUnits) MainGameInfo.SpeedUnits = 1;
			break;
		case 2:
			if (!MainGameInfo.AspectRatio) MainGameInfo.AspectRatio = 1;
			break;
		}
	}

	if (CurrentSelection != OldCurrentSelection)
	{
		AllWords->SetColour(Options[OldCurrentSelection], 0xFFA0A0A0);
	}

	if (ControlPacket & 1 << CON_Y)
	{
		QueuedCommand = 'S';
		FadeOut = true;

		GlobalSprites->Hide(PSButtons[3]);
		GlobalSprites->Hide(PSButtons[1]);
		GlobalSprites->Hide(RedShade);
		AllWords->Hide(HelpTexts[0]);
		AllWords->Hide(HelpTexts[1]);

		QueuedValue = (Screen *)new OptionsScreen(NULL);
		((OptionsScreen *)QueuedValue)->SetDelay(10);
	}

	return 0;
}

void GraphicsOptionsScreen::DrawBackground()
{
}

void GraphicsOptionsScreen::Destroy()
{
	for (int i=0 ; i<6 ; i++)
		GlobalSprites->Delete(LocalHorizLines[i]);
	for (i=0 ; i<2 ; i++)
		GlobalSprites->Delete(LocalVertLines[i]);
	delete AllWords;
	delete AllSprites; AllSprites = NULL;
}
