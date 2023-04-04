// $Header$

// $Log$
// Revision 1.4  2000-09-25 16:05:06+01  img
// Snapshot when converting to Igor
//
// Revision 1.3  2000-09-13 12:04:08+01  img
// TrackDetails mostly working...
//
// Revision 1.2  2000-08-19 17:08:05+01  img
// Front end mostly complete (old car config screen)
//
// Revision 1.1  2000-08-07 16:20:02+01  img
// Adding options screens...
//
// Revision 1.0  2000-07-03 16:21:04+01  img
// Initial revision
//
// Revision 1.59  2000-05-26 09:12:33+01  img
// Gotta be final version now...
//
// Revision 1.58  2000-05-16 16:13:24+01  img
// FINAL release build???
//
// Revision 1.57  2000-05-11 14:17:04+01  img
// Smaller start
//
// Revision 1.56  2000-05-11 10:42:48+01  img
// Static "Device removed" message when no controllers left
//
// Revision 1.55  2000-05-09 15:50:53+01  jcf
// FrontEndObject ctors now correct in LoadControllers
//
// Revision 1.54  2000-05-09 12:10:31+01  jcf
// Vibration pack detection in LoadDefaultSettings
//
// Revision 1.53  2000-05-08 18:20:32+01  jjs
// Fixes memory leaks.
//
// Revision 1.52  2000-05-05 18:13:01+01  img
// Build 19
//
// Revision 1.51  2000-05-05 12:17:20+01  img
// Build 19
//
// Revision 1.50  2000-05-04 16:25:30+01  jcf
// Cacheing used for some archives. language archives push/popped in LoadAllCars
//
// Revision 1.49  2000-05-04 11:58:59+01  img
// American Language
//
// Revision 1.48  2000-04-28 11:09:10+01  jcf
// <>
//
// Revision 1.47  2000-04-26 10:08:53+01  img
// <>
//
// Revision 1.46  2000-04-25 11:50:04+01  img
// Bug fixes
//
// Revision 1.45  2000-04-24 20:27:36+01  img
// Bug fixes
//
// Revision 1.44  2000-04-22 18:26:10+01  img
// Added C button to joystick support
//
// Revision 1.43  2000-04-21 19:43:46+01  img
// DestroyScreen() added
//
// Revision 1.42  2000-04-21 12:59:30+01  img
// bug fiz
//
// Revision 1.41  2000-04-20 20:19:57+01  img
// better memory handling
//
// Revision 1.40  2000-04-20 16:23:24+01  img
// Removed L_* #define conflict
//
// Revision 1.39  2000-04-20 15:17:50+01  img
// Memory leak fixed
//
// Revision 1.38  2000-04-20 13:31:22+01  img
// More memory fixes
//
// Revision 1.37  2000-04-20 12:56:47+01  img
// Memory leaks fixed
//
// Revision 1.36  2000-04-19 21:40:49+01  img
// fixes
//
// Revision 1.35  2000-04-19 21:38:06+01  img
// Localisation.
//
// Revision 1.34  2000-04-17 14:53:01+01  img
// Bug fixes
//
// Revision 1.33  2000-04-17 10:11:13+01  jcf
// Progress bars
//
// Revision 1.32  2000-04-17 10:03:21+01  img
// Bug fix
//
// Revision 1.31  2000-04-12 18:11:27+01  img
// bugfixes
//
// Revision 1.30  2000-04-12 12:20:25+01  img
// Force dreampad
//
// Revision 1.29  2000-04-11 16:13:53+01  img
// REP_QUICKRACEOVER implemented
//
// Revision 1.28  2000-04-10 17:20:46+01  img
// Preloads all controllers
//
// Revision 1.27  2000-04-07 17:32:12+01  img
// Force dreampad
//
// Revision 1.26  2000-04-07 15:58:27+01  img
// Controllers loaded here now.  LoadDefaultSettings added.
//
// Revision 1.25  2000-04-07 15:29:13+01  img
// Better management of frontscreen's memory card
//
// Revision 1.24  2000-04-07 14:38:17+01  img
// Help support, generic background support
//
// Revision 1.23  2000-04-05 17:46:54+01  img
// More preloadings
//
// Revision 1.22  2000-04-04 18:39:46+01  img
// Debugging version
//
// Revision 1.21  2000-04-03 14:54:05+01  img
// Preloadings, and no PlayerScreen
//
// Revision 1.20  2000-03-30 14:48:28+01  img
// More preloadings
//
// Revision 1.19  2000-03-28 17:57:01+01  img
// Preloading grid models and bug fixes.
//
// Revision 1.18  2000-03-27 10:26:33+01  jjs
// Beta build and removed renderReset().
//
// Revision 1.17  2000-03-22 12:47:12+00  jjs
// <>
//
// Revision 1.16  2000-03-22 09:59:46+00  jjs
// <>
//
// Revision 1.15  2000-03-22 09:29:09+00  img
// <>
//
// Revision 1.14  2000-03-21 17:28:29+00  img
// Pre-loading more now...
//
// Revision 1.13  2000-03-20 18:01:01+00  img
// Beta build
//
// Revision 1.12  2000-03-09 10:33:01+00  img
// Use 512x512 texture again
//
// Revision 1.11  2000-03-08 10:32:16+00  img
// DefaultAlphabet uses Courier. Also uses 3df backdrops
//
// Revision 1.10  2000-03-07 16:02:55+00  img
// Use re-entry point
//
// Revision 1.9  2000-03-07 14:23:41+00  img
// ReEntryPoint now works
//
// Revision 1.8  2000-03-07 14:02:34+00  img
// Uses pvr files
//
// Revision 1.7  2000-03-06 13:03:17+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.6  2000-02-29 11:28:29+00  jjs
// Added Header and Log lines.
//

/* First screen of the front end
	SOS_Index.cpp

	29/11/1999 */

#include "ConsoleFrontLib.h"

#include "SmallFont.h"
#include "MiddleFont.h"
#include "LargeFont.h"

TwoDFont *SmallFont, *MiddleFont, *LargeFont;
SpriteList *GlobalSprites;
int FontBox[3], RaceModeTexts[3];
int CurveSections[3], Line, Banner[3], Grid[4], VerticalLines[7], HorizontalLines[7], RedShade, OrangeSquare, EmptySquare;
int PSButtons[4], GreyArrows[4], WhiteArrows[4], HalfRedShade;
int UsingPlayer;

Game::Game(int ReEntryPoint)
{
	// Define the first player by default.
	UsingPlayer = 0;

	CreateDefaultValues();

	GlobalSprites = new SpriteList();

#if defined(IGOR)
	FontArchive = NewArchive();
	FontArchive->Open(FRONTEND_DATA "Fonts1.tc");
#else
	arcPush(".\\FrontLibData\\Fonts1.tc");
#endif
//	SmallFont = new TwoDFont("Fonts", (RECT *)SmallFontDimensions, "Fonts2");
//	MiddleFont = new TwoDFont("Fonts2", (RECT *)MiddleFontDimensions);
//	LargeFont = new TwoDFont("Fonts", (RECT *)LargeFontDimensions);
	
	Render2D ThisMaterial;
	GlobalSprites->LoadTexture(&ThisMaterial, NULL, "Fonts");//, GameArchive);

	FontBox[0] = GlobalSprites->Add(&ThisMaterial, 2,81,15,18);
	FontBox[1] = GlobalSprites->Add(&ThisMaterial, 2,81,15,18);
	FontBox[2] = GlobalSprites->Add(&ThisMaterial, 2,81,15,18);
#if !defined(IGOR)
	arcPop();
#endif	
	dprintf("Creating new MainScreen...\n");
	CurrentScreen = (Screen *)new MainScreen(NULL);
	dprintf("CurrentScreen early in constructor is 0x%x.\n", CurrentScreen);

	FadeOutScreen = NULL;

	MainGameInfo.RaceMode = 0;
	MainGameInfo.VehicleType = 1;
	MainGameInfo.LiveryID = 1;
	MainGameInfo.ColourID = 1;

#if defined(IGOR)
	GlobalIconsArchive = NewArchive();
	GlobalIconsArchive->Open(FRONTEND_DATA "GlobalIcons.tc");
#else
	arcPush(".\\FrontLibData\\ChooseClassScreen.tc");
#endif
	GlobalSprites->LoadTexture(&ThisMaterial, NULL, "Curve_a");//, GlobalIconsArchive);

	// Curve is at 125,302 -> 141,430 
	// Bitmap 0,0 -> 15,127
	CurveSections[0] = GlobalSprites->Add(&ThisMaterial, 0,0,15,127);
	GlobalSprites->SetPosition(CurveSections[0], &Vector(125.0f,302.0f,1.0f), &Vector(128.0f,128.0f,1.0f));
	// Then 131,174 -> 159,301
	// Bitmap 16,0 -> 44,127
	CurveSections[1] = GlobalSprites->Add(&ThisMaterial, 16,0,29,127);
	GlobalSprites->SetPosition(CurveSections[1], &Vector(137.0f,174.0f,1.0f), &Vector(128.0f,128.0f,1.0f));
	// Then 155,45 -> 199,173
	// Bitmap 45,0 -> 99,127
	CurveSections[2] = GlobalSprites->Add(&ThisMaterial, 45,0,54,127);
	GlobalSprites->SetPosition(CurveSections[2], &Vector(162.0f,46.0f,1.0f), &Vector(128.0f,128.0f,1.0f));

	GlobalSprites->LoadTexture(&ThisMaterial, NULL, "Text_a");//, GlobalIconsArchive);
	Line = GlobalSprites->Add(&ThisMaterial, 0,238, 16,3);
	GlobalSprites->SetPosition(Line, &Vector(320.0f,77.0f,1.0f), NULL, &Vector(41.0f,1.0f,5.3f));

	GlobalSprites->LoadTexture(&ThisMaterial, NULL, "DakarBanner_a");//, GlobalIconsArchive);
	Banner[0] = GlobalSprites->Add(&ThisMaterial, 8,0,75,125);
	GlobalSprites->SetPosition(Banner[0], &Vector(52.0f,302.0f,1.0f), &Vector(256.0f, 128.0f, 1.0f));
	Banner[1] = GlobalSprites->Add(&ThisMaterial, 87,0,75,128);
	GlobalSprites->SetPosition(Banner[1], &Vector(52.0f,174.0f,1.0f), &Vector(256.0f, 128.0f, 1.0f));
	Banner[2] = GlobalSprites->Add(&ThisMaterial, 166,0,75,128);
	GlobalSprites->SetPosition(Banner[2], &Vector(52.0f,46.0f,1.0f), &Vector(256.0f, 128.0f, 1.0f));

	GlobalSprites->LoadTexture(&ThisMaterial, NULL, "Grid_a4444");//, GlobalIconsArchive);
	Grid[0] = GlobalSprites->Add(&ThisMaterial, 0,136,256,120);
	GlobalSprites->SetPosition(Grid[0], &Vector(128.0f,55.0f, 1.0f));
	GlobalSprites->LoadTexture(&ThisMaterial, NULL, "Grid_b4444");//, GlobalIconsArchive);
	Grid[1] = GlobalSprites->Add(&ThisMaterial, 0,136,256,120);
	GlobalSprites->SetPosition(Grid[1], &Vector(384.0f,55.0f, 1.0f));
	GlobalSprites->LoadTexture(&ThisMaterial, NULL, "Grid_c4444");//, GlobalIconsArchive);
	Grid[2] = GlobalSprites->Add(&ThisMaterial, 0,0,256,256);
	GlobalSprites->SetPosition(Grid[2], &Vector(128.0f, 175.0f, 1.0f));
	GlobalSprites->LoadTexture(&ThisMaterial, NULL, "Grid_d4444");//, GlobalIconsArchive);
	Grid[3] = GlobalSprites->Add(&ThisMaterial, 0,0,256,256);
	GlobalSprites->SetPosition(Grid[3], &Vector(384.0f, 175.0f, 1.0f));

	GlobalSprites->LoadTexture(&ThisMaterial, NULL, "Text_a");//, GlobalIconsArchive);
	RaceModeTexts[0] = GlobalSprites->Add(&ThisMaterial, 0,47,256,26);
	RaceModeTexts[1] = GlobalSprites->Add(&ThisMaterial, 0,133,256,26);
	RaceModeTexts[2] = GlobalSprites->Add(&ThisMaterial, 0,4,256,34);

#if defined(IGOR)
#else
	arcPop();
	arcPush(".\\FrontLibData\\GlobalIcons.tc");
#endif

	GlobalSprites->LoadTexture(&ThisMaterial, NULL, "PSButtons");//, GlobalIconsArchive);
	PSButtons[0] = GlobalSprites->Add(&ThisMaterial, 0,0,27,27);
	PSButtons[1] = GlobalSprites->Add(&ThisMaterial, 27,0,27,27);
	PSButtons[2] = GlobalSprites->Add(&ThisMaterial, 0,27,27,27);
	PSButtons[3] = GlobalSprites->Add(&ThisMaterial, 27,27,27,27);

	GlobalSprites->LoadTexture(&ThisMaterial, NULL, "Arrows");//, GlobalIconsArchive);
	GreyArrows[0] = GlobalSprites->Add(&ThisMaterial, 14,10, 9,10);
	GreyArrows[1] = GlobalSprites->Add(&ThisMaterial, 27,10, 8,10);
	GreyArrows[2] = GlobalSprites->Add(&ThisMaterial, 41, 4, 10,9);
	GreyArrows[3] = GlobalSprites->Add(&ThisMaterial, 41,17, 10,9);
	WhiteArrows[0] = GlobalSprites->Add(&ThisMaterial, 14,40, 9,10);
	WhiteArrows[1] = GlobalSprites->Add(&ThisMaterial, 27,40, 8,10);
	WhiteArrows[2] = GlobalSprites->Add(&ThisMaterial, 41,34, 10,9);
	WhiteArrows[3] = GlobalSprites->Add(&ThisMaterial, 41,47, 10,9);

	GlobalSprites->LoadTexture(&ThisMaterial, NULL, "HorizLinesNIcon");//, GlobalIconsArchive);
	HorizontalLines[0] = GlobalSprites->Add(&ThisMaterial, 0,2,255,3);
	HorizontalLines[1] = GlobalSprites->Add(&ThisMaterial, 0,8,254,3);
	HorizontalLines[2] = GlobalSprites->Add(&ThisMaterial, 0,14,190,3);
	HorizontalLines[3] = GlobalSprites->Add(&ThisMaterial, 0,20,122,3);
	HorizontalLines[4] = GlobalSprites->Add(&ThisMaterial, 0,26,70,3);
	HorizontalLines[5] = GlobalSprites->Add(&ThisMaterial, 0,32,32,3);
	HorizontalLines[6] = GlobalSprites->Add(&ThisMaterial, 0,38,15,3);

	OrangeSquare = GlobalSprites->Add(&ThisMaterial, 214,17,9,15);
	EmptySquare = GlobalSprites->Add(&ThisMaterial, 224,17,9,15);	

	GlobalSprites->LoadTexture(&ThisMaterial, NULL, "VertLines");//, GlobalIconsArchive);
	VerticalLines[0] = GlobalSprites->Add(&ThisMaterial, 23,0, 3,15);
	VerticalLines[1] = GlobalSprites->Add(&ThisMaterial, 29,0, 3,32);
	VerticalLines[2] = GlobalSprites->Add(&ThisMaterial, 35,0, 3,70);
	VerticalLines[3] = GlobalSprites->Add(&ThisMaterial, 41,0, 3,122);
	VerticalLines[4] = GlobalSprites->Add(&ThisMaterial, 47,0, 3,190);
	VerticalLines[5] = GlobalSprites->Add(&ThisMaterial, 53,0, 3,234);
	VerticalLines[6] = GlobalSprites->Add(&ThisMaterial, 59,0, 3,256);

	GlobalSprites->LoadTexture(&ThisMaterial, NULL, "RedGradient");//, GlobalIconsArchive);
	RedShade = GlobalSprites->Add(&ThisMaterial, 0,0,256,256);
	HalfRedShade = GlobalSprites->Add(&ThisMaterial, 0,128,256,128);

#if defined(IGOR)
//	GameArchive->Close();
#else
	arcPop();
#endif

	GlobalSprites->Hide(Line);
	for (int i=0 ; i<3 ; i++) GlobalSprites->Hide(Banner[i]);
	for (i=0 ; i<3 ; i++) GlobalSprites->Hide(CurveSections[i]);
	for (i=0 ; i<4 ; i++) GlobalSprites->Hide(Grid[i]);

	LoadBackdrop(DefaultBackdrop, "MainBackground", true, Bars, "MB_LetterBox", BarFill);
	dprintf("Using bars at 0x%x and 0x%x.", &(BarFill[0]), &(BarFill[1]));

	dprintf("Current screen in constructor is 0x%lx.\n", CurrentScreen);
}

void Game::CreateDefaultValues()
{
	for (int i=0 ; i<4 ; i++)
	{
		MainGameInfo.CarPrefs[i].Prefs[CPS_STIFFNESS] = 1;
		MainGameInfo.CarPrefs[i].Prefs[CPS_SHOCKS] = 2;
		MainGameInfo.CarPrefs[i].Prefs[CPS_RIDEHEIGHT] = 0;
		MainGameInfo.CarPrefs[i].Prefs[CPS_TYREPRESSURE] = 1;
	}

	MainGameInfo.SplitScreen = 0;
	MainGameInfo.SpeedUnits = 0;
	MainGameInfo.AspectRatio = 0;
	MainGameInfo.ScreenOffsetX = 0;
	MainGameInfo.ScreenOffsetY = 0;
	MainGameInfo.ControllerConfig = 0;
	MainGameInfo.Vibration = 0;

	MainGameInfo.MusicVolume = 5;
	MainGameInfo.SFXVolume = 5;
	MainGameInfo.SpeechVolume = 5;
	MainGameInfo.SoundOutput= 0;
	MainGameInfo.Surround = 0;

	MainGameInfo.Route[0] = 255;
}

int Game::ControlPressed(int ControlPacket)
{
	if (CurrentScreen)
		return CurrentScreen->ControlPressed(ControlPacket);
	else
		return 0;
}

void Game::Update()
{
	static struct Instruction ScreenCommand;
	static struct Instruction FadeOutCommand;

	// Default to no instruction.
	ScreenCommand.Command = 0;
	FadeOutCommand.Command = 0;

	GlobalSprites->DrawAll();
	if (CurrentScreen)
		CurrentScreen->Update(&ScreenCommand);
	if (FadeOutScreen)
		FadeOutScreen->Update(&FadeOutCommand);

	// Parse the result of the fade-out.
	switch (FadeOutCommand.Command)
	{
	case 'D':
		if (FadeOutScreen)
			delete FadeOutScreen;
		FadeOutScreen = NULL;
		break;
	default:
		break;
	}

	// Parse the command.
	switch (ScreenCommand.Command)
	{
	case 'D':
		NonFatalError("Delete screen called on current screen!!");
		break;
	case 'S':
		if (FadeOutScreen)
		{
			FadeOutScreen->Destroy();
			delete FadeOutScreen;
		}
		FadeOutScreen = CurrentScreen;
		CurrentScreen = (Screen *)ScreenCommand.Value;

		dprintf("screen changed to %lx",CurrentScreen);
		ForceNoCatchUp = true;
		break;
	
	default:
		break;
	}
}

void Game::DrawBackground()
{
	if(!CurrentScreen)
	{
		dprintf("no current screen!");
		return;
	}
	if (CurrentScreen->UseDefaultBackdrop)
	{
		CurrentScreen->DrawFullScreen2D((DWORD *)DefaultBackdrop, true, (DWORD *)Bars, (DWORD *)BarFill);
	}
	else
	{
		dprintf("CurrentScreen %lx",CurrentScreen);
		CurrentScreen->DrawBackground();
	}
}

void Game::DestroyScreen()
{
	CurrentScreen->Destroy();
}

void Game::Destroy()
{
	FontArchive->Close();			delete FontArchive;
	GlobalIconsArchive->Close();	delete GlobalIconsArchive;

	delete SmallFont;
	delete FadeOutScreen;
	delete CurrentScreen;

}
