// $Header

// $Log

// ChooseClass screen class

#include "../ConsoleFrontLib.h"

ChooseClassScreen::ChooseClassScreen(Alphabet *_MainAlphabet)
{
	MainAlphabet = _MainAlphabet;

#if defined(IGOR)
	ScreenArchive = NewArchive();
	ScreenArchive->Open(FRONTEND_DATA "ChooseClassScreen.tc");
#else
	arcPush(".\\FrontLibData\\ChooseClassScreen.tc");
#endif

	AllSprites = new SpriteList();
	
	Render2D ThisMaterial;
	AllSprites->LoadTexture(&ThisMaterial, NULL, "VehicleIcons_a4444");//, ScreenArchive);
	OrangeFade = AllSprites->Add(&ThisMaterial, 112,88,119,77);
	AllSprites->SetPosition(OrangeFade, &Vector(73.0f, 91.0f, 1.0f), NULL, &Vector(1.0f, 4.0f, 8.3f));

	WhiteVehicles[0] = AllSprites->Add(&ThisMaterial, 3,1,	 103,46);
	WhiteVehicles[1] = AllSprites->Add(&ThisMaterial, 3,52, 102,67);
	WhiteVehicles[2] = AllSprites->Add(&ThisMaterial, 1,119,109,50);
	WhiteVehicles[3] = AllSprites->Add(&ThisMaterial, 6,173, 96,59);
	for (int i=1 ; i<4 ; i++)
		AllSprites->Hide(WhiteVehicles[i]);
	WhiteBox = AllSprites->Add(&ThisMaterial, 112,4,120,80);

	AllSprites->LoadTexture(&ThisMaterial, NULL, "GreyedVehicleIcons_a");//, ScreenArchive);
	for (i=0 ; i<4 ; i++)
	{
		GreyedBox[i] = AllSprites->Add(&ThisMaterial, 112,10,120,77);
		AllSprites->SetPosition(GreyedBox[i], &Vector(73.0f, (float)(91+i*77), 1.0f));
	}
	GreyedBoxLid = AllSprites->Add(&ThisMaterial, 112,4,120,4);
	AllSprites->SetPosition(GreyedBoxLid, &Vector(73.0f, 90.0f, 1.0f));
	AllSprites->SetPosition(WhiteBox, &Vector(73.0f, 89.0f, 0.95f));
	AllSprites->Hide(WhiteBox);

	GreyedVehicles[0] = AllSprites->Add(&ThisMaterial, 3,1,  103,46);
	GreyedVehicles[1] = AllSprites->Add(&ThisMaterial, 3,52, 102,67);
	GreyedVehicles[2] = AllSprites->Add(&ThisMaterial, 1,119,109,50);
	GreyedVehicles[3] = AllSprites->Add(&ThisMaterial, 6,173, 96,59);
	
	VehiclePositions[0] = Vector(82.0f,105.0f,1.0f);
	VehiclePositions[1] = Vector(82.0f,172.0f,1.0f);
	VehiclePositions[2] = Vector(77.0f,256.0f,1.0f);
	VehiclePositions[3] = Vector(85.0f,331.0f,1.0f);

	for (i=0 ; i<4 ; i++)
	{
		AllSprites->SetPosition(GreyedVehicles[i], &VehiclePositions[i]);
		AllSprites->SetPosition(WhiteVehicles[i], &VehiclePositions[i]);
		AllSprites->Hide(GreyedVehicles[i]);
		AllSprites->Hide(WhiteVehicles[i]);
	}

	AllSprites->LoadTexture(&ThisMaterial, NULL, "Text_a");//, ScreenArchive);
	ChooseClassText = AllSprites->Add(&ThisMaterial, 0,157,256,22);
	AllSprites->SetPosition(ChooseClassText, &Vector(209.0f,91.0f,1.0f));
	AllSprites->Hide(ChooseClassText);
/*
	switch (MainGameInfo.RaceMode)
	{
	case 2:
		ModeText = AllSprites->Add(&ThisMaterial, 0,4,256,34);
		break;
	case 0:
		ModeText = AllSprites->Add(&ThisMaterial, 0,47,256,26);
		break;
	case 1:
		ModeText = AllSprites->Add(&ThisMaterial, 0,133,256,26);
		break;
	}
*/
	GlobalSprites->SetPosition(RaceModeTexts[MainGameInfo.RaceMode], &Vector(32.0f, 52.0f, 1.0f));
	GlobalSprites->Hide(RaceModeTexts[MainGameInfo.RaceMode]);

	for (i=0 ; i<4 ; i++)
	{
		VehicleText[i] = AllSprites->Add(&ThisMaterial, 0,179+14*i,123,14);
		AllSprites->SetPosition(VehicleText[i], &Vector(72.0f,407.0f, 1.0f));
		AllSprites->Hide(VehicleText[i]);
	}

#if defined(IGOR)
#else
	arcPop();
#endif

	GlobalSprites->Show(Line);
	for (i=0 ; i<3 ; i++) GlobalSprites->Show(Banner[i]);
	for (i=0 ; i<3 ; i++) GlobalSprites->Show(CurveSections[i]);
	for (i=0 ; i<4 ; i++) GlobalSprites->Show(Grid[i]);

	SelectionCycler = new CycledBox(0xFFFFFFFF, 0xFF444444, 0x00A0A040, 0xFFFFFFFF);
	RedCycler = new CycledBox(0xFFDA3760, 0xFFFFFFCC, 0x004040A0, 0xFFFFFFFF);

	CurrentSelection = 0;
	SelectionY = 90;
	CyclePeriod = 30;
	Delay = 0;

	// Load in the words.
	AllWords = new WordList();

	// Reset the counter to zero.
	Counter = 0;
	MaxSlideCounter = 30;
	SlideCounter = MaxSlideCounter;

	Mode = CCS_CHOOSETYPE;

	for (i=0 ; i<4 ; i++)
		MaxOfType[i] = 15;

	LoadModels();

	Canceling = false;
	QueuedCommand = 0;

	// Use the ChooseClass background.
	UseDefaultBackdrop = true;
}

ChooseClassScreen::~ChooseClassScreen()
{
}

void ChooseClassScreen::MoveVehicle(FrontEndObject *ThisObject)
{
	ThisObject->Rotate(8.0f+(float)cos(Counter*0.01f)*2.0f,220.0f+(float)sin(Counter*0.013f)*1.5f,0.0f);
}

void ChooseClassScreen::Update(struct Instruction *ScreenCommand)
{
	if (QueuedCommand == 'S')
	{
		ScreenCommand->Command = QueuedCommand;
		ScreenCommand->Value = QueuedValue;
		QueuedCommand = 0;
		return;
	}

	if (!AllSprites)
	{
		ScreenCommand->Command = 'D';
		ScreenCommand->Value = NULL;
		return;
	}

	Counter++;
	LetterCounter++;

	float NewCounter;
	NewCounter = (float)(Counter%(2*CyclePeriod));
	if (NewCounter > CyclePeriod) NewCounter = (float)(2*CyclePeriod - NewCounter);
	NewCounter = NewCounter/(float)CyclePeriod;

	if (Mode == CCS_CHOOSETYPE)
		AllSprites->SetColour(WhiteBox, SelectionCycler->CurrentColour(NewCounter));

	if (SlideCounter || FadeOut)
	{
		if (FadeOut)
		{
			SlideCounter++;
			for (int i=0 ; i<3 ; i++)
			{
				AllSprites->Hide(Banner[i]);
				AllSprites->Hide(CurveSections[i]);
			}
			AllSprites->Hide(WhiteBox);
			AllSprites->Hide(Line);
			for (i=0 ; i<4 ; i++)
				AllSprites->Hide(Grid[i]);
		}
		else
		{
			if (Delay)
				Delay--;
			else
				SlideCounter--;
		}
		float NewPosition;
		if (SlideCounter)
		{
			if (!Delay)
			{
				if (SlideCounter < 20)
					NewPosition = -270.0f+302.0f*(float)sin((20.0f-(float)SlideCounter)/20.0f*PI/2.0f);
				else
					NewPosition = -300.0f;
				float Rotation = (1.0f-(float)SlideCounter/(float)MaxSlideCounter)*PI/2.0f;
				
				if (!FadeOut || Canceling ) 
					GlobalSprites->SetPosition(RaceModeTexts[MainGameInfo.RaceMode], &Vector(NewPosition, 52.0f, 1.0f));
				GlobalSprites->Show(RaceModeTexts[MainGameInfo.RaceMode]);
				AllSprites->SetPosition(ChooseClassText, NULL, NULL, &Vector(1.0f, (float)fabs(sin(Rotation)), 5.3f));
				if (Mode == CCS_CHOOSETYPE) AllSprites->Show(ChooseClassText);
			}
			else
			{
				for (int i=0 ; i<4 ; i++)
					AllSprites->Hide(GreyedVehicles[i]);
				AllSprites->Hide(WhiteVehicles[CurrentSelection]);
				AllSprites->Hide(WhiteBox);
				AllSprites->Hide(VehicleText[CurrentSelection]);
			}
		}
		else 
		{
			NewPosition = 73.0f;
			for (int i=0 ; i<4 ; i++)
				AllSprites->Show(GreyedVehicles[i]);
			AllSprites->Show(WhiteVehicles[CurrentSelection]);
			AllSprites->Show(WhiteBox);
			if (Mode == CCS_CHOOSETYPE)	AllSprites->Show(VehicleText[CurrentSelection]);
		}

		if (SlideCounter > 10)
			NewPosition = -120.0f+193.0f*(float)sin((20.0f-(float)(SlideCounter-10.0f))/20.0f*PI/2.0f);
		else
			NewPosition = 73.0f;
		for (int i=0 ; i<4 ; i++)
			AllSprites->SetPosition(GreyedBox[i], &Vector(NewPosition, (float)(91+i*77), 1.0f));
		AllSprites->SetPosition(GreyedBoxLid, &Vector(NewPosition, 90.0f, 1.0f));
		AllSprites->SetPosition(WhiteBox, &Vector(NewPosition, (float)(89+CurrentSelection*77),0.95f));
		AllSprites->SetPosition(OrangeFade, &Vector(NewPosition, 91.0f, 1.0f), NULL, &Vector(1.0f, 4.0f, 8.3f));
	}
	
	
	int OldSelectionY = SelectionY;
	int TargetY = 89+(SelectedVehicleType*77);
	
	if (TargetY > SelectionY)
	{
		SelectionY+=10;
		
		AllSprites->SetPosition(WhiteBox, &Vector(73.0f, (float)SelectionY, 0.9f));
	}
	else if (TargetY < SelectionY)
	{
		SelectionY-=10;
		
		AllSprites->SetPosition(WhiteBox, &Vector(73.0f, (float)SelectionY, 0.9f));
	}
	
	if (   OldSelectionY < TargetY && SelectionY > TargetY
		|| OldSelectionY > TargetY && SelectionY < TargetY)
	{
		SelectionY = TargetY;
		AllSprites->SetPosition(WhiteBox, &Vector(73.0f, (float)SelectionY, 0.9f));
	}
	
	bool ChangeText = false;
	if (!SlideCounter)
	{
		if ((float)((SelectionY-90)%77)/77.0f < 0.5f && SelectionY < OldSelectionY)
			ChangeText = true;
		if ((float)((SelectionY-90)%77)/77.0f > 0.5f && SelectionY > OldSelectionY)
			ChangeText = true;
	}
	
	if (ChangeText)
	{
		int Old, New;
		New = OldSelectionY > SelectionY ? (SelectionY-90+32)/77 : (SelectionY-90+33)/77;
		Old = OldSelectionY > SelectionY ? New+1 : New-1;
		if (Old<0) Old=0;
		AllSprites->Hide(WhiteVehicles[Old]);
		AllSprites->Show(WhiteVehicles[New]);
		AllSprites->Hide(VehicleText[Old]);
		AllSprites->Show(VehicleText[New]);
	}

	
	if (Mode == CCS_CONFIRM)
	{
		NewCounter = (float)(LetterCounter%(2*CyclePeriod));
		if (NewCounter > CyclePeriod) NewCounter = (float)(2*CyclePeriod - NewCounter);
		NewCounter = NewCounter/(float)CyclePeriod;
		
		switch (CurrentSelection)
		{
		case 0:
			AllWords->SetColour(VehicleSelectionString, SelectionCycler->CurrentColour(NewCounter));
			break;
		case 1:
			GlobalSprites->SetColour(FontBox[0], RedCycler->CurrentColour(NewCounter));
			AllWords->SetColour(LiveryChoices[MainGameInfo.LiveryID-1], SelectionCycler->CurrentColour(NewCounter));
			break;
		case 2:
			GlobalSprites->SetColour(FontBox[1], RedCycler->CurrentColour(NewCounter));
			AllWords->SetColour(ColourChoices[MainGameInfo.ColourID-1], SelectionCycler->CurrentColour(NewCounter));
			break;
		case 3:
			AllWords->SetColour(OKString, SelectionCycler->CurrentColour(NewCounter));
			break;
		}

		MoveVehicle(VehicleObject[SelectedVehicleType]);
	}
	
	AllSprites->DrawAll();
	AllWords->DrawAll();

	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;

	if (SlideCounter > MaxSlideCounter || Delay && FadeOut)
	{
		Destroy();
		ScreenCommand->Command = 'D';
		ScreenCommand->Value = NULL;
	}

	QueuedCommand = 0;
}

void ChooseClassScreen::DrawOneOfXString(int Current, int Selection)
{
	char Buffer[10];
	sprintf(Buffer, "%d OF %d", Current, Selection);
	VehicleSelectionString = AllWords->AddWord(SmallFont, Buffer, &Vector(310.0f, 140.0f, 0.95f), NULL, 0xFFA0A0A0);
}

void ChooseClassScreen::LoadModels()
{
	return;
	VehicleObject[0] = new FrontEndObject("..\\FrontLibData\\Nissan.tc", "Nissan", 1.0f, NULL, NULL);
	VehicleObject[1] = new FrontEndObject("..\\FrontLibData\\BMW650.tc", "BMW", 1.0f, NULL, NULL);
	VehicleObject[2] = new FrontEndObject("..\\FrontLibData\\Nissan.tc", "Nissan", 1.0f, NULL, NULL);
	VehicleObject[3] = new FrontEndObject("..\\FrontLibData\\BMW650.tc", "BMW", 1.0f, NULL, NULL);

	for (int i=0 ; i<4 ; i++)
	{
		VehicleObject[i]->Draw(454,284,7.0f);
		VehicleObject[i]->Hide();
	}
}

int ChooseClassScreen::ControlPressed(int ControlPacket)
{
	int i;
	switch (Mode)
	{
	case CCS_CHOOSETYPE:
		if (ControlPacket & (1 << CON_UP) && CurrentSelection) 
			CurrentSelection--;
		if (ControlPacket & (1 << CON_DOWN) && CurrentSelection<3) 
			CurrentSelection++;

		SelectedVehicleType = CurrentSelection;

		if (ControlPacket & (1 << CON_A))
		{
			LetterCounter = 0;
				
			AllSprites->SetColour(WhiteBox, 0xFFFFFFFF);

			CurrentSelection = 3;
			Mode = CCS_CONFIRM;
			AllSprites->Hide(ChooseClassText);
			VehicleName = AllWords->AddWord(LargeFont, "Nissan Patrol", &Vector(207.0f,92.0f,0.95f), NULL);
			VehicleString = AllWords->AddWord(SmallFont, "VEHICLE", &Vector(220.0f, 139.0f, 0.95f), NULL, 0xFFA0A0A0);
			DrawOneOfXString(MainGameInfo.VehicleType,15);
			LiveryString = AllWords->AddWord(SmallFont, "LIVERY", &Vector(220.0f, 160.0f, 0.95f), NULL, 0xFFA0A0A0);
			ColourString = AllWords->AddWord(SmallFont, "COLOUR", &Vector(220.0f, 180.0f, 0.95f), NULL, 0xFFA0A0A0);

			char Buffer[2];
			for (int i=0 ; i<5 ; i++)
			{
				sprintf(Buffer, "%d", i+1);
				LiveryChoices[i] = AllWords->AddWord(SmallFont, Buffer, &Vector(310.0f+15.0f*i, 166.0f, 0.95f), NULL, 0xFFA0A0A0);
				AllWords->Justify(LiveryChoices[i], 5);
				sprintf(Buffer, "%c", i+65);
				ColourChoices[i] = AllWords->AddWord(SmallFont, Buffer, &Vector(310.0f+15.0f*i, 186.0f, 0.95f), NULL, 0xFFA0A0A0);
				AllWords->Justify(ColourChoices[i], 5);
			}
			OKString = AllWords->AddWord(SmallFont, "OK", &Vector(220.0f, 200.0f, 0.95f), NULL);

			GlobalSprites->SetPosition(FontBox[0], &Vector(302.0f+15.0f*(MainGameInfo.LiveryID-1), 158.0f, 0.98f), NULL, NULL);
			GlobalSprites->SetColour(FontBox[0], 0xFFFF512F);
			GlobalSprites->SetPosition(FontBox[1], &Vector(302.0f+15.0f*(MainGameInfo.ColourID-1), 178.0f, 0.98f), NULL, NULL);
			GlobalSprites->SetColour(FontBox[1], 0xFFFF512F);
			GlobalSprites->Show(FontBox[0]);
			GlobalSprites->Show(FontBox[1]);

// SNARK			VehicleObject[SelectedVehicleType]->Draw();
		}

		if (ControlPacket & (1 << CON_Y))
		{
			FadeOut = true;
			
			for (int i=0 ; i<4 ; i++)
			{
				AllSprites->Hide(WhiteVehicles[i]);
				AllSprites->Hide(GreyedVehicles[i]);
			}
			AllSprites->Hide(VehicleText[CurrentSelection]);
			GlobalSprites->Hide(RaceModeTexts[MainGameInfo.RaceMode]);
			AllSprites->Hide(ChooseClassText);
			QueuedCommand = 'S';
			QueuedValue = (Screen *)new MainScreen(NULL);
			((MainScreen *)QueuedValue)->SetDelay(10);

			Canceling = true;
		}
		break;
	case CCS_CONFIRM:
		if (ControlPacket & (1 << CON_UP) && CurrentSelection)
		{
			CurrentSelection--;
			switch (CurrentSelection)
			{
			case 0:
				AllWords->SetColour(VehicleString, 0xFFFFFFFF);
				AllWords->SetColour(VehicleSelectionString, 0xFFFFFFFF);
				AllWords->SetColour(LiveryString, 0xFFA0A0A0);
				for (i=0 ; i<5 ; i++) AllWords->SetColour(LiveryChoices[i], 0xFFA0A0A0);
				GlobalSprites->SetColour(FontBox[0], 0xFFFF512F);
				break;
			case 1:
				AllWords->SetColour(LiveryString, 0xFFFFFFFF);
				for (i=0 ; i<5 ; i++) AllWords->SetColour(LiveryChoices[i], 0xFFFFFFFF);
				AllWords->SetColour(ColourString, 0xFFA0A0A0);
				for (i=0 ; i<5 ; i++) AllWords->SetColour(ColourChoices[i], 0xFFA0A0A0);
				GlobalSprites->SetColour(FontBox[1], 0xFFFF512F);
				break;
			case 2:
				AllWords->SetColour(ColourString, 0xFFFFFFFF);
				for (i=0 ; i<5 ; i++) AllWords->SetColour(ColourChoices[i], 0xFFFFFFFF);
				AllWords->SetColour(OKString, 0xFFA0A0A0);
				break;
			}
			LetterCounter = 0;
		}
		if (ControlPacket & (1 << CON_DOWN) && CurrentSelection < 3)
		{
			CurrentSelection++;
			switch (CurrentSelection)
			{
			case 1:
				AllWords->SetColour(VehicleString, 0xFFA0A0A0);
				AllWords->SetColour(VehicleSelectionString, 0xFFA0A0A0);
				AllWords->SetColour(LiveryString, 0xFFFFFFFF);
				for (i=0 ; i<5 ; i++) AllWords->SetColour(LiveryChoices[i], 0xFFFFFFFF);
				break;
			case 2:
				AllWords->SetColour(LiveryString, 0xFFA0A0A0);
				for (i=0 ; i<5 ; i++) AllWords->SetColour(LiveryChoices[i], 0xFFA0A0A0);
				GlobalSprites->SetColour(FontBox[0], 0xFFFF512F);
				AllWords->SetColour(ColourString, 0xFFFFFFFF);
				for (i=0 ; i<5 ; i++) AllWords->SetColour(ColourChoices[i], 0xFFFFFFFF);
				break;
			case 3:
				AllWords->SetColour(ColourString, 0xFFA0A0A0);
				for (i=0 ; i<5 ; i++) AllWords->SetColour(ColourChoices[i], 0xFFA0A0A0);
				GlobalSprites->SetColour(FontBox[1], 0xFFFF512F);
				AllWords->SetColour(OKString, 0xFFFFFFFF);
				break;
			}
			LetterCounter = 0;
		}

		if (ControlPacket & ((1 << CON_LEFT) | (1 << CON_RIGHT)))
		{
			switch (CurrentSelection)
			{
			case 0:
				if (ControlPacket & (1 << CON_RIGHT) && MainGameInfo.VehicleType < MaxOfType[SelectedVehicleType])
					MainGameInfo.VehicleType++;
				if (ControlPacket & (1 << CON_LEFT) && MainGameInfo.VehicleType > 1)
					MainGameInfo.VehicleType--;
				
				AllWords->Delete(VehicleSelectionString);
				DrawOneOfXString(MainGameInfo.VehicleType, MaxOfType[SelectedVehicleType]);
				AllWords->SetColour(VehicleSelectionString, 0xFFFFFFFF);
				break;

			case 1:
				AllWords->SetColour(LiveryChoices[MainGameInfo.LiveryID-1], 0xFFFFFFFF);
				if (ControlPacket & (1 << CON_RIGHT) && MainGameInfo.LiveryID < 5)
					MainGameInfo.LiveryID++;
				if (ControlPacket & (1 << CON_LEFT) && MainGameInfo.LiveryID > 1)
					MainGameInfo.LiveryID--;
				GlobalSprites->SetPosition(FontBox[0], &Vector(302.0f+15.0f*(MainGameInfo.LiveryID-1), 158.0f, 0.98f), NULL, NULL);
				break;
			case 2:
				AllWords->SetColour(ColourChoices[MainGameInfo.ColourID-1], 0xFFFFFFFF);
				if (ControlPacket & (1 << CON_RIGHT) && MainGameInfo.ColourID < 5)
					MainGameInfo.ColourID++;
				if (ControlPacket & (1 << CON_LEFT) && MainGameInfo.ColourID > 1)
					MainGameInfo.ColourID--;
				GlobalSprites->SetPosition(FontBox[1], &Vector(302.0f+15.0f*(MainGameInfo.ColourID-1), 178.0f, 0.98f), NULL, NULL);
				break;
			case 3:
				break;
			}
			LetterCounter = 0;
		}
		
		if (ControlPacket & (1 << CON_Y))
		{
			AllWords->Delete(VehicleName);
			AllWords->Delete(VehicleString);
			AllWords->Delete(VehicleSelectionString);
			GlobalSprites->Hide(FontBox[0]);
			GlobalSprites->Hide(FontBox[1]);
			
			AllSprites->Show(ChooseClassText);
			Mode = CCS_CHOOSETYPE;
			CurrentSelection = SelectedVehicleType;

// SNARK 		VehicleObject[SelectedVehicleType]->Hide();
		}

		if (ControlPacket & (1 << CON_A))
		{
			AllWords->Delete(VehicleName);
			AllWords->Delete(VehicleString);
			AllWords->Delete(VehicleSelectionString);
			GlobalSprites->Hide(FontBox[0]);
			GlobalSprites->Hide(FontBox[1]);

			GlobalSprites->Hide(Line);
			for (i=0 ; i<3 ; i++) GlobalSprites->Hide(Banner[i]);
			for (i=0 ; i<3 ; i++) GlobalSprites->Hide(CurveSections[i]);
			for (i=0 ; i<4 ; i++) GlobalSprites->Hide(Grid[i]);


			for (int i=0 ; i<4 ; i++)
			{
				AllSprites->Hide(WhiteVehicles[i]);
				AllSprites->Hide(GreyedVehicles[i]);
//				GlobalSprites->Hide(RaceModeTexts[i]);
			}

			AllSprites->Show(ChooseClassText);
			Mode = CCS_CHOOSETYPE;
			CurrentSelection = SelectedVehicleType;

// SNARK			VehicleObject[SelectedVehicleType]->Hide();

			FadeOut = true;

			QueuedCommand = 'S';
			QueuedValue = (Screen *)new TrackDetailsScreen(NULL);
			((TrackDetailsScreen *)QueuedValue)->SetDelay(10);
		}

		break;
	}
	return 0;
}

void ChooseClassScreen::DrawBackground()
{
}

void ChooseClassScreen::Destroy()
{
	ScreenArchive->Close();

	delete AllSprites;	AllSprites = NULL;
	delete AllWords;	
	delete SelectionCycler;

	if (Canceling)
		for (int i=0 ; i<3 ; i++)
			GlobalSprites->Hide(RaceModeTexts[i]);
// SNARK	for (int i=0 ; i<4 ; i++) delete VehicleObject[i];

}
