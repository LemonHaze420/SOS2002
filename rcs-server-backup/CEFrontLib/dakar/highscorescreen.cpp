// $Header

// $Log

// Default screen class

#include "../ConsoleFrontLib.h"

HighScoreScreen::HighScoreScreen(Alphabet *_MainAlphabet)
{
	MainAlphabet = _MainAlphabet;

	sprintf(ModeWords[0], "Time Attack Mode");
	sprintf(ModeWords[1], "Arcade Mode");
	sprintf(ModeWords[2], "Campaign Mode");
	
	CurrentSelection = 0;

	AllSprites = new SpriteList();
		
#if defined (IGOR)
	ScreenArchive = NewArchive();
	ScreenArchive->Open(FRONTEND_DATA "HiScoreScreen.tc");
#else
	arcPush(".\\FrontLibData\\HiScoreScreen.tc");
#endif
	Render2D ThisMaterial;
	AllSprites->LoadTexture(&ThisMaterial, NULL, "HiScoreStuff.3df");
	OrangeArrow = AllSprites->Add(&ThisMaterial, 39, 84, 31, 18);
	OrangeFade = AllSprites->Add(&ThisMaterial, 0,40, 256,20);
	HiScoreWord = AllSprites->Add(&ThisMaterial, 5,3, 189,22);
#if defined (IGOR)
	FreeArchive(ScreenArchive);
#else
	arcPop();
#endif

	AllSprites->SetPosition(OrangeArrow, &Vector(84.0f, 191.0f, 0.6f), &Vector(256.0f, 128.0f, 1.0f));
	AllSprites->Hide(OrangeArrow);

	AllSprites->SetPosition(OrangeFade, &Vector(111.0f, 140.0f, 1.0f), &Vector(256.0f, 128.0f, 1.0f), &Vector(459.0f/256.0f, 1.0f, 4.2f));
	GlobalSprites->SetPosition(HalfRedShade, &Vector(111.0f, 160.0f, 1.0f), NULL, &Vector(459.0f/256.0f, 211.0f/128.0f, 7.1f));
	GlobalSprites->Show(HalfRedShade);
	GlobalSprites->SetPosition(WhiteArrows[ARROW_LEFT], &Vector(50.0f, 400.0f, 0.6f), &Vector(64.0f, 64.0f, 1.0f));
	GlobalSprites->SetPosition(WhiteArrows[ARROW_RIGHT], &Vector(63.0f, 400.0f, 0.6f), &Vector(64.0f, 64.0f, 1.0f));
	GlobalSprites->Hide(WhiteArrows[ARROW_LEFT]);
	GlobalSprites->Hide(WhiteArrows[ARROW_RIGHT]);
	GlobalSprites->SetPosition(PSButtons[BUTTON_CROSS], &Vector(142.0, 393.0f, 0.6f), &Vector(64.0f, 64.0f, 1.0f));
	GlobalSprites->Hide(PSButtons[BUTTON_CROSS]);

	AllLines = new LineList;
	for (int i=0 ; i<11 ; i++)
		HorizontalLines[i] = AllLines->Add(&Vector(711.0f, 160.0f+i*20, 0.8f), &Vector(1370.0f, 160.0f+i*20.0f, 0.8f), 0xA0A0A0A0);

	AllWords = new WordList;
	PositionWord = AllWords->AddWord(MiddleFont, "Position", &Vector(124.0f, 144.0f, 0.6f), NULL, 0xD0D0D0D0);
	PlayerWord = AllWords->AddWord(MiddleFont, "Player", &Vector(239.0f, 144.0f, 0.6f), NULL, 0xD0D0D0D0);
	TotalTimeWord = AllWords->AddWord(MiddleFont, "Total Time", &Vector(354.0f, 144.0f, 0.6f), NULL, 0xD0D0D0D0);
	TracksWord = AllWords->AddWord(MiddleFont, "Tracks", &Vector(490.0f, 144.0f, 0.6f), NULL, 0xD0D0D0D0);
	ModeWord = AllWords->AddWord(SmallFont, "Mode", &Vector(80.0f, 400.0f, 0.6f), NULL, 0xD0D0D0D0);
	ExitWord = AllWords->AddWord(SmallFont, "Exit", &Vector(180.0f, 400.0f, 0.6f), NULL, 0xD0D0D0D0);
	AllWords->Hide(ModeWord);
	AllWords->Hide(ExitWord);
	ChosenModeWord[0] = AllWords->AddWord(LargeFont, ModeWords[0], &Vector(472.0f, 92.0f, 0.6f), NULL, 0xFFFFFFFF);
	ChosenModeWord[1] = AllWords->AddWord(LargeFont, ModeWords[1], &Vector(472.0f, 92.0f, 0.6f), NULL, 0xFFFFFFFF);
	ChosenModeWord[2] = AllWords->AddWord(LargeFont, ModeWords[2], &Vector(472.0f, 92.0f, 0.6f), NULL, 0xFFFFFFFF);
	for (i=0 ; i<3 ; i++)
	{
		AllWords->Hide(ChosenModeWord[i]);
		AllWords->Justify(ChosenModeWord[i], 5);
	}
	for (i=0 ; i<10 ; i++)
	{
		char Buffer[20];
		sprintf(Buffer, "%d", i);
		PositionWords[i] = AllWords->AddWord(SmallFont, Buffer, &Vector(160.0f, 166.0f+i*20.0f, 0.6f), NULL, 0xFFFFFFFF);
		AllWords->Hide(PositionWords[i]);

		PlayerWords[i] = AllWords->AddWord(SmallFont, i%2 == 1 ? "Andy" : "The Master", &Vector(268.0f, 172.0f+i*20.0f, 0.6f), NULL, 0xFFFFFFFF);
		AllWords->Justify(PlayerWords[i], 5);
		AllWords->Hide(PlayerWords[i]);

		TotalTimeWords[i] = AllWords->AddWord(SmallFont, "1:30:25", &Vector(406.0f, 172.0f+i*20.0f, 0.6f), NULL, 0xFFFFFFFF);
		AllWords->Justify(TotalTimeWords[i], 5);
		AllWords->Hide(TotalTimeWords[i]);

		sprintf(Buffer, "%d", 12-i);
		TracksWords[i] = AllWords->AddWord(SmallFont, Buffer, &Vector(520.0f, 166.0f+i*20.0f, 0.6f), NULL, 0xFFFFFFFF);
		AllWords->Hide(TracksWords[i]);
	}

	AllWords->Hide(PositionWord);
	AllWords->Hide(PlayerWord);
	AllWords->Hide(TotalTimeWord);
	AllWords->Hide(TracksWord);

	// Reset the counter to zero.
	Counter = 0;
	SlideCounter = MaxSlideCounter = 20;
	
	// Use the default background.
	UseDefaultBackdrop = true;
}

HighScoreScreen::~HighScoreScreen()
{
}

void HighScoreScreen::Update(struct Instruction *ScreenCommand)
{
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;

	if (Delay)
	{
		Delay--;
	}
	else if (SlideCounter || FadeOut)
	{
		if (!FadeOut)
			SlideCounter--;
		else
			SlideCounter++;

		if (!SlideCounter)
		{
			AllWords->Show(PositionWord);
			AllWords->Show(PlayerWord);
			AllWords->Show(TotalTimeWord);
			AllWords->Show(TracksWord);
			AllWords->Show(ModeWord);
			AllWords->Show(ExitWord);
			AllWords->Show(ChosenModeWord[CurrentSelection]);
			
			for (int i=0 ; i<10 ; i++)
			{
				AllWords->Show(PositionWords[i]);
				AllWords->Show(PlayerWords[i]);
				AllWords->Show(TotalTimeWords[i]);
				AllWords->Show(TracksWords[i]);
			}

			AllSprites->Show(OrangeArrow);
			GlobalSprites->Show(WhiteArrows[ARROW_LEFT]);
			GlobalSprites->Show(WhiteArrows[ARROW_RIGHT]);
			GlobalSprites->Show(PSButtons[BUTTON_CROSS]);
		}
		else
		{
			if (AllSprites)
			{
				AllWords->Hide(PositionWord);
				AllWords->Hide(PlayerWord);
				AllWords->Hide(TotalTimeWord);
				AllWords->Hide(TracksWord);
				AllWords->Hide(ModeWord);
				AllWords->Hide(ExitWord);
				AllWords->Hide(ChosenModeWord[CurrentSelection]);
				
				for (int i=0 ; i<10 ; i++)
				{
					AllWords->Hide(PositionWords[i]);
					AllWords->Hide(PlayerWords[i]);
					AllWords->Hide(TotalTimeWords[i]);
					AllWords->Hide(TracksWords[i]);
				}

				AllSprites->Hide(OrangeArrow);
				GlobalSprites->Hide(WhiteArrows[ARROW_LEFT]);
				GlobalSprites->Hide(WhiteArrows[ARROW_RIGHT]);
				GlobalSprites->Hide(PSButtons[BUTTON_CROSS]);

				float NewPosition = 711.0f-600.0f*(float)sin(((float)MaxSlideCounter-(float)SlideCounter)/(float)MaxSlideCounter*PI/2.0f);
				for (i=0 ; i<11 ; i++)
					AllLines->SetPosition(HorizontalLines[i], &Vector(NewPosition, 160.0f+i*20.0f, 0.6f), &Vector(NewPosition+459.0f, 160.0f+i*20.0f, 0.6f));
			}
		}
	}
	Counter++;
	
	if (AllSprites)
	{
		float NewPosition;
		if (SlideCounter < 20)
			NewPosition = -270.0f+302.0f*(float)sin((20.0f-(float)SlideCounter)/20.0f*PI/2.0f);
		else
			NewPosition = -300.0f;
		
		AllSprites->SetPosition(HiScoreWord, &Vector(NewPosition, 52.0f, 1.0f), &Vector(256.0f, 128.0f, 1.0f));
	}
	
	if (AllSprites)
	{
		AllSprites->DrawAll();
		AllLines->Draw();
		AllWords->DrawAll();
	}

	if (SlideCounter > MaxSlideCounter && !Delay)
	{
		ScreenCommand->Command = 'D';
		ScreenCommand->Value = NULL;

		Destroy();
	}
}

int HighScoreScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket & 1 << CON_Y || ControlPacket & 1 << CON_A)
	{
		QueuedCommand = 'S';
		FadeOut = true;

		QueuedValue = (Screen *)new OptionsScreen(NULL);
		((OptionsScreen *)QueuedValue)->SetDelay(10);
	}

	if (ControlPacket & 1 << CON_LEFT)
	{
		AllWords->Hide(ChosenModeWord[CurrentSelection]);
		if (--CurrentSelection == -1) 
			CurrentSelection = 2;
		AllWords->Show(ChosenModeWord[CurrentSelection]);
	}

	if (ControlPacket & 1 << CON_RIGHT)
	{
		AllWords->Hide(ChosenModeWord[CurrentSelection]);
		if (++CurrentSelection == 3) 
			CurrentSelection = 0;
		AllWords->Show(ChosenModeWord[CurrentSelection]);
	}
	return 0;
}

void HighScoreScreen::DrawBackground()
{
}

void HighScoreScreen::Destroy()
{
	delete AllSprites; AllSprites = NULL;
	delete AllWords;
	delete AllLines;
	GlobalSprites->Hide(HalfRedShade);
}
