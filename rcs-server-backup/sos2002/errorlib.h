/* ErrorLib.h

	The main header file for all the errors in the Error Lib.

	Author:		Ian Gledhill
	Date:		22nd June 1999

	(C) Broadsword Interactive Ltd. */


#ifdef WIN32
# include <windows.h>
#endif

#include <stdio.h>
#include <stdarg.h>

#define fatalError FatalError

void __cdecl FatalError(char *ErrorText, ...);
void __cdecl NonFatalError(char *ErrorText, ...);	// Same as FatalError but returns.

void __cdecl dprintf(char *format, ...);