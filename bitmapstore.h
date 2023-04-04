// $Header$

// $Log$

#ifndef _BITMAPSTORE_H_
#define _BITMAPSTORE_H_

// Prevents to multiple defining of HWND. 
#if !defined(STRICT)
#define STRICT  
#endif

// ------------------------------------------------------------------------------------------
// Includes
#include <ddraw.h>
#include <windows.h>

// End section
// ------------------------------------------------------------------------------------------

// - Class ----------------------------------------------------------------------------------
class BitmapStore
{
public:
	
	BitmapStore();
	~BitmapStore()
	{
		ClearBitmaps();
	}

	// Loads an SSB file and returns the index to the newly 
	// added bitmap, or -1 in the event of a failure.
	int AddBitmap(char	*_Name);

	// Returns true if the bitmap was successfully removed.
	bool RemoveBitmap(int _ID);

	// Clears the lot
	void ClearBitmaps();

	// Returns the number of Bitmaps currently being stored
	int NumOfBitmaps()
	{
		return NumberOfBitmaps;
	}

	// Returns the ID of the named bitmap, if stored. (-1 if not found).
	int	GetBitmapID(char *_Name);

	// Returns the bitmap's width and height respectively.
	int GetBitmapWidth(int _ID)
	{
		return ListOfBitmaps[_ID].Width;	
	}

	int GetBitmapHeight(int _ID)
	{
		return ListOfBitmaps[_ID].Height;	
	}

	// Given the index of a bitmap this method
	// returns a pointer to the bitmap's surface.
	LPDIRECTDRAWSURFACE4 BitmapTextureHandle(int _ID);

	// Returns the handle of the named bitmap.
	LPDIRECTDRAWSURFACE4 BitmapTextureHandle(char *_Name);

private:
	
	#define MAX_BITMAPS 128

	// Store the width, height and the texture handle
	struct BitmapStruct
	{
		char	Name[48];
		int	Width;
		int	Height;
		//DWORD	TextureHandle;
		LPDIRECTDRAWSURFACE4 TextureHandle;
	};

	// C style, although C++ legal too..
	struct BitmapStruct ListOfBitmaps[MAX_BITMAPS];
	int NumberOfBitmaps;
};

#endif