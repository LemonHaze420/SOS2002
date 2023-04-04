/* Shade.h

	 Class to describe shadings on the bitmap.
 	 Author:	Ian Gledhill
 	 Date:	28th June 1999

     (C) Broadsword Interactive Ltd. */

#ifndef __SHADE_H__
#define __SHADE_H__

class Shade
{
public:
	// The constructor.
	Shade(int _x, int _y, int _Width, int _Height, int _Degree, Bitmap *_UsedBitmap, FrontEndClass *_Owner);

	// And the destructor. This replaces the bitmap that was before on the surface.
	~Shade();
private:
	// The owning front end.
	FrontEndClass *ParentFrontEnd;

	// The bitmap that the shade applies to.
	Bitmap *UsedBitmap;

	// The rectangle that the shade applies to.
	RECT Location;
	// And the amount of the shading, as a percentage.
	int Degree;

	// The previous contents of the buffer.
	LPDIRECTDRAWSURFACE4 OldSurface;
};

#endif  __SHADE_H__