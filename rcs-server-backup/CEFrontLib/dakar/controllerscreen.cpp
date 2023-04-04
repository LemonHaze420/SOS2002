// $Header

// $Log

// Default screen class

#include "../ConsoleFrontLib.h"

ControllerScreen::ControllerScreen(Alphabet *_MainAlphabet)
{
	MainAlphabet = _MainAlphabet;

	AllWords = new WordList();

	Options[0] = AllWords->AddWord(MiddleFont, "Configuration", &Vector(202.0f, 324.0f, 1.0f), NULL, 0xFFA0A0A0);
	Options[1] = AllWords->AddWord(MiddleFont, "Vibration", &Vector(202.0f, 348.0f, 1.0f), NULL, 0xFFA0A0A0);

	Values[0] = AllWords->AddWord(SmallFont, "setup 1", &Vector(380.0f, 327.0f, 1.0f), NULL, 0xFFA0A0A0);
	Values[1] = AllWords->AddWord(SmallFont, "setup 2", &Vector(442.0f, 327.0f, 1.0f), NULL, 0xFFA0A0A0);
	Values[2] = AllWords->AddWord(SmallFont, "setup 3", &Vector(504.0f, 327.0f, 1.0f), NULL, 0xFFA0A0A0);
	Values[3] = AllWords->AddWord(SmallFont, "off", &Vector(380.0f, 351.0f, 1.0f), NULL, 0xFFA0A0A0);
	Values[4] = AllWords->AddWord(SmallFont, "low", &Vector(442.0f, 351.0f, 1.0f), NULL, 0xFFA0A0A0);
	Values[5] = AllWords->AddWord(SmallFont, "high", &Vector(504.0f, 351.0f, 1.0f), NULL, 0xFFA0A0A0);

	for (int i=0 ; i<4 ; i+=2)
	{
		LocalHorizLines[i] = GlobalSprites->Clone(HorizontalLines[2]);
		LocalHorizLines[i+1] = GlobalSprites->Clone(HorizontalLines[4]);
		GlobalSprites->SetPosition(LocalHorizLines[i], &Vector(790.0f, 340.0f+i*12.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f), &Vector(10.0f, 1.0f, 1.0f));
		GlobalSprites->Justify(LocalHorizLines[i+1], 6);
		GlobalSprites->SetPosition(LocalHorizLines[i+1], &Vector(789.0f, 340.0f+i*12.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f), NULL, 2);
	}

	LocalVertLines[0] = GlobalSprites->Clone(VerticalLines[2]);
	LocalVertLines[1] = GlobalSprites->Clone(VerticalLines[2]);
	GlobalSprites->SetPosition(LocalVertLines[0], &Vector(190.0f, 652.0f, 1.0f), &Vector(64.0f, 256.0f, 1.0f), NULL);
	GlobalSprites->Justify(LocalVertLines[1], 1);
	GlobalSprites->SetPosition(LocalVertLines[1], &Vector(190.0f, 651.0f, 1.0f), &Vector(64.0f, 256.0f, 1.0f), NULL, 2);

	HelpTexts[0] = AllWords->AddWord(SmallFont, "Select", &Vector(473.0f, 409.0f, 1.0f), NULL, 0xFFA0A0A0);
	HelpTexts[1] = AllWords->AddWord(SmallFont, "Back", &Vector(576.0f, 409.0f, 1.0f), NULL, 0xFFA0A0A0);
	AllWords->Hide(HelpTexts[0]); AllWords->Hide(HelpTexts[1]);

	CurrentSelection = 0;
	AllWords->SetColour(Options[CurrentSelection], 0xFFFFFFFF);

	SelectionCycler = new CycledBox(0xFFFFFFFF, 0xFF444444, 0x00A0A040, 0xFFFFFFFF);

	AllWords->SetColour(Values[0], MainGameInfo.ControllerConfig==0 ? 0xFFFFFFFF : 0xFFA0A0A0);
	AllWords->SetColour(Values[1], MainGameInfo.ControllerConfig==1 ? 0xFFFFFFFF : 0xFFA0A0A0);
	AllWords->SetColour(Values[2], MainGameInfo.ControllerConfig==2 ? 0xFFFFFFFF : 0xFFA0A0A0);
	AllWords->SetColour(Values[3], MainGameInfo.Vibration==0 ? 0xFFFFFFFF : 0xFFA0A0A0);
	AllWords->SetColour(Values[4], MainGameInfo.Vibration==1 ? 0xFFFFFFFF : 0xFFA0A0A0);
	AllWords->SetColour(Values[5], MainGameInfo.Vibration==2 ? 0xFFFFFFFF : 0xFFA0A0A0);

	ControlWords[0][0] = AllWords->AddWord(SmallFont, "L1 gear up", &Vector(153.0f, 100.0f, 1.0f), NULL, 0xFFA0A0A0);
	ControlWords[0][1] = AllWords->AddWord(SmallFont, "R1 gear down", &Vector(153.0f, 120.0f, 1.0f), NULL, 0xFFA0A0A0);
	ControlWords[0][2] = AllWords->AddWord(SmallFont, "Steering", &Vector(183.0f, 270.0f, 1.0f), NULL, 0xFFA0A0A0);
	ControlWords[0][3] = AllWords->AddWord(SmallFont, "brake", &Vector(350.0f, 140.0f, 1.0f), NULL, 0xFFA0A0A0);
	ControlWords[0][4] = AllWords->AddWord(SmallFont, "R2 look behind", &Vector(500.0f, 110.0f, 1.0f), NULL, 0xFFA0A0A0);
	ControlWords[0][5] = AllWords->AddWord(SmallFont, "change camera", &Vector(500.0f, 160.0f, 1.0f), NULL, 0xFFA0A0A0);
	ControlWords[0][6] = AllWords->AddWord(SmallFont, "handbrake", &Vector(500.0f, 190.0f, 1.0f), NULL, 0xFFA0A0A0);
	ControlWords[0][7] = AllWords->AddWord(SmallFont, "accelerate", &Vector(500.0f, 220.0f, 1.0f), NULL, 0xFFA0A0A0);

	for (i=0 ; i<8 ; i++)
		AllWords->Hide(ControlWords[0][i]);

	GlobalSprites->SetPosition(RedShade, &Vector(192.0f, 290.0f, 1.0f), NULL, &Vector(178.0f/256.0f, 128.0f/256.0f, 4.1f));
	GlobalSprites->Hide(RedShade);

	AllSprites = new SpriteList();

	Render2D ThisMaterial;
	AllSprites->LoadTexture(&ThisMaterial, ".\\FrontLibData\\ControllerScreen.tc", "Controller.3df");

	JoyPad = AllSprites->Add(&ThisMaterial, 0,0, 256,190);
	AllSprites->SetPosition(JoyPad, &Vector(238.0f, 102.0f, 1.0f));
	AllSprites->Hide(JoyPad);

	AllSprites->LoadTexture(&ThisMaterial, NULL, "OptionsTitles.3df");
	ControllerWord = AllSprites->Add(&ThisMaterial, 5,102,201,21);

	for (i=0 ; i<2 ; i++)
		AllWords->Hide(Options[i]);
	for (i=0 ; i<6 ; i++)
		AllWords->Hide(Values[i]);

	// Reset the counter to zero.
	Counter = 0;
	SlideCounter = MaxSlideCounter = 20;
	CyclePeriod = 30;
	
	// Use the default background.
	UseDefaultBackdrop = true;
}

ControllerScreen::~ControllerScreen()
{
}

void ControllerScreen::Update(struct Instruction *ScreenCommand)
{
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
		float NewPosition = 652.0f-300.0f*(float)sin(((float)MaxSlideCounter-(float)SlideCounter)/(float)MaxSlideCounter*PI/2.0f);
		GlobalSprites->SetPosition(LocalVertLines[0], &Vector(190.0f, NewPosition, 1.0f));
		GlobalSprites->SetPosition(LocalVertLines[1], &Vector(190.0f, NewPosition-1, 1.0f), NULL, NULL, 2);

		NewPosition = 790.0f-600.0f*(float)sin(((float)MaxSlideCounter-(float)SlideCounter)/(float)MaxSlideCounter*PI/2.0f);
		for (int i=0 ; i<4 ; i+=2)
		{
			GlobalSprites->SetPosition(LocalHorizLines[i], &Vector(NewPosition, 340.0f+i*12.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f), &Vector(2.0f, 1.0f, 4.1f));
			GlobalSprites->SetPosition(LocalHorizLines[i+1], &Vector(NewPosition, 340.0f+i*12.0f, 1.0f), NULL, NULL, 2);
		}
		if (!FadeOut)
			SlideCounter--;

		if (!SlideCounter)
		{
			for (int i=0 ; i<2 ; i++)
				AllWords->Show(Options[i]);
			GlobalSprites->Show(PSButtons[3]);
			GlobalSprites->Show(PSButtons[1]);
			AllWords->Show(HelpTexts[0]); AllWords->Show(HelpTexts[1]);
			GlobalSprites->Show(RedShade);
			AllSprites->Show(JoyPad);
			for (i=0 ; i<8 ; i++)
				AllWords->Show(ControlWords[0][i]);
			for (i=0 ; i<2 ; i++)
				AllWords->Show(Options[i]);
			for (i=0 ; i<6 ; i++)
				AllWords->Show(Values[i]);
			
		}
	}
	
	switch (CurrentSelection)
	{
	case 0:
		AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.ControllerConfig], SelectionCycler->CurrentColour(NewCounter));
		break;
	case 1:
		AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.Vibration], SelectionCycler->CurrentColour(NewCounter));
		break;
	}
	
	AllSprites->DrawAll();

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
		
		AllSprites->SetPosition(ControllerWord, &Vector(NewPosition, 52.0f, 1.0f), &Vector(256.0f, 128.0f, 1.0f));

		AllSprites->DrawAll();
	}

	Counter++;

	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int ControllerScreen::ControlPressed(int ControlPacket)
{
	int OldCurrentSelection = CurrentSelection;
	
	if (ControlPacket & 1 << CON_DOWN)
	{
		switch (CurrentSelection)
		{
		case 0:
			AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.ControllerConfig], 0xFFFFFFFF);
			break;
		case 1:
			AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.Vibration], 0xFFFFFFFF);
			break;
		}

		AllWords->SetColour(Options[CurrentSelection], 0xFFA0A0A0);
		if (CurrentSelection < 1) CurrentSelection++;
		AllWords->SetColour(Options[CurrentSelection], 0xFFFFFFFF);
	}

	if (ControlPacket & 1 << CON_UP)
	{
		switch (CurrentSelection)
		{
		case 0:
			AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.ControllerConfig], 0xFFFFFFFF);
			break;
		case 1:
			AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.Vibration], 0xFFFFFFFF);
			break;
		}

		AllWords->SetColour(Options[CurrentSelection], 0xFFA0A0A0);
		if (CurrentSelection) CurrentSelection--;
		AllWords->SetColour(Options[CurrentSelection], 0xFFFFFFFF);
	}

	if (ControlPacket & 1 << CON_LEFT)
	{
		switch (CurrentSelection)
		{
		case 0:
			AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.ControllerConfig], 0xFFA0A0A0);
			if (MainGameInfo.ControllerConfig) MainGameInfo.ControllerConfig--;
			break;
		case 1:
			AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.Vibration], 0xFFA0A0A0);
			if (MainGameInfo.Vibration) MainGameInfo.Vibration--;
			break;
		}
	}

	if (ControlPacket & 1 << CON_RIGHT)
	{
		switch (CurrentSelection)
		{
		case 0:
			AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.ControllerConfig], 0xFFA0A0A0);
			if (MainGameInfo.ControllerConfig < 2) MainGameInfo.ControllerConfig++;
			break;
		case 1:
			AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.Vibration], 0xFFA0A0A0);
			if (MainGameInfo.Vibration < 2) MainGameInfo.Vibration++;
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
		for (int i=0 ; i<8 ; i++)
			AllWords->Hide(ControlWords[0][i]);
		AllSprites->Hide(JoyPad);

		QueuedValue = (Screen *)new OptionsScreen(NULL);
		((OptionsScreen *)QueuedValue)->SetDelay(10);
	}

	return 0;
}

void ControllerScreen::DrawBackground()
{
}

void ControllerScreen::Destroy()
{
	for (int i=0 ; i<4 ; i++)
		GlobalSprites->Delete(LocalHorizLines[i]);
	for (i=0 ; i<2 ; i++)
		GlobalSprites->Delete(LocalVertLines[i]);

	delete AllSprites; AllSprites = NULL;
	delete AllWords;
}
