/* Button.h

	 The button class.
 	 Author:	Ian Gledhill
 	 Date:	25th June 1999

     (C) Broadsword Interactive Ltd. */


#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <windows.h>

class Button
{
public:
	Button(int _x, int _y, int w, int h, char *_Text, char *Font1, char *Font2, char *_CallBack, char *_CallBackDelay, FrontEndClass *Owner);
	
	// This also blits the previous background to the main bitmap.
	~Button();

	// Highlight the button by rendering the text 100%
	void Highlight();
	// Or Lowlight it by rendering it dark.
	void Lowlight();

	// IsUnderXY returns true if the zone specified by the button is under the co-ordinate at (x,y)
	bool IsUnderXY(int xc, int yc);

	// GetCallback returns the name of the label used as the callback function.
	char *GetCallback();

	// Get CallBackDelay returns the name of the label to use when the button is depressed for a second.
	char *GetCallBackDelay();

private:
	// The owner class.
	FrontEndClass *ParentFrontEnd;
	// The location on the bitmap of the button.
	int x, y, Width, Height;
	RECT Location;

	// The label of the button.
	char Text[256];

	// The fonts used, and the function name.
	char NormalFont[256], HighlightedFont[256], CallBack[256], CallBackDelay[256];

	// The old contents of the bitmap.
	LPDIRECTDRAWSURFACE4 OldSurface;
};

#endif  __BUTTON_H__
