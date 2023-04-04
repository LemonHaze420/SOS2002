// $Header

// $Log

// Default screen class

#define D3D_OVERLOADS

#include "../ConsoleFrontLib.h"

#if defined(IGOR)
int LineVertices[3];
#else
D3DTLVERTEX LineVertices[3];
#endif

Archive *OptionsTitlesArchive;

MainScreen::MainScreen(Alphabet *_MainAlphabet)
{
	MainAlphabet = _MainAlphabet;

#if defined (IGOR)
	ScreenArchive = NewArchive();
	ScreenArchive->Open(FRONTEND_DATA "MainScreen.tc");
#else
	arcPush(".\\FrontLibData\\MainScreen.tc");
#endif
	AllSprites = new SpriteList();
	Render2D ThisMaterial;
	AllSprites->LoadTexture(&ThisMaterial, NULL, "Sc01_Icons_a");//, ScreenArchive);

	int x1,y1;

	SelectionBorder = AllSprites->Add(&ThisMaterial, 86,127,107,80);
 	AllSprites->SetPosition(SelectionBorder, &Vector(41.0f+MainGameInfo.RaceMode*113,203.0f,0.90f));

	for (int i=0 ; i<5 ; i++)
	{
		x1 = (i<4 ? 0 : 86);
		y1 = (i<4 ? i*59 : 0);
		Buttons[i] = AllSprites->Add(&ThisMaterial,x1,y1,81,55);
		AllSprites->SetPosition(Buttons[i], &Vector(54.0f+113*i,214.0f,0.8f));

		Borders[i] = AllSprites->Add(&ThisMaterial,86,60,92,64);
		AllSprites->SetPosition(Borders[i], &Vector(49.0f+113*i,210.0f,0.75f));
	}

	AllSprites->LoadTexture(&ThisMaterial, NULL, "Sc01_TextHints_a", ScreenArchive);

	for (i=0 ; i<5 ; i++)
	{
		x1 = 0; y1 = 26*i;
		Texts[i] = AllSprites->Add(&ThisMaterial, x1, y1,256,25);
		AllSprites->SetPosition(Texts[i], &Vector(55.0f,406.0f, 1.0f), &Vector(256.0f,128.0f,0.0f));

		if (i != MainGameInfo.RaceMode) AllSprites->Hide(Texts[i]);
	}

#if defined (IGOR)
#else
	arcPop();
#endif

	// No background artefacts.
	GlobalSprites->Hide(Line);
	for (i=0 ; i<3 ; i++) GlobalSprites->Hide(Banner[i]);
	for (i=0 ; i<3 ; i++) GlobalSprites->Hide(CurveSections[i]);
	for (i=0 ; i<4 ; i++) GlobalSprites->Hide(Grid[i]);

	// Initialise the colour cycling box.
	SelectionBox = new CycledBox(0xFFDA3760, 0xFFFFFFCC, 0x004040A0, 0xFFFFFFFF);

	CurrentSelection = MainGameInfo.RaceMode; ShownSelection = CurrentSelection;
	BoxCurrentX = 41+CurrentSelection*113;

	TextSpin = 20; ButtonSpin = 20;
	MaxTextSpin = TextSpin; MaxButtonSpin = ButtonSpin;

	Vector Target;

#if defined(IGOR)
	// SNARK
#else
	LineVertices[0] = _D3DTLVERTEX(_D3DVECTOR(320.0f, 240.0f, 0.999f),0.001f,0xFFFFFFFF,0xFFFFFFFF,0.5f,0.5f);
	LineVertices[1] = _D3DTLVERTEX(_D3DVECTOR(500.0f, 240.0f, 0.999f),0.001f,0xFFFFFFFF,0xFFFFFFFF,0.5f,0.5f);
	LineVertices[2] = _D3DTLVERTEX(_D3DVECTOR(320.0f, 340.0f, 0.999f), 0.001f, 0xFFFFFFFF, 0xFFFFFFFF, 0.5f, 0.5f);
#endif

	// Reset the counter to zero.
	Counter = 0;

	// Not fading out.
	FadeOut = false;
	FadeOutCounter = 20;
	Delay = 0;

	// Set the colour cycling.
	CyclePeriod = 30;

	// Use the default background.
	UseDefaultBackdrop = true;
}

MainScreen::~MainScreen()
{
}

void MainScreen::Update(struct Instruction *ScreenCommand)
{
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;

	float NewCounter;
	NewCounter = (float)(Counter%(2*CyclePeriod));
	if (NewCounter > CyclePeriod) NewCounter = (float)(2*CyclePeriod - NewCounter);
	
	NewCounter = NewCounter/(float)CyclePeriod;
	
	// Move the box.
	int DestinationX = 41+CurrentSelection*113;
	int OldCurrentX = BoxCurrentX;
	if (BoxCurrentX > DestinationX)
		BoxCurrentX-=14;
	else if (BoxCurrentX < DestinationX)
		BoxCurrentX+=14;

	if (OldCurrentX != BoxCurrentX)
	{
		bool ChangeText = false;

		if (OldCurrentX < DestinationX && BoxCurrentX > DestinationX ||
			OldCurrentX > DestinationX && BoxCurrentX < DestinationX)
		{
			BoxCurrentX = DestinationX;
		}

		if (BoxCurrentX > OldCurrentX)
			if ((float)(BoxCurrentX - 41+(CurrentSelection-1)*113) / (DestinationX - (41+(CurrentSelection-1)*113)) > 0.5f)
				ChangeText = true;
		if (BoxCurrentX < OldCurrentX)
			if ((float)((BoxCurrentX-41)%113) / 113.0f < 0.5f)
				ChangeText = true;

		if (ChangeText)
		{
			AllSprites->SetPosition(Texts[ShownSelection], NULL, NULL, &Vector(1.0f, 1.0f, 5.3f));
			AllSprites->Hide(Texts[ShownSelection]);
			ShownSelection = (BoxCurrentX-41+56)/113;
			if (!Delay)
				AllSprites->Show(Texts[ShownSelection]);
		}

		AllSprites->SetPosition(SelectionBorder, &Vector((float)BoxCurrentX,203.0f,0.90f));
	}

	AllSprites->SetColour(SelectionBorder, SelectionBox->CurrentColour(NewCounter));

	if (Delay || ButtonSpin > MaxButtonSpin)
	{
		if (--Delay || ButtonSpin > MaxButtonSpin)
		{
			dprintf("Max: %d Current: %d", MaxButtonSpin, ButtonSpin);
			for (int i=0 ; i<5 ; i++)
			{
				AllSprites->Hide(Buttons[i]);
				AllSprites->Hide(Borders[i]);	
				AllSprites->Hide(SelectionBorder);	
				AllSprites->Hide(Texts[CurrentSelection]);
			}
		}
		else
		{
			for (int i=0 ; i<5 ; i++)
			{
				AllSprites->Show(Buttons[i]);
				AllSprites->Show(Borders[i]);	
				AllSprites->Show(SelectionBorder);	
				AllSprites->Show(Texts[CurrentSelection]);
			}
		}
	}
	if (!Delay)
	{
		if (TextSpin || FadeOut)
		{
			if (FadeOut) TextSpin++; else TextSpin--;
			AllSprites->SetPosition(Texts[CurrentSelection], NULL, NULL, &Vector(1.0f,1.0f-(float)TextSpin/(float)MaxTextSpin, 5.3f));
		}	
	
		if (ButtonSpin || FadeOut)
		{
			if (FadeOut) ButtonSpin++; else ButtonSpin--;
			
			for (int i=0; i<5 ; i++)
			{
				float Rotation = (1.0f-(float)ButtonSpin/(float)MaxButtonSpin)*PI/2.0f;
				AllSprites->SetPosition(Buttons[i], NULL, NULL, &Vector(1.0f,(float)fabs(sin(Rotation)), 5.3f));
				AllSprites->SetPosition(Borders[i], NULL, NULL, &Vector(1.0f,(float)fabs(sin(Rotation)), 5.3f));
				AllSprites->SetPosition(SelectionBorder, NULL, NULL, &Vector(1.0f,(float)fabs(sin(Rotation)), 5.3f));
			}
		}
	}

	AllSprites->DrawAll();

#if defined(IGOR)
// SNARK
#else
	renderSetCurrentMaterial(NULL);
#endif

	Counter++;
	if (FadeOut)
	{
		FadeOutCounter--;
		if (!FadeOutCounter)
		{
			Destroy();
			ScreenCommand->Command = 'D';
		}
	}
	
}

int MainScreen::ControlPressed(int ControlPacket)
{
	if (ControlPacket & 1 << CON_A)
	{
		// Make sure we don`t draw anything extra.
		if (Delay) Delay = 500;

		switch (CurrentSelection)
		{
		case 0:
		case 1:
		case 2:
			QueuedCommand = 'S';
			FadeOut = true;

			QueuedValue = (Screen *)new ChooseClassScreen(NULL);

			((ChooseClassScreen *)QueuedValue)->SetDelay(10);
			break;
		case 3:
			FadeOut = true;
			QueuedCommand = 'S';

			// Push on the archive for the options texts.
#if defined (IGOR)
			OptionsTitlesArchive = NewArchive();
			OptionsTitlesArchive->Open(FRONTEND_DATA "OptionsTitles.tc");
#else
			arcPush(".\\FrontLibData\\OptionsTitles.tc");
#endif
			QueuedValue = (Screen *)new OptionsScreen(NULL);
			((OptionsScreen *)QueuedValue)->SetDelay(10);
			break;
		case 4: 
			FadeOut = true;
			QueuedCommand = 'S';
			QueuedValue = (Screen *)new LoadSaveScreen(NULL);
			break;
		}
	}
	if (ControlPacket & 1 << CON_RIGHT)
	{
		if (CurrentSelection < 4)
			CurrentSelection++;
		MainGameInfo.RaceMode = CurrentSelection;
	}
	if (ControlPacket & 1 << CON_LEFT)
	{
		if (CurrentSelection)
			CurrentSelection--;
		MainGameInfo.RaceMode = CurrentSelection;
	}
	return 0;
}

void MainScreen::DrawBackground()
{
}

void MainScreen::Destroy()
{
	dprintf("Destroying mainscreen.");
	ScreenArchive->Close();	//delete ScreenArchive;
	delete SelectionBox;
	delete AllSprites;
}


