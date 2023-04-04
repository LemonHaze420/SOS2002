// $Header$

// $Log$
// Revision 1.3  2000-04-11 17:05:42+01  jjs
// Uses unsigned char to allow for foreign language and copyright symbols.
//
// Revision 1.2  2000-02-29 13:34:18+00  jjs
// Removed dependancy on DREAMCAST define.
//


/* dprintf.cpp

  usage:
  dprintf(const char *template, ...)

  See printf for more details.

  (C) Broadsword Interactive Ltd.

  Author:	Ian Gledhill
  Date:		22nd June 1999
*/

#include "errorlib.h"
#include <windows.h>

void __cdecl dprintf(char *format, ...)
{
	char buf[2048];
	va_list argptr;

	va_start(argptr,format);
	vsprintf(buf,format,argptr);

#if defined(UNDER_CE)
	unsigned short NewBuf[2048];
	ASCIItoUNI((unsigned char *)buf, NewBuf);
	OutputDebugString(NewBuf);OutputDebugString(TEXT("\n"));
#else	
	OutputDebugString(buf);OutputDebugString("\n");
#endif
	va_end(argptr);
}