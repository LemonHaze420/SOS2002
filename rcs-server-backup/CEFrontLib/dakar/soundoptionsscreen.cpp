// $Header

// $Log

// Default screen class

#include "../ConsoleFrontLib.h"

SoundOptionsScreen::SoundOptionsScreen(Alphabet *_MainAlphabet)
{
	MainAlphabet = _MainAlphabet;

	AllWords = new WordList();

	Options[0] = AllWords->AddWord(MiddleFont, "Music Volume", &Vector(168.0f, 164.0f, 1.0f), NULL, 0xFFA0A0A0);
	Options[1] = AllWords->AddWord(MiddleFont, "SFX Volume", &Vector(168.0f, 188.0f, 1.0f), NULL, 0xFFA0A0A0);
	Options[2] = AllWords->AddWord(MiddleFont, "Speech Volume", &Vector(168.0f, 212.0f, 1.0f), NULL, 0xFFA0A0A0);
	Options[3] = AllWords->AddWord(MiddleFont, "Output", &Vector(168.0f, 236.0f, 1.0f), NULL, 0xFFA0A0A0);
	Options[4] = AllWords->AddWord(MiddleFont, "Dolby Surround", &Vector(168.0f, 260.0f, 1.0f), NULL, 0xFFA0A0A0);

	for (int i=0 ; i<5 ; i++)
		AllWords->Hide(Options[i]);

	for (i=0 ; i<10 ; i+=2)
	{
		LocalHorizLines[i] = GlobalSprites->Clone(HorizontalLines[2]);
		LocalHorizLines[i+1] = GlobalSprites->Clone(HorizontalLines[5]);
		GlobalSprites->SetPosition(LocalHorizLines[i], &Vector(760.0f, 180.0f+i*12.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f), &Vector(2.0f, 1.0f, 4.1f));
		GlobalSprites->Justify(LocalHorizLines[i+1], 6);
		GlobalSprites->SetPosition(LocalHorizLines[i+1], &Vector(760.0f, 180.0f+i*12.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f), NULL, 2);
	}

	LocalVertLines[0] = GlobalSprites->Clone(VerticalLines[3]);
	LocalVertLines[1] = GlobalSprites->Clone(VerticalLines[2]);
	GlobalSprites->SetPosition(LocalVertLines[0], &Vector(154.0f, 515.0f, 1.0f), &Vector(64.0f, 256.0f, 1.0f), NULL);
	GlobalSprites->Justify(LocalVertLines[1], 1);
	GlobalSprites->SetPosition(LocalVertLines[1], &Vector(154.0f, 515.0f, 1.0f), &Vector(64.0f, 256.0f, 1.0f), NULL, 2);

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

	for (i=0 ; i<10 ; i++)
	{
		if (i<MainGameInfo.MusicVolume)
			Bars[i] = GlobalSprites->Clone(OrangeSquare);
		else
			Bars[i] = GlobalSprites->Clone(EmptySquare);

		if (i<MainGameInfo.SFXVolume)
			Bars[10+i] = GlobalSprites->Clone(OrangeSquare);
		else
			Bars[10+i] = GlobalSprites->Clone(EmptySquare);

		if (i<MainGameInfo.SpeechVolume)
			Bars[20+i] = GlobalSprites->Clone(OrangeSquare);
		else
			Bars[20+i] = GlobalSprites->Clone(EmptySquare);
	}

	for (i=0 ; i<10 ; i++)
	{
		GlobalSprites->SetPosition(Bars[i], &Vector(376.0f+i*10.0f, 164.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f));
		GlobalSprites->SetColour(Bars[i], 0xFFFFFFFF);
		
		GlobalSprites->SetPosition(Bars[10+i], &Vector(376.0f+i*10.0f, 188.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f));
		GlobalSprites->SetColour(Bars[10+i], 0xFF707070);
		GlobalSprites->SetPosition(Bars[20+i], &Vector(376.0f+i*10.0f, 212.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f));
		GlobalSprites->SetColour(Bars[20+i], 0xFF707070);
	}

	Values[0] = AllWords->AddWord(SmallFont, "mono", &Vector(376.0f, 240.0f, 1.0f), NULL, 0xFFA0A0A0);
	Values[1] = AllWords->AddWord(SmallFont, "stereo", &Vector(450.0f, 240.0f, 1.0f), NULL, 0xFFA0A0A0);
	Values[2] = AllWords->AddWord(SmallFont, "on", &Vector(376.0f, 264.0f, 1.0f), NULL, 0xFFA0A0A0);
	Values[3] = AllWords->AddWord(SmallFont, "off", &Vector(450.0f, 264.0f, 1.0f), NULL, 0xFFA0A0A0);

	AllWords->SetColour(Values[MainGameInfo.SoundOutput], 0xFFFFFFFF);
	AllWords->SetColour(Values[MainGameInfo.Surround+2], 0xFFFFFFFF);
	
	for (i=0 ; i<30 ; i++)
		GlobalSprites->Hide(Bars[i]);
	for (i=0 ; i<5 ; i++)
		AllWords->Hide(Options[i]);
	for (i=0 ; i<4 ; i++)
		AllWords->Hide(Values[i]);
	
	AllSprites = new SpriteList;
	
	Render2D ThisMaterial;
	AllSprites->LoadTexture(&ThisMaterial, NULL, "OptionsTitles.3df");
	SoundWord = AllSprites->Add(&ThisMaterial, 5,69, 116,21);

	// Reset the counter to zero.
	Counter = 0;
	SlideCounter = MaxSlideCounter = 20;
	CyclePeriod = 30;
	
	// Use the default background.
	UseDefaultBackdrop = true;
}

SoundOptionsScreen::~SoundOptionsScreen()
{
}

void SoundOptionsScreen::Update(struct Instruction *ScreenCommand)
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
		float NewPosition = 515.0f-300.0f*(float)sin(((float)MaxSlideCounter-(float)SlideCounter)/(float)MaxSlideCounter*PI/2.0f);
		GlobalSprites->SetPosition(LocalVertLines[0], &Vector(154.0f, NewPosition, 1.0f));
		GlobalSprites->SetPosition(LocalVertLines[1], &Vector(154.0f, NewPosition-1, 1.0f), NULL, NULL, 2);

		NewPosition = 760.0f-600.0f*(float)sin(((float)MaxSlideCounter-(float)SlideCounter)/(float)MaxSlideCounter*PI/2.0f);
		for (int i=0 ; i<10 ; i+=2)
		{
			GlobalSprites->SetPosition(LocalHorizLines[i], &Vector(NewPosition, 180.0f+i*12.0f, 1.0f), NULL, &Vector(2.0f, 1.0f, 4.1f));
			GlobalSprites->SetPosition(LocalHorizLines[i+1], &Vector(NewPosition, 180.0f+i*12.0f, 1.0f), NULL, NULL, 2);
		}
		if (!FadeOut)
			SlideCounter--;

		if (!SlideCounter)
		{
			for (int i=0 ; i<5 ; i++)
				AllWords->Show(Options[i]);
			GlobalSprites->Show(PSButtons[3]);
			GlobalSprites->Show(PSButtons[1]);
			AllWords->Show(HelpTexts[0]);
			AllWords->Show(HelpTexts[1]);
			
			for (i=0 ; i<30 ; i++)
				GlobalSprites->Show(Bars[i]);
			for (i=0 ; i<5 ; i++)
				AllWords->Show(Options[i]);
			for (i=0 ; i<4 ; i++)
				AllWords->Show(Values[i]);
			
		}
	}
	
	switch (CurrentSelection)
	{
	case 0:
	case 1:
	case 2:
		AllWords->SetColour(Options[CurrentSelection], SelectionCycler->CurrentColour(NewCounter));
		break;
	case 3:
		AllWords->SetColour(Values[MainGameInfo.SoundOutput], SelectionCycler->CurrentColour(NewCounter));
		break;
	case 4:
		AllWords->SetColour(Values[2+MainGameInfo.Surround], SelectionCycler->CurrentColour(NewCounter));
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
		
		AllSprites->SetPosition(SoundWord, &Vector(NewPosition, 52.0f, 1.0f), &Vector(256.0f, 128.0f, 1.0f));

		AllSprites->DrawAll();
	}

	Counter++;

	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;
}

int SoundOptionsScreen::ControlPressed(int ControlPacket)
{
	int OldCurrentSelection = CurrentSelection;
	
	if (ControlPacket & 1 << CON_DOWN)
	{
		AllWords->SetColour(Options[CurrentSelection], 0xFFA0A0A0);
		switch (CurrentSelection)
		{
		case 3:
			AllWords->SetColour(Values[MainGameInfo.SoundOutput], 0xFFFFFFFF);
			break;
		case 4:
			AllWords->SetColour(Values[2+MainGameInfo.Surround], 0xFFFFFFFF);
			break;
		default:
			break;
		}
		if (CurrentSelection < 3)
			for (int i=0 ; i<10 ; i++)
				GlobalSprites->SetColour(Bars[CurrentSelection*10+i], 0xFF707070);
		if (CurrentSelection < 4) CurrentSelection++;
		if (CurrentSelection < 3)
			for (int i=0 ; i<10 ; i++)
				GlobalSprites->SetColour(Bars[CurrentSelection*10+i], 0xFFFFFFFF);

		if (CurrentSelection >= 3)
			AllWords->SetColour(Options[CurrentSelection], 0xFFFFFFFF);
	}

	if (ControlPacket & 1 << CON_UP)
	{
		AllWords->SetColour(Options[CurrentSelection], 0xFFA0A0A0);
		switch (CurrentSelection)
		{
		case 3:
			AllWords->SetColour(Values[MainGameInfo.SoundOutput], 0xFFFFFFFF);
			break;
		case 4:
			AllWords->SetColour(Values[2+MainGameInfo.Surround], 0xFFFFFFFF);
			break;
		default:
			break;
		}
		if (CurrentSelection < 3)
			for (int i=0 ; i<10 ; i++)
				GlobalSprites->SetColour(Bars[CurrentSelection*10+i], 0xFF707070);
		if (CurrentSelection) CurrentSelection--;
		if (CurrentSelection < 3)
			for (int i=0 ; i<10 ; i++)
				GlobalSprites->SetColour(Bars[CurrentSelection*10+i], 0xFFFFFFFF);
		if (CurrentSelection >= 3)
			AllWords->SetColour(Options[CurrentSelection], 0xFFFFFFFF);
	}

	if (ControlPacket & 1 << CON_LEFT)
	{
		switch (CurrentSelection)
		{
		case 0:
			if (MainGameInfo.MusicVolume)
			{
				MainGameInfo.MusicVolume--;
				GlobalSprites->Delete(Bars[MainGameInfo.MusicVolume]);
				Bars[MainGameInfo.MusicVolume] = GlobalSprites->Clone(EmptySquare);
				GlobalSprites->SetPosition(Bars[MainGameInfo.MusicVolume], &Vector(376.0f+MainGameInfo.MusicVolume*10.0f, 164.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f));
				GlobalSprites->SetColour(Bars[MainGameInfo.MusicVolume], 0xFFFFFFFF);
			}
			break;
		case 1:
			if (MainGameInfo.SFXVolume)
			{
				MainGameInfo.SFXVolume--;
				GlobalSprites->Delete(Bars[10+MainGameInfo.SFXVolume]);
				Bars[10+MainGameInfo.SFXVolume] = GlobalSprites->Clone(EmptySquare);
				GlobalSprites->SetPosition(Bars[10+MainGameInfo.SFXVolume], &Vector(376.0f+MainGameInfo.SFXVolume*10.0f, 188.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f));
				GlobalSprites->SetColour(Bars[10+MainGameInfo.SFXVolume], 0xFFFFFFFF);
			}
			break;
		case 2:
			if (MainGameInfo.SpeechVolume)
			{
				MainGameInfo.SpeechVolume--;
				GlobalSprites->Delete(Bars[20+MainGameInfo.SpeechVolume]);
				Bars[20+MainGameInfo.SpeechVolume] = GlobalSprites->Clone(EmptySquare);
				GlobalSprites->SetPosition(Bars[20+MainGameInfo.SpeechVolume], &Vector(376.0f+MainGameInfo.SpeechVolume*10.0f, 212.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f));
				GlobalSprites->SetColour(Bars[20+MainGameInfo.SpeechVolume], 0xFFFFFFFF);
			}
			break;
		case 3:
			AllWords->SetColour(Values[MainGameInfo.SoundOutput], 0xFFA0A0A0);
			if (MainGameInfo.SoundOutput) MainGameInfo.SoundOutput--;
			AllWords->SetColour(Values[MainGameInfo.SoundOutput], 0xFFFFFFFF);
			break;
		case 4:
			AllWords->SetColour(Values[2+MainGameInfo.Surround], 0xFFA0A0A0);
			if (MainGameInfo.Surround) MainGameInfo.Surround--;
			AllWords->SetColour(Values[2+MainGameInfo.Surround], 0xFFFFFFFF);
			break;
		}
	}

	if (ControlPacket & 1 << CON_RIGHT)
	{
		switch (CurrentSelection)
		{
		case 0:
			if (MainGameInfo.MusicVolume < 10)
			{
				MainGameInfo.MusicVolume++;
				GlobalSprites->Delete(Bars[MainGameInfo.MusicVolume-1]);
				Bars[MainGameInfo.MusicVolume-1] = GlobalSprites->Clone(OrangeSquare);
				GlobalSprites->SetPosition(Bars[MainGameInfo.MusicVolume-1], &Vector(376.0f+(MainGameInfo.MusicVolume-1)*10.0f, 164.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f));
				GlobalSprites->SetColour(Bars[MainGameInfo.MusicVolume-1], 0xFFFFFFFF);
			}
			break;
		case 1:
			if (MainGameInfo.SFXVolume < 10)
			{
				MainGameInfo.SFXVolume++;
				GlobalSprites->Delete(Bars[10+MainGameInfo.SFXVolume-1]);
				Bars[10+MainGameInfo.SFXVolume-1] = GlobalSprites->Clone(OrangeSquare);
				GlobalSprites->SetPosition(Bars[10+MainGameInfo.SFXVolume-1], &Vector(376.0f+(MainGameInfo.SFXVolume-1)*10.0f, 188.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f));
				GlobalSprites->SetColour(Bars[10+MainGameInfo.SFXVolume-1], 0xFFFFFFFF);
			}
			break;
		case 2:
			if (MainGameInfo.SpeechVolume < 10)
			{
				MainGameInfo.SpeechVolume++;
				GlobalSprites->Delete(Bars[20+MainGameInfo.SpeechVolume-1]);
				Bars[20+MainGameInfo.SpeechVolume-1] = GlobalSprites->Clone(OrangeSquare);
				GlobalSprites->SetPosition(Bars[20+MainGameInfo.SpeechVolume-1], &Vector(376.0f+(MainGameInfo.SpeechVolume-1)*10.0f, 212.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f));
				GlobalSprites->SetColour(Bars[20+MainGameInfo.SpeechVolume-1], 0xFFFFFFFF);
			}
			break;
		case 3:
			AllWords->SetColour(Values[MainGameInfo.SoundOutput], 0xFFA0A0A0);
			if (MainGameInfo.SoundOutput<1) MainGameInfo.SoundOutput++;
			AllWords->SetColour(Values[MainGameInfo.SoundOutput], 0xFFFFFFFF);
			break;
		case 4:
			AllWords->SetColour(Values[2+MainGameInfo.Surround], 0xFFA0A0A0);
			if (MainGameInfo.Surround<1) MainGameInfo.Surround++;
			AllWords->SetColour(Values[2+MainGameInfo.Surround], 0xFFFFFFFF);
			break;
		}
	}

	if (CurrentSelection != OldCurrentSelection)
	{
		AllWords->SetColour(Options[OldCurrentSelection], 0xFFA0A0A0);
	}

	if (ControlPacket & 1 << CON_Y)
	{
		GlobalSprites->Hide(PSButtons[3]);
		GlobalSprites->Hide(PSButtons[1]);
		
		for (int i=0 ; i<30 ; i++)
			GlobalSprites->Hide(Bars[i]);
		for (i=0 ; i<5 ; i++)
			AllWords->Hide(Options[i]);
		for (i=0 ; i<4 ; i++)
			AllWords->Hide(Values[i]);

		QueuedCommand = 'S';
		FadeOut = true;

		QueuedValue = (Screen *)new OptionsScreen(NULL);
		((MainScreen *)QueuedValue)->SetDelay(10);
	}

	return 0;
}

void SoundOptionsScreen::DrawBackground()
{
}

void SoundOptionsScreen::Destroy()
{
	for (int i=0 ; i<10 ; i++)
		GlobalSprites->Delete(LocalHorizLines[i]);
	for (i=0 ; i<2 ; i++)
		GlobalSprites->Delete(LocalVertLines[i]);
	delete AllWords;
	delete AllSprites; AllSprites = NULL;
}
