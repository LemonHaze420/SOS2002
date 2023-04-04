/* Video.h

	 Class to show videos.
 	 Author:	Ian Gledhill
 	 Date:	24th June 1999

     (C) Broadsword Interactive Ltd. */

#ifndef __VIDEO_H__
#define __VIDEO_H__

#include <windows.h>
#include <mmstream.h>
#include <amstream.h>
#include <ddstream.h>
#include <vfw.h>

class Video
{
public:
	// The constructor. Guess what the parameters are!
	Video(int _x, int _y, char *VideoName, int _Speed, HWND _VideoHWND, bool _WindowMode, FrontEndClass *Owner);

	// And the destructor. This returns the bitmap to its original state.
	~Video();

	// Draw the next frame.
	void Update();

	// Kill the video. Shutdown DXMedia and stuff.
	Kill();

	// Blit the video frame to another surface
	void BlitToSurface(LPDIRECTDRAWSURFACE4 DestinationSurface);

private:
	// The location and speed of the video stream. Interval is the time between ticks.
	int x, y, Speed, Interval;
	char Name[1024];

	RECT Location;

	// Identifier of the DirectDraw class
	IDirectDraw *DirectDrawPointer;
	

	PAVISTREAM    m_paviStream;    // The AVI stream
	PAVIFILE		m_paviFile;
	PGETFRAME     m_pgfFrame;      // Where in the stream to get the next frame
	AVISTREAMINFO m_psiStreamInfo; // Info about the AVI stream
	AVIFILEINFO	  m_pfiFileInfo;
	// The render area
	RECT VideoRect;

	int StartTime;
	DWORD dwCurrFrame;

	// HWND for the window.
	HWND VideoHWND;
	// Parent front end.
	FrontEndClass *ParentFrontEnd;

	// Set this if rendering to a window.
	bool WindowMode;

	// The surface to render to
	LPDIRECTDRAWSURFACE4 RenderSurface;
};

#endif  __VIDEO_H__