// $Header

// $Log

// Default screen class

#define D3D_OVERLOADS

#include "../ConsoleFrontLib.h"

#define MAP_STARTX	0.0f
#define MAP_STARTY	0.0f

#define VIEWPORT_X 430.0f
#define VIEWPORT_Y 150.0f
#define VIEWPORT_W 150.0f
#define VIEWPORT_H 103.0f

#define VIEWPORT_SPEED 50.0f

struct ViewPortDetails
{
	float ViewedX, ViewedY, ViewedW, ViewedH;
	float WindowX, WindowY, WindowW, WindowH;
	float MapScaleX, MapScaleY;
	Vector2 MapOffset;
	int Colour;
	bool RenderViewPort;
};

bool CheckForViewPort(Vector2 *TVector2, Vector2 *TVector1, Vector2 *a, Vector2 *b, struct ViewPortDetails *ViewedPort)
{
	// Cohen-Sutherland time...
	int Line1=0, Line2=0;
	float x1,y1,x2,y2;

	x1 = ViewedPort->ViewedX;
	y1 = ViewedPort->ViewedY;
	x2 = ViewedPort->ViewedX+ViewedPort->ViewedW;
	y2 = ViewedPort->ViewedY+ViewedPort->ViewedH;

	Vector2 *ThisLine = TVector1;
	int *ThisResult = &Line1;
	
	for (int i=0 ; i<2 ; i++, ThisResult = &Line2, ThisLine = TVector2)
	{
		if (ThisLine->y < y1)
		{
			if (ThisLine->x < x1)								*ThisResult = 9;
			else if (ThisLine->x >= x1 && ThisLine->x <= x2)	*ThisResult = 1;
			else if (ThisLine->x > x2)							*ThisResult = 5;
		}
		else if (ThisLine->y >= y1 && ThisLine->y <=y2)
		{
			if (ThisLine->x < x1)								*ThisResult = 8;
			else if (ThisLine->x > x2)							*ThisResult = 4;
		}
		else if (ThisLine->y > y2)
		{
			if (ThisLine->x < x1)								*ThisResult = 10;
			else if (ThisLine->x >= x1 && ThisLine->x <= x2)	*ThisResult = 2;
			else if (ThisLine->x > x2)	
				*ThisResult = 6;
		}
	}

	if (!(Line1 | Line2))
	{
#if defined (IGOR)
		// SNARK
		int LineVertices[2];
#else
		renderSetCurrentMaterial(NULL);
		D3DTLVERTEX LineVertices[2];
#endif	
		Vector2 Transformed[2];
		float ZoomRatio = ViewedPort->WindowW / ViewedPort->ViewedW;

		Vector2 OffsetToZero, OffsetFromZero;
		OffsetToZero.x = - (ViewedPort->ViewedX + (ViewedPort->ViewedW * 0.5f));
		OffsetToZero.y = -(480 - (ViewedPort->ViewedY + (ViewedPort->ViewedH * 0.5f)));
		OffsetFromZero.x = (ViewedPort->WindowX + (ViewedPort->WindowW * 0.5f));
		OffsetFromZero.y = (ViewedPort->WindowY + (ViewedPort->WindowH * 0.5f));

		Transformed[0].x = (a->x - MAP_STARTX)*ViewedPort->MapScaleX + ViewedPort->MapOffset.x + OffsetToZero.x;
		Transformed[0].y = ((a->y - MAP_STARTY)*ViewedPort->MapScaleY + (480 - ViewedPort->MapOffset.y) + OffsetToZero.y);
		Transformed[1].x = (b->x - MAP_STARTX)*ViewedPort->MapScaleX + ViewedPort->MapOffset.x + OffsetToZero.x;
		Transformed[1].y = ((b->y - MAP_STARTY)*ViewedPort->MapScaleY + (480 -ViewedPort->MapOffset.y) + OffsetToZero.y);

		Transformed[0] *= ZoomRatio;
		Transformed[1] *= ZoomRatio;

		Transformed[0].x += OffsetFromZero.x;
		Transformed[0].y = 480-(Transformed[0].y + 480-OffsetFromZero.y);
		Transformed[1].x += OffsetFromZero.x;
		Transformed[1].y = 480-(Transformed[1].y + 480-OffsetFromZero.y);

#if defined (IGOR)
// SNARK
#else
		LineVertices[0] = _D3DTLVERTEX(_D3DVECTOR(Transformed[0].x, Transformed[0].y, 0.999f), 0.001f, ViewedPort->Colour, 0xFFFFFFFF, 0.5f, 0.5f);
		LineVertices[1] = _D3DTLVERTEX(_D3DVECTOR(Transformed[1].x, Transformed[1].y, 0.999f), 0.001f, ViewedPort->Colour, 0xFFFFFFFF, 0.5f, 0.5f);
		renderLine((D3DTLVERTEX *)LineVertices, 2);
#endif	
		
		return true;
	}
	else 
		return false;
}

void DrawSplineSection(Vector2 * a, Vector2 * b, float t, void * data, int spno, float res)
{
#if defined (IGOR)
	int LineVertices[2];
#else
	D3DTLVERTEX LineVertices[2];
#endif
	Vector2 Transformed[2];

	Transformed[0].x = (a->x - MAP_STARTX)*((struct ViewPortDetails *)data)->MapScaleX + ((struct ViewPortDetails *)data)->MapOffset.x;
	Transformed[0].y = ((struct ViewPortDetails *)data)->MapOffset.y - (a->y - MAP_STARTY)*((struct ViewPortDetails *)data)->MapScaleY;
	Transformed[1].x = (b->x - MAP_STARTX)*((struct ViewPortDetails *)data)->MapScaleX + ((struct ViewPortDetails *)data)->MapOffset.x;
	Transformed[1].y = ((struct ViewPortDetails *)data)->MapOffset.y - (b->y - MAP_STARTY)*((struct ViewPortDetails *)data)->MapScaleY;

#if defined (IGOR)
	// SNARK
#else
	LineVertices[0] = _D3DTLVERTEX(_D3DVECTOR(Transformed[0].x, Transformed[0].y, 0.999f), 0.001f, ((struct ViewPortDetails *)data)->Colour, 0xFFFFFFFF, 0.5f, 0.5f);
	LineVertices[1] = _D3DTLVERTEX(_D3DVECTOR(Transformed[1].x, Transformed[1].y, 0.999f), 0.001f, ((struct ViewPortDetails *)data)->Colour, 0xFFFFFFFF, 0.5f, 0.5f);

	renderSetCurrentMaterial(NULL);
	renderLine((D3DTLVERTEX *)LineVertices, 2);
#endif
	if (((struct ViewPortDetails *)data)->RenderViewPort)
		CheckForViewPort(&Transformed[0], &Transformed[1], a, b, 
		(struct ViewPortDetails *) data);
}

TrackDetailsScreen::TrackDetailsScreen(Alphabet *_MainAlphabet)
{
	MainAlphabet = _MainAlphabet;

	AllSprites = new SpriteList();
#if defined (IGOR)
	ScreenArchive = NewArchive();
	ScreenArchive->Open(FRONTEND_DATA "TrackDetails.tc");
#else
	arcPush(".\\FrontLibData\\TrackDetails.tc");
#endif

	Render2D ThisMaterial;
	AllSprites->LoadTexture(&ThisMaterial, NULL, "SavannahPanel.3df");

	Panel[0] = AllSprites->Add(&ThisMaterial, 0,0, 101,128);
	Panel[1] = AllSprites->Add(&ThisMaterial, 100,0,101,128);
	Panel[2] = AllSprites->Add(&ThisMaterial, 200,-1,55,101);

	AllSprites->LoadTexture(&ThisMaterial, NULL, "MainIcons.tga");

	WhiteIcons[0] = AllSprites->Add(&ThisMaterial, 2,1,71,56);
	WhiteIcons[1] = AllSprites->Add(&ThisMaterial, 2,62,71,56);
	WhiteIcons[2] = AllSprites->Add(&ThisMaterial, 2,123,71,56);
	WhiteIcons[3] = AllSprites->Add(&ThisMaterial, 2,184,71,56);
	GreyedIcons[0] = AllSprites->Add(&ThisMaterial, 79,1,71,56);
	GreyedIcons[1] = AllSprites->Add(&ThisMaterial, 79,62,71,56);
	GreyedIcons[2] = AllSprites->Add(&ThisMaterial, 79,123,71,56);
	GreyedIcons[3] = AllSprites->Add(&ThisMaterial, 79,184,71,56);
	for (int i=1 ; i<4 ; i++)
		AllSprites->Hide(WhiteIcons[i]);

	for (i=0 ; i<4 ; i++)
		IconPositions[i] = Vector(76.0f,92.0f+i*61.0f,1.0f);

	for (i=0 ; i<4 ; i++)
	{
		OrangeBox[i] = AllSprites->Add(&ThisMaterial, 157,68, 77,62);
		AllSprites->SetPosition(OrangeBox[i], &Vector(74.0f, (float)(90+i*60), 0.95f));
		AllSprites->Hide(OrangeBox[i]);
	}

	for (i=0 ; i<4 ; i++)
	{
		AllSprites->SetPosition(GreyedIcons[i], &IconPositions[i]);
		AllSprites->SetPosition(WhiteIcons[i], &IconPositions[i]);
		AllSprites->Hide(GreyedIcons[i]);
		AllSprites->Hide(WhiteIcons[i]);
	}

	for (i=0 ; i<4 ; i++)
	{
		GreyedBox[i] = AllSprites->Add(&ThisMaterial, 157,2,77,62);
		AllSprites->SetPosition(GreyedBox[i], &Vector(74.0f, (float)(90+i*60), 1.0f));
		AllSprites->SetColour(GreyedBox[i], 0xFFA0A0A0);
		AllSprites->Hide(GreyedBox[i]);
	}
	WhiteBox = AllSprites->Add(&ThisMaterial, 157,2,77,62);
	AllSprites->Hide(WhiteBox);

	AllSprites->LoadTexture(&ThisMaterial, NULL, "LowerText.3df");

	SelectionTexts[0] = AllSprites->Add(&ThisMaterial, 2,0, 172,14);
	SelectionTexts[1] = AllSprites->Add(&ThisMaterial, 2,14, 172,14);
	SelectionTexts[2] = AllSprites->Add(&ThisMaterial, 2,28, 172,14);
	SelectionTexts[3] = AllSprites->Add(&ThisMaterial, 2,42, 172,14);
	
	AllSprites->LoadTexture(&ThisMaterial, NULL, "IG_Track1.3df");
	MapMockup[0] = AllSprites->Add(&ThisMaterial, 0,0,256,160);
	AllSprites->LoadTexture(&ThisMaterial, NULL, "IG_Track2.3df");
	MapMockup[1] = AllSprites->Add(&ThisMaterial, 0,0,128,160);

	AllSprites->SetPosition(MapMockup[0], &Vector(215.0f, 210.0f, 1.0f));
	AllSprites->SetPosition(MapMockup[1], &Vector(471.0f, 210.0f, 1.0f), &Vector(128.0f, 256.0f, 1.0f));

	AllSprites->Hide(MapMockup[0]);
	AllSprites->Hide(MapMockup[1]);

#if defined (IGOR)
	FreeArchive(ScreenArchive);
#else
	arcPop();
#endif
	AllWords = new WordList();

	for (i=0 ; i<3 ; i++)
		AllWords->Hide(SelectionTexts[i]);

	NameText = AllWords->AddWord(MiddleFont, "Screaming Savannah", &Vector(160.0f, 116.0f, 1.0f), NULL, 0xFFA0A0A0);
	StageText = AllWords->AddWord(LargeFont, "Stage 4", &Vector(160.0f, 90.0f, 1.0f), NULL, 0xFFFFFFFF);
	DescriptionTexts[0] = AllWords->AddWord(SmallFont, "Distance", &Vector(171.0f, 169.0f, 1.0f), NULL, 0xFFD0D0D0);
	DescriptionTexts[1] = AllWords->AddWord(SmallFont, "Terrain", &Vector(171.0f, 189.0f, 1.0f), NULL, 0xFFD0D0D0);
	DescriptionTexts[2] = AllWords->AddWord(SmallFont, "Routes", &Vector(171.0f, 209.0f, 1.0f), NULL, 0xFFD0D0D0);
	DescriptionValues[0] =AllWords->AddWord(SmallFont, "22.1km", &Vector(280.0f, 169.0f, 1.0f), NULL, 0xFFD0D0D0);
	DescriptionValues[1] =AllWords->AddWord(SmallFont, "Dusty Grassland", &Vector(280.0f, 189.0f, 1.0f), NULL, 0xFFD0D0D0);
	DescriptionValues[2] =AllWords->AddWord(SmallFont, "8", &Vector(280.0f, 209.0f, 1.0f), NULL, 0xFFD0D0D0);

	for (i=0 ; i<4 ; i++)
	{
		AllSprites->SetPosition(SelectionTexts[i], &Vector(74.0f, 406.0f, 0.95f), &Vector(256.0f, 64.0f, 1.0f));
		AllSprites->Hide(SelectionTexts[i]);
	}

	for (i=0 ; i<3 ; i++)
		AllSprites->Show(Panel[i]);

	SlideCounter = MaxSlideCounter = 20;

	for (i=0 ; i<3 ; i++)
	{
		AllSprites->SetPosition(SelectionTexts[i], &Vector(74.0f, 406.0f, 0.95f));
		AllSprites->Hide(SelectionTexts[i]);
	}

	// Start setting up car configuration
	for (i=0 ; i<8 ; i+=2)
	{
		LocalHorizLines[i] = GlobalSprites->Clone(HorizontalLines[2]);
		LocalHorizLines[i+1] = GlobalSprites->Clone(HorizontalLines[5]);
		GlobalSprites->Justify(LocalHorizLines[i+1], 6);
		
		if (i<6)
		{
			GlobalSprites->SetPosition(LocalHorizLines[i], &Vector(199.0f, 171.0f+10.0f*i, 1.0f), &Vector(256.0f, 64.0f, 1.0f), &Vector(3.0f, 1.0f, 4.0f));
			GlobalSprites->SetPosition(LocalHorizLines[i+1], &Vector(199.0f, 171.0f+10.0f*i, 1.0f), &Vector(256.0f, 64.0f, 1.0f), NULL, 2); 
		}
		else
		{
			GlobalSprites->SetPosition(LocalHorizLines[i], &Vector(199.0f, 311.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f), &Vector(3.0f, 1.0f, 4.0f));
			GlobalSprites->SetPosition(LocalHorizLines[i+1], &Vector(199.0f, 311.0f, 1.0f), &Vector(256.0f, 64.0f, 1.0f), NULL, 2); 
		}
	}

	LocalVertLines[0] = GlobalSprites->Clone(VerticalLines[3]);
	LocalVertLines[1] = GlobalSprites->Clone(VerticalLines[2]);
	LocalVertLines[2] = GlobalSprites->Clone(VerticalLines[2]);
	LocalVertLines[3] = GlobalSprites->Clone(VerticalLines[1]);
	GlobalSprites->Justify(LocalVertLines[0],1);
	GlobalSprites->Justify(LocalVertLines[2],1);

	GlobalSprites->SetPosition(LocalVertLines[0], &Vector(190.0f, 200.0f, 1.0f), &Vector(64.0f, 256.0f, 1.0f), NULL, 2);
	GlobalSprites->SetPosition(LocalVertLines[1], &Vector(190.0f, 201.0f, 1.0f), &Vector(64.0f, 256.0f, 1.0f));

	GlobalSprites->SetPosition(LocalVertLines[2], &Vector(190.0f, 305.0f, 1.0f), &Vector(64.0f, 256.0f, 1.0f), NULL, 2);
	GlobalSprites->SetPosition(LocalVertLines[3], &Vector(190.0f, 306.0f, 1.0f), &Vector(64.0f, 256.0f, 1.0f));

	Titles[0] = AllWords->AddWord(MiddleFont, "Suspension", &Vector(205.0f, 115.0f, 1.0f), NULL, 0xFFA0A0A0);
	Titles[1] = AllWords->AddWord(MiddleFont, "Tyres", &Vector(204.0f, 258.0f, 1.0f), NULL, 0xFFA0A0A0);
	
	Headers[0] = AllWords->AddWord(SmallFont, "Stiffness", &Vector(215.0f, 157.0f, 1.0f), NULL, 0xFFA0A0A0);
	Headers[1] = AllWords->AddWord(SmallFont, "Shock Absorbers", &Vector(215.0f, 177.0f, 1.0f), NULL, 0xFFA0A0A0);
	Headers[2] = AllWords->AddWord(SmallFont, "Ride Height", &Vector(215.0f, 197.0f, 1.0f), NULL, 0xFFA0A0A0);
	Headers[3] = AllWords->AddWord(SmallFont, "Tyre Pressure", &Vector(215.0f, 297.0f, 1.0f), NULL, 0xFFA0A0A0);

	for (i=0 ; i<4 ; i++)
	{
		Values[i*3] = AllWords->AddWord(SmallFont, "low", &Vector(420.0f, (i==3 ? 237.0f : 157.0f)+i*20.0f, 1.0f), NULL, 0xFFA0A0A0);
		Values[i*3+1] = AllWords->AddWord(SmallFont, "med", &Vector(480.0f, (i==3 ? 237.0f : 157.0f)+i*20.0f, 1.0f), NULL, 0xFFA0A0A0);
		Values[i*3+2] = AllWords->AddWord(SmallFont, "high", &Vector(540.0f, (i==3 ? 237.0f : 157.0f)+i*20.0f, 1.0f), NULL, 0xFFA0A0A0);
	}
	
	for (i=0 ; i<4 ; i++)
		AllWords->SetColour(i*3+Values[MainGameInfo.CarPrefs[UsingPlayer].Prefs[i]], 0xFFFFFFFF);

	GlobalSprites->SetPosition(PSButtons[1], &Vector(436.0f, 402.0f, 1.0f), &Vector(64.0f, 64.0f, 1.0f));
	GlobalSprites->SetPosition(PSButtons[3], &Vector(538.0f, 402.0f, 1.0f), &Vector(64.0f, 64.0f, 1.0f));

	GlobalSprites->Show(Line);
	for (i=0 ; i<3 ; i++) GlobalSprites->Show(Banner[i]);
	for (i=0 ; i<3 ; i++) GlobalSprites->Show(CurveSections[i]);

	GlobalSprites->Hide(PSButtons[3]);

	SelectionCycler = new CycledBox(0xFFFFFFFF, 0xFF444444, 0x00A0A040, 0xFFFFFFFF);

	EditText = AllWords->AddWord(SmallFont, "Edit", &Vector(575.0f,408.0f, 1.0f), NULL, 0xFFD0D0D0);
	DoneText = AllWords->AddWord(SmallFont, "Done", &Vector(575.0f,408.0f, 1.0f), NULL, 0xFFD0D0D0);
	CancelText = AllWords->AddWord(SmallFont, "Back", &Vector(474.0f,408.0f, 1.0f), NULL, 0xFFD0D0D0);

	LocalRedShades[0] = GlobalSprites->Clone(RedShade);
	LocalRedShades[1] = GlobalSprites->Clone(RedShade);

	GlobalSprites->SetPosition(LocalRedShades[0], &Vector(191.0f, 126.0f, 1.0f), NULL, &Vector(207.0f/256.0f, 136.0f/ 256.0f, 4.1f));
	GlobalSprites->SetPosition(LocalRedShades[1], &Vector(191.0f, 262.0f, 1.0f), NULL, &Vector(207.0f/256.0f, 88.0f/ 256.0f, 4.1f));

	// Set up the help guide for the route selection.
	GlobalSprites->SetPosition(WhiteArrows[ARROW_LEFT], &Vector(230.0f,100.0f, 1.0f), &Vector(64.0f, 64.0f, 1.0f));
	GlobalSprites->SetPosition(WhiteArrows[ARROW_RIGHT],&Vector(243.0f,100.0f, 1.0f), &Vector(64.0f, 64.0f, 1.0f));
	GlobalSprites->Hide(WhiteArrows[ARROW_LEFT]);
	GlobalSprites->Hide(WhiteArrows[ARROW_RIGHT]);
	
	RouteWord = AllWords->AddWord(SmallFont, "Route", &Vector(266.0f, 98.0f, 1.0f), NULL, 0xFFD0D0D0);
	NextWord = AllWords->AddWord(SmallFont, "Next", &Vector(266.0f, 130.0f, 1.0f), NULL, 0xFFD0D0D0);
	BackWord = AllWords->AddWord(SmallFont, "Back", &Vector(266.0f, 160.0f, 1.0f), NULL, 0xFFD0D0D0);
	CancelWord = AllWords->AddWord(SmallFont, "Cancel", &Vector(473.0f, 408.0f, 1.0f), NULL, 0xFFD0D0D0);

	// Load the spline etc.
	TrackSplines = new SplineSet(".\\frontlibdata\\testsplines.tc", "test");
	SplineColours = (int *)malloc(TrackSplines->splineno * sizeof(int));
	for (i=0 ; i<TrackSplines->splineno ; i++)
		SplineColours[i] = 0xFFC0C0C0;

	int CurrentSpline;
	for (i=0 ; i<TrackSplines->splineno ; i++)
		if (TrackSplines->spline[i].start_junc == -1)
		{
			CurrentSpline = i;
			i=TrackSplines->splineno;
		}
	StartingSpline = CurrentSpline;

	if (MainGameInfo.Route[0] == 255)
	{
		// Use defaults.
		SetUpRouteDefaults();
	}

	ColourRoute();
	HideDetails(0);
	HideDetails(1);
	HideDetails(5);

	// Set up the zoom window.
	ViewPortPosition.x = 430.0f;	ViewPortPosition.y = 91.0f;
	ViewPortSize.x = 150.0f;		ViewPortSize.y = 103.0f;
	ViewedPosition.x = 400.0f;		ViewedPosition.y = 240.f;
	ViewedSize.x = 60.0f;			ViewedSize.y = 40.0f;

	MapOffset.x = 275.0f;			MapOffset.y = 360.0f;
	MapScale.x = 0.025f;			MapScale.y = 0.025f;

	CurrentSelection = 0;
	ShowDetails(CurrentSelection);

	// Hide the extra bits that shouldn`t appear immediately.
	AllWords->Hide(StageText);
	AllWords->Hide(NameText);
	for (i=0 ; i<3 ; i++)
	{
		AllWords->Hide(DescriptionTexts[i]);
		AllWords->Hide(DescriptionValues[i]);
	}

	SelectionY = 90;
	CyclePeriod = 30;

	DisplayedSubScreen = 0;

	// Reset the counter to zero.
	Counter = 0;
	MaxSlideCounter = 30;
	SlideCounter = MaxSlideCounter;
	
	// Use the default background.
	UseDefaultBackdrop = true;
}

TrackDetailsScreen::~TrackDetailsScreen()
{
}

void TrackDetailsScreen::ColourRoute()
{
	for (int i=0 ; i<TrackSplines->splineno ; i++)
		SplineColours[i] = 0xFFC0C0C0;
		
	int CurrentSpline = StartingSpline;
	while (CurrentSpline != -1)
	{
		SplineColours[CurrentSpline] = 0xFFFFFF80;
		if (TrackSplines->spline[CurrentSpline].end_junc == -1)
			CurrentSpline = -1;
		else
		{
			if (DisplayedSubScreen == 2)
				CurrentSpline = TrackSplines->junc[TrackSplines->spline[CurrentSpline].end_junc].splineout[MainGameInfo.Route[TrackSplines->spline[CurrentSpline].end_junc]];
			else
				CurrentSpline = TrackSplines->junc[TrackSplines->spline[CurrentSpline].end_junc].splineout[TemporaryRoute[TrackSplines->spline[CurrentSpline].end_junc]];
		}
	}
}

void TrackDetailsScreen::SetUpRouteDefaults()
{
	int CurrentSpline = StartingSpline;
	
	// Always use the first spline out of each junction.
	for (int i=0 ; i<TrackSplines->juncno ; i++)
		MainGameInfo.Route[i] = 0;

	while (CurrentSpline != -1)
	{
		SplineColours[CurrentSpline] = 0xFFFFFF80;
		if (TrackSplines->spline[CurrentSpline].end_junc == -1)
			CurrentSpline = -1;
		else
			CurrentSpline = TrackSplines->junc[TrackSplines->spline[CurrentSpline].end_junc].splineout[MainGameInfo.Route[TrackSplines->spline[CurrentSpline].end_junc]];
	}
}

void TrackDetailsScreen::DrawTrack()
{
	struct ViewPortDetails Data;
	Data.ViewedX = ViewedPosition.x;
	Data.ViewedY = ViewedPosition.y;
	Data.ViewedW = ViewedSize.x;
	Data.ViewedH = ViewedSize.y;

	Data.WindowX = ViewPortPosition.x;
	Data.WindowY = ViewPortPosition.y;
	Data.WindowW = ViewPortSize.x;
	Data.WindowH = ViewPortSize.y;

	Data.MapOffset = MapOffset;
	Data.MapScaleX = MapScale.x;
	Data.MapScaleY = MapScale.y;

	for (int i=0 ; i<TrackSplines->splineno ; i++)
	{
		Data.Colour = SplineColours[i];
		Data.RenderViewPort = (DisplayedSubScreen == 5 ? true : false);
		TrackSplines->spline[i].ProcessLines(0.2f, 0.0f, (float)TrackSplines->spline[i].pointno, NULL, &DrawSplineSection, (void *)&Data);
	}
	
#if defined (IGOR)
	int BoxVertices[8];
#else
	D3DTLVERTEX BoxVertices[8];
#endif
	if (DisplayedSubScreen == 5)
	{
		// Draw the box.
#if defined	(IGOR)
		// SNARK
#else
		BoxVertices[0] = _D3DTLVERTEX(_D3DVECTOR(ViewedPosition.x, ViewedPosition.y, 0.999f), 0.001f, 0xFFFFFFFF, 0xFFFFFFFF, 0.5f, 0.5f);
		BoxVertices[1] = _D3DTLVERTEX(_D3DVECTOR(ViewedPosition.x + ViewedSize.x, ViewedPosition.y, 0.999f), 0.001f, 0xFFFFFFFF, 0xFFFFFFFF, 0.5f, 0.5f);
		BoxVertices[2] = BoxVertices[1];
		BoxVertices[3] = _D3DTLVERTEX(_D3DVECTOR(ViewedPosition.x + ViewedSize.x, ViewedPosition.y+ViewedSize.y, 0.999f), 0.001f, 0xFFFFFFFF, 0xFFFFFFFF, 0.5f, 0.5f);
		BoxVertices[4] = BoxVertices[3];
		BoxVertices[5] = _D3DTLVERTEX(_D3DVECTOR(ViewedPosition.x, ViewedPosition.y+ViewedSize.y, 0.999f), 0.001f, 0xFFFFFFFF, 0xFFFFFFFF, 0.5f, 0.5f);
		BoxVertices[6] = BoxVertices[5];
		BoxVertices[7] = BoxVertices[0];
		
		renderSetCurrentMaterial(NULL);
		renderLine((D3DTLVERTEX *)BoxVertices, 8);

		BoxVertices[0] = _D3DTLVERTEX(_D3DVECTOR(ViewPortPosition.x, ViewPortPosition.y, 0.999f), 0.001f, 0xFFFFFFFF, 0xFFFFFFFF, 0.5f, 0.5f);
		BoxVertices[1] = _D3DTLVERTEX(_D3DVECTOR(ViewPortPosition.x + ViewPortSize.x, ViewPortPosition.y, 0.999f), 0.001f, 0xFFFFFFFF, 0xFFFFFFFF, 0.5f, 0.5f);
		BoxVertices[2] = BoxVertices[1];
		BoxVertices[3] = _D3DTLVERTEX(_D3DVECTOR(ViewPortPosition.x + ViewPortSize.x, ViewPortPosition.y+ViewPortSize.y, 0.999f), 0.001f, 0xFFFFFFFF, 0xFFFFFFFF, 0.5f, 0.5f);
		BoxVertices[4] = BoxVertices[3];
		BoxVertices[5] = _D3DTLVERTEX(_D3DVECTOR(ViewPortPosition.x, ViewPortPosition.y+ViewPortSize.y, 0.999f), 0.001f, 0xFFFFFFFF, 0xFFFFFFFF, 0.5f, 0.5f);
		BoxVertices[6] = BoxVertices[5];
		BoxVertices[7] = BoxVertices[0];
	
		renderSetCurrentMaterial(NULL);
		renderLine((D3DTLVERTEX *)BoxVertices, 8);
#endif
	}
}

void TrackDetailsScreen::Update(struct Instruction *ScreenCommand)
{
	ScreenCommand->Command = QueuedCommand;
	ScreenCommand->Value = QueuedValue;

	Counter++;

	float NewCounter;
	NewCounter = (float)(Counter%(2*CyclePeriod));
	if (NewCounter > CyclePeriod) NewCounter = (float)(2*CyclePeriod - NewCounter);
	NewCounter = NewCounter/(float)CyclePeriod;

	if (DisplayedSubScreen < 4)
		AllSprites->SetColour(WhiteBox, SelectionCycler->CurrentColour(NewCounter));
	else if (DisplayedSubScreen == 4)
	{
		AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.CarPrefs[UsingPlayer].Prefs[CurrentSelection]], SelectionCycler->CurrentColour(NewCounter));
	}

	int OldSelectionY = SelectionY;
	int TargetY = DisplayedSubScreen < 4 ? 90+(DisplayedSubScreen*60) : 90+((DisplayedSubScreen-3)*60);

	if (!Delay)
	{
		if (SlideCounter || FadeOut)
		{
			if (FadeOut) SlideCounter++; else SlideCounter--;
			if (SlideCounter)
			{
				float SlidePosition = (float)sin(((float)SlideCounter / (float)MaxSlideCounter)*PI/2.0f)*90.0f;
/*			
				AllSprites->SetPosition(Panel[0], &Vector(501.0f+SlidePosition, 90.0f,1.0f), &Vector(256.0f, 128.0f, 1.0f));
				AllSprites->SetPosition(Panel[1], &Vector(501.0f+SlidePosition, 218.0f,1.0f), &Vector(256.0f, 128.0f, 1.0f));
				AllSprites->SetPosition(Panel[2], &Vector(501.0f+SlidePosition, 346.0f,1.0f), &Vector(256.0f, 128.0f, 1.0f), NULL, 6);
*/				
				SlidePosition = 1.0f - (float)SlideCounter / (float)MaxSlideCounter;
				
				AllSprites->SetPosition(Panel[0], &Vector(501.0f, 90.0f, 1.0f), &Vector(256.0f,128.0f, 1.0f), &Vector(SlidePosition, 1.0f, 5.3f));
				AllSprites->SetPosition(Panel[1], &Vector(501.0f, 218.0f, 1.0f), &Vector(256.0f,128.0f, 1.0f), &Vector(SlidePosition, 1.0f, 5.3f));
				AllSprites->SetPosition(Panel[2], &Vector(501.0f, 346.0f, 1.0f), &Vector(256.0f,128.0f, 1.0f), &Vector(SlidePosition, 1.0f, 5.3f), 6);

				AllWords->Hide(StageText);
				AllWords->Hide(NameText);
				for (int i=0 ; i<3 ; i++)
				{
					AllWords->Hide(DescriptionTexts[i]);
					AllWords->Hide(DescriptionValues[i]);
				}

			}
			else
			{
				AllSprites->SetPosition(Panel[0], &Vector(501.0f, 90.0f, 1.0f), &Vector(256.0f,128.0f, 1.0f));
				AllSprites->SetPosition(Panel[1], &Vector(501.0f, 218.0f, 1.0f), &Vector(256.0f,128.0f, 1.0f));
				AllSprites->SetPosition(Panel[2], &Vector(501.0f, 346.0f, 1.0f), &Vector(256.0f,128.0f, 1.0f), NULL, 6);

				for (int i=0 ; i<4 ; i++)
					AllSprites->Show(GreyedIcons[i]);
				AllSprites->Show(WhiteBox);
				AllSprites->Show(WhiteIcons[CurrentSelection]);
				AllSprites->Show(SelectionTexts[CurrentSelection]);

				ShowDetails(CurrentSelection);
			}
		}

		float NewPosition;
		if (SlideCounter > 10)
			NewPosition = -120.0f+193.0f*(float)sin((20.0f-(float)(SlideCounter-10.0f))/20.0f*PI/2.0f);
		else
			NewPosition = 73.0f;
		for (int i=0 ; i<4 ; i++)
			AllSprites->SetPosition(GreyedBox[i], &Vector(NewPosition, (float)(90+i*60.0f), 1.0f));
		AllSprites->SetPosition(WhiteBox, &Vector(NewPosition, (float)(TargetY),0.95f));
		for (i=0 ; i<4 ; i++)
		{
			AllSprites->SetPosition(OrangeBox[i], &Vector(NewPosition, 90.0f+i*60.0f, 1.0f));
		}

	}
	else
	{
		if (!--Delay)
		{
			for (int i=0 ; i<4 ; i++)
			{
				AllSprites->SetPosition(OrangeBox[i], &Vector(-100.0f,0.0f,0.95f));
				AllSprites->SetPosition(GreyedBox[i], &Vector(-100.0f, 0.0f, 0.95f));
				AllSprites->Show(OrangeBox[i]);
				AllSprites->Show(GreyedBox[i]);
			}		
		}
	}
	AllSprites->Show(WhiteBox);

	if (TargetY > SelectionY)
	{
		SelectionY+=10;
		AllSprites->SetPosition(WhiteBox, &Vector(73.0f, (float)SelectionY, 1.0f));
	}
	else if (TargetY < SelectionY)
	{
		SelectionY-=10;
		AllSprites->SetPosition(WhiteBox, &Vector(73.0f, (float)SelectionY, 1.0f));
	}
	
	if (   OldSelectionY < TargetY && SelectionY > TargetY
		|| OldSelectionY > TargetY && SelectionY < TargetY)
	{
		SelectionY = TargetY;
		AllSprites->SetPosition(WhiteBox, &Vector(73.0f, (float)SelectionY, 1.0f));
	}
	
	bool ChangeText = false;
	if (!SlideCounter)
	{
		if ((float)((SelectionY-90)%60)/60.0f < 0.5f && SelectionY < OldSelectionY)
			ChangeText = true;
		if ((float)((SelectionY-90)%60)/60.0f > 0.5f && SelectionY > OldSelectionY)
			ChangeText = true;
	}
	
	if (ChangeText)
	{
		int Old, New;
		New = OldSelectionY > SelectionY ? (SelectionY-90+30)/60 : (SelectionY-90+30)/60;
		Old = OldSelectionY > SelectionY ? New+1 : New-1;
		if (Old<0) Old=0;
		AllSprites->Hide(WhiteIcons[Old]);
		AllSprites->Show(WhiteIcons[New]);
		AllSprites->Hide(SelectionTexts[Old]);
		AllSprites->Show(SelectionTexts[New]);
	}

	// Move the map box if necessary.
	if (TargetViewedPosition.x != ViewedPosition.x || TargetViewedPosition.y != ViewedPosition.y)
	{
		ViewedPosition = ViewedPosition + (TargetViewedPosition - ViewedPosition) * ((float)(Counter-CounterStart))/VIEWPORT_SPEED;
	}

	if (DisplayedSubScreen == 2 || DisplayedSubScreen == 5) DrawTrack(); 
	AllSprites->DrawAll();
	AllWords->DrawAll();

	if (SlideCounter > MaxSlideCounter && !Delay)
	{
		ScreenCommand->Command = 'D';
		ScreenCommand->Value = NULL;

		Destroy();
	}
}

int TrackDetailsScreen::FindJunctionNumber(int Spline)
{
	int Result=-1;
	for (int i=0 ; i<TrackSplines->junc[TrackSplines->spline[Spline].start_junc].no_out ; i++)
	{
		if (TrackSplines->junc[TrackSplines->spline[Spline].start_junc].splineout[i] == Spline)
		{
			Result = i;
			i = TrackSplines->juncno;
		}
	}

	if (Result == -1)
		NonFatalError("Warning!! Can't find junction number for this spline!");

	return Result;
}

void TrackDetailsScreen::ChooseSpline(int ControlPacket)
{
	int JunctionID;
	int ThisSpline;

	int OldSelection = ThisSpline = CurrentSelection;
	
	if (ControlPacket & (1 << CON_LEFT))
	{
		JunctionID = FindJunctionNumber(ThisSpline);
		JunctionID--;
		if (JunctionID < 0)
			JunctionID = TrackSplines->junc[TrackSplines->spline[ThisSpline].start_junc].no_out - 1;
		TemporaryRoute[TrackSplines->spline[ThisSpline].start_junc] = JunctionID;
		CurrentSelection = ThisSpline = TrackSplines->junc[TrackSplines->spline[ThisSpline].start_junc].splineout[JunctionID];
	}
	if (ControlPacket & (1 << CON_RIGHT))
	{
		JunctionID = FindJunctionNumber(ThisSpline);
		JunctionID++;
		if (JunctionID >= TrackSplines->junc[TrackSplines->spline[ThisSpline].start_junc].no_out)
			JunctionID = 0;
		TemporaryRoute[TrackSplines->spline[ThisSpline].start_junc] = JunctionID;
		CurrentSelection = ThisSpline = TrackSplines->junc[TrackSplines->spline[ThisSpline].start_junc].splineout[JunctionID];
	}

	if (ControlPacket & (1 << CON_UP))
	{
		do
		{
			if (TrackSplines->spline[ThisSpline].end_junc != -1)
			{
				CurrentSelection = ThisSpline = TrackSplines->junc[TrackSplines->spline[ThisSpline].end_junc].splineout[TemporaryRoute[TrackSplines->spline[ThisSpline].end_junc]];
			}
		} while (TrackSplines->spline[ThisSpline].end_junc != -1 && TrackSplines->junc[TrackSplines->spline[ThisSpline].start_junc].no_out == 1);

		if (TrackSplines->spline[ThisSpline].end_junc == -1)
		{
			for (int i=0 ; i<40 ; i++)
				MainGameInfo.Route[i] = TemporaryRoute[i];
			DisplayedSubScreen = 2;
			HideDetails(5);
			ShowDetails(2);
			CurrentSelection = 2;
		}
	}
	
	if (ControlPacket & (1 << CON_DOWN))
	{
		if (TrackSplines->junc[TrackSplines->spline[ThisSpline].start_junc].splinein[0] != StartingSpline)
		{
			int NewSpline = StartingSpline;
			int TestedSpline[40];

			int SplineSequence = 0, PreviousSpline;
			
			while (NewSpline != -1)
			{
				TestedSpline[SplineSequence++] = NewSpline;

				if (TrackSplines->spline[NewSpline].end_junc == TrackSplines->spline[ThisSpline].start_junc)
				{
					PreviousSpline = CurrentSelection = NewSpline;
					NewSpline = -1;

					SplineSequence--;
					// Find the last spline with a choice...
					while (TrackSplines->junc[TrackSplines->spline[CurrentSelection = TestedSpline[--SplineSequence]].end_junc].no_out == 1)
						PreviousSpline = CurrentSelection;
			
					CurrentSelection = PreviousSpline;
				}
				else
				{
					if (TrackSplines->spline[NewSpline].end_junc == -1)
						NewSpline = -1;
					else
						NewSpline = TrackSplines->junc[TrackSplines->spline[NewSpline].end_junc].splineout[TemporaryRoute[TrackSplines->spline[NewSpline].end_junc]];
				}
			}
		}
		else
		{
			for (int i=0 ; i<40 ; i++)
				MainGameInfo.Route[i] = TemporaryRoute[i];
			DisplayedSubScreen = 2;
			HideDetails(5);
			ShowDetails(2);
			CurrentSelection = 2;
		}
	}
	
	if (CurrentSelection != OldSelection)
	{
		ColourRoute();
		
		if (DisplayedSubScreen == 5)
		{
			SplineColours[ThisSpline = CurrentSelection] = 0xFFFF0000;
			while (TrackSplines->spline[ThisSpline].end_junc != -1 && TrackSplines->junc[TrackSplines->spline[ThisSpline].end_junc].no_out == 1)
			{
				SplineColours[TrackSplines->junc[TrackSplines->spline[ThisSpline].end_junc].splineout[0]] = 0xFFFF0000;
				ThisSpline = TrackSplines->junc[TrackSplines->spline[ThisSpline].end_junc].splineout[0];
			}
			ThisSpline = CurrentSelection;
			
			TargetViewedPosition.x = (TrackSplines->junc[TrackSplines->spline[ThisSpline].start_junc].pos.x  - MAP_STARTX)*MapScale.x + MapOffset.x - ViewedSize.x * 0.5f;
			TargetViewedPosition.y = MapOffset.y - (TrackSplines->junc[TrackSplines->spline[ThisSpline].start_junc].pos.y  - MAP_STARTY)*MapScale.y - ViewedSize.y * 0.5f;
			CounterStart = Counter;
		}
	}
}

int TrackDetailsScreen::ControlPressed(int ControlPacket)
{
	int i;
	if (ControlPacket & (1 << CON_A))
	{
		switch (DisplayedSubScreen)
		{
		case 0:
			break;
		case 1:
			for (i=0 ; i<4 ; i++)
				OldPrefs[i] = MainGameInfo.CarPrefs[UsingPlayer].Prefs[i];
				
			DisplayedSubScreen = 4;
			HideDetails(1);
			ShowDetails(4);
			CurrentSelection = 0;
			break;
		case 2:
			DisplayedSubScreen = 5;
			HideDetails(2);
			ShowDetails(5);
			break;
		case 3:
			QueuedCommand = 'S';
			QueuedValue = (Screen *)new LoadingScreen(NULL);
			((LoadingScreen *)QueuedValue)->SetDelay(30);
			FadeOut = true;
			for (i=0 ; i<4 ; i++)
			{
				AllSprites->Hide(GreyedIcons[i]);
				AllSprites->Hide(WhiteIcons[i]);
			}
			HideDetails(DisplayedSubScreen);
			break;
		case 4:
			AllWords->SetColour(Headers[CurrentSelection], 0xFFA0A0A0);
			AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.CarPrefs[UsingPlayer].Prefs[CurrentSelection]], 0xFFFFFFFF);
			DisplayedSubScreen = 1;
			HideDetails(4);
			ShowDetails(1);
			CurrentSelection = 1;
			break;
		case 5:
			for (i=0 ; i<40 ; i++)
				MainGameInfo.Route[i] = TemporaryRoute[i];
			DisplayedSubScreen = 2;
			HideDetails(5);
			ShowDetails(2);
			CurrentSelection = 2;
			break;
		}
	}
	if (ControlPacket & (1 << CON_Y))
	{
		switch (DisplayedSubScreen)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			QueuedCommand = 'S';
			QueuedValue = (Screen *)new ChooseClassScreen(NULL);
			((ChooseClassScreen *)QueuedValue)->SetDelay(10);
			FadeOut = true;
			for (i=0 ; i<4 ; i++)
			{
				AllSprites->Hide(GreyedIcons[i]);
				AllSprites->Hide(WhiteIcons[i]);
			}
			HideDetails(DisplayedSubScreen);
			break;
		case 4:
			for (i=0 ; i<4 ; i++)
			{
				MainGameInfo.CarPrefs[UsingPlayer].Prefs[i] = OldPrefs[i];
				for (int j=0 ; j<3 ; j++)
					AllWords->SetColour(Values[i*3+j], MainGameInfo.CarPrefs[UsingPlayer].Prefs[i] == j ? 0xFFFFFFFF : 0xFFA0A0A0);
			}
			AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.CarPrefs[UsingPlayer].Prefs[CurrentSelection]], 0xFFFFFFFF);
			for (i=0 ; i<4 ; i++)
				MainGameInfo.CarPrefs[UsingPlayer].Prefs[i] = OldPrefs[i];
			
			AllWords->SetColour(Headers[CurrentSelection], 0xFFA0A0A0);
			DisplayedSubScreen = 1;
			HideDetails(4);
			ShowDetails(1);
			CurrentSelection = 1;
			break;
		case 5:
			HideDetails(5);
			ShowDetails(2);
			CurrentSelection = DisplayedSubScreen = 2;
			ColourRoute();
			break;
		}
	}

	int PreviousSelection = CurrentSelection;
	
	switch (DisplayedSubScreen)
	{
	case 5:
		ChooseSpline(ControlPacket);
		break;
	default:
		if (ControlPacket & (1 << CON_UP))
			if (CurrentSelection) 
			{
				if (DisplayedSubScreen < 4)
				{
					HideDetails(CurrentSelection);
					CurrentSelection--;
					ShowDetails(CurrentSelection);
				}
				else
					CurrentSelection--;
			}
		if (ControlPacket & (1 << CON_DOWN))
			if (CurrentSelection < 3) 
			{
				if (DisplayedSubScreen < 4)
				{
					HideDetails(CurrentSelection);
					CurrentSelection++;
					ShowDetails(CurrentSelection);
				}
				else
					CurrentSelection++;
			}
				
		if (ControlPacket & (1 << CON_LEFT))
			if (DisplayedSubScreen == 4)
			{
				AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.CarPrefs[UsingPlayer].Prefs[CurrentSelection]], 0xFFA0A0A0);
				if (MainGameInfo.CarPrefs[UsingPlayer].Prefs[CurrentSelection]) MainGameInfo.CarPrefs[UsingPlayer].Prefs[CurrentSelection]--;
			}
					
		if (ControlPacket & (1 << CON_RIGHT))
			if (DisplayedSubScreen == 4)
			{
				AllWords->SetColour(Values[CurrentSelection*3+MainGameInfo.CarPrefs[UsingPlayer].Prefs[CurrentSelection]], 0xFFA0A0A0);
				if (MainGameInfo.CarPrefs[UsingPlayer].Prefs[CurrentSelection] < 2) MainGameInfo.CarPrefs[UsingPlayer].Prefs[CurrentSelection]++;
			}
		break;
	}
	
	if (DisplayedSubScreen < 4)
		DisplayedSubScreen = CurrentSelection;
	else if (DisplayedSubScreen == 4)
	{
		if (PreviousSelection != CurrentSelection)
		{
			AllWords->SetColour(Values[PreviousSelection*3+MainGameInfo.CarPrefs[UsingPlayer].Prefs[PreviousSelection]], 0xFFFFFFFF);
			AllWords->SetColour(Headers[PreviousSelection], 0xFFA0A0A0);
			AllWords->SetColour(Headers[CurrentSelection], 0xFFFFFFFF);
		}
	}


	return 0;
}

void TrackDetailsScreen::ShowDetails(int SubScreen)
{
	int i;

	switch (SubScreen)
	{
	case 0:
		AllWords->Show(StageText);
		AllWords->Show(NameText);
		for (i=0 ; i<3 ; i++)
		{
			AllWords->Show(DescriptionTexts[i]);
			AllWords->Show(DescriptionValues[i]);
			AllSprites->Show(Panel[i]);
		}
		break;
	case 2:
//		AllSprites->Show(MapMockup[0]);
//		AllSprites->Show(MapMockup[1]);
		for (i=0 ; i<4 ; i++)
			GlobalSprites->Show(Grid[i]);
		GlobalSprites->Show(PSButtons[3]);
		AllWords->Show(EditText);
		break;
	case 1:
		// NO break;
		AllWords->Show(EditText);
	case 4:
		for (i=0 ; i<8 ; i++)
			GlobalSprites->Show(LocalHorizLines[i]);
		for (i=0 ; i<4 ; i++)
			GlobalSprites->Show(LocalVertLines[i]);
		for (i=0 ; i<12 ; i++)
			AllWords->Show(Values[i]);
		for (i=0 ; i<4 ; i++)
			AllWords->Show(Headers[i]);
		for (i=0 ; i<2 ; i++)
			AllWords->Show(Titles[i]);
	
		GlobalSprites->Show(PSButtons[3]);
		if (SubScreen == 4)
		{
			GlobalSprites->Show(PSButtons[1]);
			AllWords->SetColour(Headers[0], 0xFFFFFFFF);
			AllWords->Show(DoneText);
			AllWords->Show(CancelText);
		}
		GlobalSprites->Show(LocalRedShades[0]);
		GlobalSprites->Show(LocalRedShades[1]);
		AllSprites->SetColour(WhiteBox, 0xFFFFFFFF);

		break;
	case 5:
		GlobalSprites->Show(WhiteArrows[ARROW_LEFT]);
		GlobalSprites->Show(WhiteArrows[ARROW_RIGHT]);
		GlobalSprites->SetPosition(PSButtons[BUTTON_SQUARE], &Vector(229.0f, 123.0f, 1.0f), &Vector(64.0f, 64.0f, 64.0f));
		GlobalSprites->SetPosition(PSButtons[BUTTON_TRIANGLE], &Vector(229.0f, 153.0f, 1.0f), &Vector(64.0f, 64.0f, 64.0f));
		GlobalSprites->SetPosition(PSButtons[BUTTON_CROSS], &Vector(436.0f, 403.0f, 1.0f), &Vector(64.0f, 64.0f, 64.0f));
		AllWords->Show(RouteWord);
		AllWords->Show(NextWord);
		AllWords->Show(BackWord);
		AllWords->Show(CancelWord);
		for (i=0 ; i<4 ; i++)
			GlobalSprites->Show(PSButtons[i]);
		AllWords->Show(DoneText);

		for (i=0 ; i<4 ; i++)
			GlobalSprites->Show(Grid[i]);
		for (i=0 ; i<40 ; i++)
			TemporaryRoute[i] = MainGameInfo.Route[i];
		// Set the first spline to "confirmed colour".
		SplineColours[StartingSpline] = 0xFFFFFF80;
		// Set the next confirmed spline to red.
		SplineColours[TrackSplines->junc[TrackSplines->spline[StartingSpline].end_junc].splineout[TemporaryRoute[TrackSplines->spline[StartingSpline].end_junc]]] = 0xFFFF0000;

		CurrentSelection = TrackSplines->junc[TrackSplines->spline[StartingSpline].end_junc].splineout[TemporaryRoute[TrackSplines->spline[StartingSpline].end_junc]];
		while (TrackSplines->junc[TrackSplines->spline[CurrentSelection].end_junc].no_out == 1)
		{
			SplineColours[TrackSplines->junc[TrackSplines->spline[CurrentSelection].end_junc].splineout[0]] = 0xFFFF0000;
			CurrentSelection = TrackSplines->junc[TrackSplines->spline[CurrentSelection].end_junc].splineout[0];
		}

		CurrentSelection = TrackSplines->junc[TrackSplines->spline[StartingSpline].end_junc].splineout[TemporaryRoute[TrackSplines->spline[StartingSpline].end_junc]];

		TargetViewedPosition.x = ViewedPosition.x = (TrackSplines->junc[TrackSplines->spline[StartingSpline].end_junc].pos.x  - MAP_STARTX)*MapScale.x + MapOffset.x - ViewedSize.x * 0.5f;
		TargetViewedPosition.y = ViewedPosition.y = MapOffset.y - (TrackSplines->junc[TrackSplines->spline[StartingSpline].end_junc].pos.y  - MAP_STARTY)*MapScale.y - ViewedSize.y * 0.5f;
		AllSprites->SetColour(WhiteBox, 0xFFFFFFFF);
		break;
	default:
		break;
	}

}

void TrackDetailsScreen::HideDetails(int SubScreen)
{
	int i;
	switch (SubScreen)
	{
	case 0:
		AllWords->Hide(StageText);
		AllWords->Hide(NameText);
		for (i=0 ; i<3 ; i++)
		{
			AllWords->Hide(DescriptionTexts[i]);
			AllWords->Hide(DescriptionValues[i]);
			AllSprites->Hide(Panel[i]);
		}
		break;
	case 2:
//		AllSprites->Hide(MapMockup[0]);
//		AllSprites->Hide(MapMockup[1]);
		for (i=0 ; i<4 ; i++)
			GlobalSprites->Hide(Grid[i]);
		GlobalSprites->Hide(PSButtons[3]);
		AllWords->Hide(EditText);
		break;
	case 1:
		GlobalSprites->Hide(PSButtons[3]);
		AllWords->Hide(EditText);
		// NO break;
	case 4:
		for (i=0 ; i<8 ; i++)
			GlobalSprites->Hide(LocalHorizLines[i]);
		for (i=0 ; i<4 ; i++)
			GlobalSprites->Hide(LocalVertLines[i]);
		for (i=0 ; i<12 ; i++)
			AllWords->Hide(Values[i]);
		for (i=0 ; i<4 ; i++)
			AllWords->Hide(Headers[i]);
		for (i=0 ; i<2 ; i++)
			AllWords->Hide(Titles[i]);

		GlobalSprites->Hide(PSButtons[1]);
		AllWords->Hide(DoneText);
		AllWords->Hide(CancelText);
		GlobalSprites->Hide(LocalRedShades[0]);
		GlobalSprites->Hide(LocalRedShades[1]);
		break;
	case 5:
		GlobalSprites->Hide(WhiteArrows[ARROW_LEFT]);
		GlobalSprites->Hide(WhiteArrows[ARROW_RIGHT]);
		AllWords->Hide(RouteWord);
		AllWords->Hide(NextWord);
		AllWords->Hide(BackWord);
		AllWords->Hide(CancelWord);
		for (i=0 ; i<4 ; i++)
			GlobalSprites->Hide(PSButtons[i]);
		AllWords->Hide(DoneText);
		ColourRoute();
		break;
	default:
		break;
	}
}

void TrackDetailsScreen::DrawBackground()
{
}

void TrackDetailsScreen::Destroy()
{
	GlobalSprites->Delete(LocalRedShades[0]);
	GlobalSprites->Delete(LocalRedShades[1]);

	for (int i=0 ; i<8 ; i++)
		GlobalSprites->Delete(LocalHorizLines[i]);
	for (i=0 ; i<4 ; i++)
		GlobalSprites->Delete(LocalVertLines[i]);

	free(SplineColours);
	delete AllSprites;
	delete AllWords;
	delete TrackSplines;
}
