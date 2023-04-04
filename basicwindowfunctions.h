// $Header$

// $Log$

#ifndef __BASICWINDOWFUNCTIONS_H__
#define __BASICWINDOWFUNCTIONS_H__

// ----------------------------------------------------------------------------
// Open a nice little message box.
//void Notify(char *_Message="", char *_Title="Notification");
void Notify(char *_Message="", char *_Title="Notification", int _CrashCode = 0);

// ----------------------------------------------------------------------------
// Allows the user to render text to the screen.
// Width and Height are zero by default, and if left
// as default will use the size of the font selected.
// Returns the width of the text output.
/*int StaticTextArea(	int		_X, 
					int		_Y,
					char	*_Message,
					int		_Font,
					int		_Alignment	=	0);	// Left alignment
*/
/*
// ----------------------------------------------------------------------------
// Draws a button on the screen using the text given in _Message.
// Returns	0 if the mouse is not within the bounding region of the text.
//			1 if inside text area AND the mouse button is released.
//			2 if inside area, but is being held down
int TextButton(	int		_X,
				int		_Y,
				char	*_Message,
				int		_Font,
				int		_Alignment	=	0);	// Left. 1 for right hand alignment
*/
// ----------------------------------------------------------------------------
// Draws a button on the screen using the given _Image.
// Returns true if the button is released.
/*bool ImageButton(	int		_X,
					int		_Y,
					char	*_Image,
					char	*_AltImage	=	"",
					char	*_Help		=	"",
					int		Width		=	0,
					int		Height		=	0);

*/

#endif