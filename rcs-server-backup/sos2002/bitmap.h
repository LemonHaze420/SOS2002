/* Bitmap.h

	 Class to manipulate bitmaps.
 	 Author:	Ian Gledhill
 	 Date:	22nd June 1999

     (C) Broadsword Interactive Ltd. */

#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <ddraw.h>

class Bitmap
{
public:
	// The constructor.  Loads in the bitmap to memory.
	// Pass in a NULL as the owner if you like, but some functions will be unavailable.
	Bitmap(char *BitmapName, int x, int y, class FrontEndClass *Owner, bool SaveBackup = true, bool UseSystemMemory=false);
	~Bitmap();

	// Render the bitmap. Assume default dimensions if none given.
	// Otherwise render bitmap x1,y1,x2,y2 to x3,y3,x4,y4.
	// Passing in -1 to x1,y1,x2,y2 means use width and height.
	// Ditto for x3...y4.
	// AlphaMask should be set to the colour of the Alpha, or -1 if no Alpha.
	Render(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, WORD AlphaMask, LPDIRECTDRAWSURFACE7 DestinationSurface=g_pddsBackBuffer);
	Render();

	// Blit from the background onto the surface to be used as a history.
	void LoadFromBackground(int x, int y);
	void LoadFromBackground(int x, int y, RECT *ClipRect);

	// Set this to validate/invalidate the bitmap
	void SetValid(bool _Valid);
	bool GetValid();

	// Shade a rectangle on the bitmap.
	void Shade(int x1, int y1, int x2, int y2, int degree);

	// Copy one bitmap to another.
	void Copy(Bitmap *Destination, int x1, int y1, int x2, int y2);

	LPDIRECTDRAWSURFACE4 GetSurface();

	void Line(int x1, int y1, int x2, int y2, int Colour);

private:
	// The parent frontend class.
	class FrontEndClass *ParentFrontEnd;

	// The path to the bitmap.
	char Name[1024];

	// Set this is if we want to keep backups
	bool SaveBackup;

	// The surface of the bitmap.
	LPDIRECTDRAWSURFACE4 Surface;
	// And the old contents of the bitmap.
	LPDIRECTDRAWSURFACE4 OldSurface;

	// The dimensions.
	int Width, Height;

	// The location
	RECT Location;

	// This set to true if the bitmap can be used.
	bool Valid;
};

#endif __BITMAP_H__