// $Header$

// $Log$
// Revision 1.11  2000-09-13 12:04:11+01  img
// TrackDetails mostly working...
//
// Revision 1.10  2000-08-07 16:20:07+01  img
// Adding options screens...
//
// Revision 1.9  2000-05-09 17:15:52+01  jjs
// Allows debugmem when using frontend.
//
// Revision 1.8  2000-05-04 16:24:59+01  jcf
// Only 2 static screens now.
//
// Revision 1.7  2000-04-28 11:09:19+01  jcf
// <>
//
// Revision 1.6  2000-04-07 14:37:39+01  img
// Help support.
//
// Revision 1.5  2000-04-03 14:51:40+01  img
// No change. Wouldn`t let me undo!
//
// Revision 1.4  2000-03-22 09:29:07+00  img
// <>
//
// Revision 1.3  2000-03-21 17:28:21+00  img
// Pre-loading more now...
//
// Revision 1.2  2000-02-29 08:44:46+00  jjs
// Added Header and Log lines
//

/* A Screen entity holds the data for each er- screen.

   1/12/1999 Broadsword Interactive Ltd.

   Ian Gledhill */

#define FRONTSCREEN 0
#define CHAMPIONSHIP 1

#include "render2D.h"

typedef BOOL (FAR PASCAL * LPCALLBACK)(void);

void LoadBackdrop(Render2D *BackdropVariable, char *BackdropName, bool MoreTextures=false, Render2D *Bars=NULL, char *BarName=NULL, Render2D *BarFill=NULL);

/*
 The Update() function fills in the command structure.
 These commands are:
	
	  Command	 Instruction		Value type
		'S'		 Change Screen		Screen *
		'D'		 Destroy Screen		NULL
*/

extern int Counter;

extern char *static_screens[];
extern int static_screen_ct;

class Screen
{
public:
	Screen();
	virtual ~Screen() = 0;

	virtual void Update(struct Instruction *ScreenCommand) = 0;
	virtual int ControlPressed(int ControlPacket) = 0;

	virtual void DrawBackground() = 0;
	
	virtual void Destroy() = 0;

	virtual void AddButtonBank(ButtonBank *) {};

	virtual void SetHighlighted(int _Highlighted) {};

	void DrawFullScreen2D(DWORD _Background[], bool MoreTextures=false, DWORD Bars[]=NULL, DWORD BarFill[]=NULL);
	void SetBarsHeight(int TopHeight, int BottomHeight);

	void SetCounter(int NewCounter);

	void SetStartState(bool State);
	void SetHelpText(char *Text1, char *Text2);
	void ChangeHelpLine(int Line, bool State);
	void DeleteHelp();

	void SetDelay(int _Delay);

#if defined(PROPER_FRONTEND)
#  if defined (IGOR)
	int TopBar[4], BottomBar[4], TopFill[4], BottomFill[4];
#  else
	D3DTLVERTEX TopBar[4], BottomBar[4], TopFill[4], BottomFill[4];
#  endif (IGOR)
#endif

	int PreviousCounter;

	int ScreenName;

	// Command to pass back to the main bit.
	char QueuedCommand;
	void *QueuedValue;

	// Do we use the default backdrop?
	bool UseDefaultBackdrop;

	// Fading out?
	bool FadeOut;
	int FadeOutCounter;
	int Delay;
	bool Canceling;
	int Heights[2];

	// The backdrop.
	Render2D Backdrop[8];
	Render2D Bars[2], BarFill[2];

	// The text at the bottom
	TextWord *HelpText[2];

	Alphabet *MainAlphabet;

	Archive *ScreenArchive;
#if !defined(DEBUGMEM)
	void *operator new(size_t t)
	{
		if(t>100000)
		{
			dprintf("screen requires %d bytes",t);
			return NULL;
		}
		extern void InitStaticScreens();
		if(!static_screens[0])InitStaticScreens();
		dprintf("returning static screen %lx",static_screens[static_screen_ct]);
		void *q = (void *)(static_screens[static_screen_ct++]);
		memset(q,0,t);
		if(static_screen_ct==3)static_screen_ct=0;

		return q;
	}

	void operator delete(void *q)
	{
		dprintf("deleting screen %lx",q);
	}

#endif
private:
};
