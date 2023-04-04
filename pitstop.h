/* Pitstop.h

	Routines to govern the pitstops, viz. starting, stopping and
	updating them.

	Ian Gledhill   13/07/1999

	Broadsword Interactive Ltd */

#ifndef __PITSTOP_H__
#define __PITSTOP_H__



class PitstopGraphics
{
public:
	// Initialise and load the pitstop graphics.
	// Pass in the name of the archive with the graphics in. This enables
	// use of locales, in theory.
	PitstopGraphics(char *ArchiveName);

	// Render a graphic.
	void RenderBitmap(int x, int y, int Icon, float pcnt);

	// Render the guage.
	// Fullness is a precentage.
	void RenderGauge(int x, int y, float Fullness);

	//Render the pixels 1:1 on the screen at the position given
	void Render1to1(int x, int y,int xp,int yp, int w, int h, int BitmapNumber, float ibx, float iby);

	//Render a percentage to the screen
	void RenderPercent(int x, int y, float pcnt);
private:

	DWORD Bitmap1,Bitmap2;

};


class Pitstop
{
public:
	// The constructor.
	// Pass in the car that's taking the pitstop
	Pitstop(int _CarNo, PitstopGraphics *_Graphics);

	// Update the pitstop; mend damage, count down and render stuff as
	// necessary.
	// Returns true when the pitstop is complete.
	bool Update();
	// Render the pitstop screen.
	void Render(bool dorender=true);

	// Stop the pitstop taking place.
	void Cancel();
	bool tyrechanging;
	bool frontchange;
private:
	// The number of the car that's taking the stop.
	int CarNo;

	// The amount of time remaining in the stop, in ticks.
	float TimeRemaining;

	int tyrechcount;

	// The actual graphics.
	PitstopGraphics *Graphics;
};

#endif __PITSTOP_H__