// $Header$

// $Log$
// Revision 1.0  2002-07-17 16:06:17+01  jjs
// Initial revision
//
// Revision 1.4  2000-04-11 17:05:43+01  jjs
// Uses unsigned char to allow for foreign language and copyright symbols.
//
// Revision 1.3  2000-02-29 12:50:25+00  jjs
// Really removed dependancy on DREAMCAST define.
//
// Revision 1.2  2000-02-29 12:42:08+00  jjs
// Removed dependancy on DREAMCAST define.
//

/* FatalError.cpp

  usage:
  dprintf(const char *template, ...)

  See printf for more details.

  (C) Broadsword Interactive Ltd.

  Author:	Ian Gledhill
  Date:		22nd June 1999
*/

#define FILEOUTPUT 0

#include "FatalError.h"

#ifndef WIN32
# define OutputDebugString(x) dprintf(x)
#endif

void __cdecl FatalError(char *ErrorText, ...)
{
	char buf[2048];
	va_list argptr;

	va_start(argptr,ErrorText);
	vsprintf(buf,ErrorText,argptr);
	va_end(argptr);

#if defined(UNDER_CE)
	unsigned short NewBuf[2048];
	ASCIItoUNI((unsigned char *)buf, NewBuf);
	OutputDebugString(TEXT("*** FATAL ERROR ***: "));
	OutputDebugString(NewBuf);OutputDebugString(TEXT("\n"));
#else	
	OutputDebugString("*** FATAL ERROR ***: ");
	OutputDebugString(buf);
	OutputDebugString("\n");
#endif


#if FILEOUTPUT
	FILE *DebugOutputFile;

	DebugOutputFile = fopen("FatalError.txt", "a+");

	if (DebugOutputFile)
	{
		fseek(DebugOutputFile, 0, SEEK_END);
		fprintf(DebugOutputFile, buf);
	}
	fclose(DebugOutputFile);
#endif
	exit(0);
}

void __cdecl NonFatalError(char *ErrorText, ...)
{
	char buf[2048];
	va_list argptr;

	va_start(argptr,ErrorText);
	vsprintf(buf,ErrorText,argptr);
	va_end(argptr);
	
	strcat(buf, "\n");

#if defined(UNDER_CE)
	unsigned short NewBuf[2048];
	ASCIItoUNI((unsigned char *)buf, NewBuf);
	OutputDebugString(TEXT("*** NON-FATAL ERROR ***: "));
	OutputDebugString(NewBuf);OutputDebugString(TEXT("\n"));
#else	
	OutputDebugString("*** NON-FATAL ERROR ***: ");
	OutputDebugString(buf);
	OutputDebugString("\n");
#endif

#if FILEOUTPUT
	FILE *DebugOutputFile;

	DebugOutputFile = fopen("FatalError.txt", "a");

	if (DebugOutputFile)
	{
		fseek(DebugOutputFile, 0, SEEK_END);
		
		fprintf(DebugOutputFile, buf);
		fclose(DebugOutputFile);
	}
#endif
	return;
}