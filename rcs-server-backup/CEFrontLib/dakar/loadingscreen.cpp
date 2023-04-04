// $Header

// $Log

// Default screen class

#include "../ConsoleFrontLib.h"

LoadingScreen::LoadingScreen(Alphabet *_MainAlphabet)
{
	MainAlphabet = _MainAlphabet;

	GlobalSprites->Hide(RaceModeTexts[MainGameInfo.RaceMode]);

	AllSprites = new SpriteList();

	// Load in the chevrons etc.
#if defined (IGOR)
	ScreenArchive = NewArchive();
	ScreenArchive->Open(FRONTEND_DATA "LoadingScreen.tc");
#else
	arcPush(".\\FrontLibData\\LoadingScreen.tc");
#endif

	Render2D ThisMaterial;
	AllSprites->LoadTexture(&ThisMaterial, NULL, "Loading.3df");

	Chevrons[0] = AllSprites->Add(&ThisMaterial, 1,1,60,106);
	Chevrons[1] = AllSprites->Clone(Chevrons[0]);
	Chevrons[2] = AllSprites->Clone(Chevrons[0]);
	Chevrons[3] = AllSprites->Clone(Chevrons[0]);
	Chevrons[4] = AllSprites->Clone(Chevrons[0]);
	Chevrons[5] = AllSprites->Clone(Chevrons[0]);
	Chevrons[6] = AllSprites->Clone(Chevrons[0]);
	Chevrons[7] = AllSprites->Add(&ThisMaterial, 63,1,60,106);
	Chevrons[8] = AllSprites->Add(&ThisMaterial, 125,1,60,106);
	Chevrons[9] = AllSprites->Add(&ThisMaterial, 187,1,60,106);
	Chevrons[10] = AllSprites->Add(&ThisMaterial, 1,109,60,106);
	Chevrons[11] = AllSprites->Add(&ThisMaterial, 63,109,60,106);

	LoadingText = AllSprites->Add(&ThisMaterial, 105,224, 148,31);
	BlackBar = AllSprites->Add(&ThisMaterial, 141,147, 106,15);
	ProgressBar = AllSprites->Add(&ThisMaterial, 141,122, 106,15);

#if defined (IGOR)
	FreeArchive(ScreenArchive);
#else
	arcPop();
#endif

	for (int i=0 ; i<9 ; i++)
		AllSprites->SetPosition(Chevrons[i], &Vector(132.0f+i*40.0f, 185.0f, 0.8f)); 
	
	AllSprites->SetPosition(BlackBar, &Vector(229.0f, 264.0f, 0.2f), NULL, &Vector(202.0f/106.0f, 1.0f, 4.1f)); 
	AllSprites->Hide(BlackBar);
	AllSprites->SetPosition(ProgressBar, &Vector(230.0f, 264.0f, 0.15f), NULL, &Vector(1.0f/106.0f, 1.0f, 4.1f));
	AllSprites->Hide(ProgressBar);

	for (i=0 ; i<3 ; i++)
		GlobalSprites->Hide(Banner[i]);
	for (i=0 ; i<3 ; i++)
		GlobalSprites->Hide(CurveSections[i]);

	// Reset the counter to zero.
	Counter = 0;
	SlideCounter = MaxSlideCounter = 30;
	CyclePeriod = 30;

	// Use the default background.
	UseDefaultBackdrop = true;
}

LoadingScreen::~LoadingScreen()
{
}

void LoadingScreen::Update(struct Instruction *ScreenCommand)
{
	float NewCounter;
	NewCounter = (float)(Counter%(2*CyclePeriod));
	if (NewCounter > CyclePeriod) NewCounter = (float)(2*CyclePeriod - NewCounter);
	NewCounter = NewCounter/(float)CyclePeriod;

	if (Delay)
	{
		Delay--;
	}
	else if (SlideCounter || FadeOut)
	{
		float NewTop = 45.0f+132.0f*(float)sin(((float)MaxSlideCounter-(float)SlideCounter)/(float)MaxSlideCounter*PI/2.0f);
		float NewBottom = 50.0f+129.0f*(float)sin(((float)MaxSlideCounter-(float)SlideCounter)/(float)MaxSlideCounter*PI/2.0f);

		SetBarsHeight((int)NewTop, (int)NewBottom);

		if (!FadeOut)
			SlideCounter--;
		else
			SlideCounter++;

		if (!SlideCounter)
		{
			AllSprites->SetPosition(LoadingText, &Vector(254.0f,227.0f,0.3f));
			AllSprites->Show(LoadingText);
			AllSprites->Show(BlackBar);
			AllSprites->Show(ProgressBar);
		}
	}
	Counter++;
	
	if (AllSprites)
	{
		int ChevronTimer = (int)((float)Counter*0.3f);
		
		for (int i=0 ; i<12 ; i++)
		{
			if (452-i*40.0f < 132.0f || SlideCounter)
				AllSprites->Hide(Chevrons[11-(i+ChevronTimer%12)%12]);
			else
				AllSprites->Show(Chevrons[11-(i+ChevronTimer%12)%12]);
			
			AllSprites->SetPosition(Chevrons[11-(i+ChevronTimer%12)%12], &Vector(452.0f-i*40.0f, 185.0f, 0.8f)); 
		}

		if ((int)(Counter/10.0f) < 100.0f)
			AllSprites->SetPosition(ProgressBar, &Vector(230.0f, 264.0f, 0.15f), NULL, &Vector(Counter/1000.0f*200/106.0f, 1.0f, 4.1f));
	}
	
	if (AllSprites)
		AllSprites->DrawAll();
	
	if (SlideCounter > MaxSlideCounter && !Delay)
	{
		ScreenCommand->Command = 'D';
		ScreenCommand->Value = NULL;

		Destroy();
	}
	
	ScreenCommand->Command = QueuedCommand;

	ScreenCommand->Value = QueuedValue;
}

int LoadingScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket & 1 << CON_Y)
	{
		AllSprites->Hide(LoadingText);
		AllSprites->Hide(BlackBar);
		AllSprites->Hide(ProgressBar);
		QueuedCommand = 'S';
		QueuedValue = (Screen *) new TrackDetailsScreen(NULL);
		((TrackDetailsScreen *)QueuedValue)->SetDelay(10);
		FadeOut = true;
	}
	return 0;
}

void LoadingScreen::DrawBackground()
{
}

void LoadingScreen::Destroy()
{
	delete AllSprites; AllSprites = NULL;
}
