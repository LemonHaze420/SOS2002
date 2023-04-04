/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       wassert.c
 *  Content:    Windows assert handler
 *              You must externally define hWndMain and szAppName for this
 *              to work.
 *
 ***************************************************************************/

/*
	$Header$
	$Log$
	Revision 1.0  2000-02-21 12:34:33+00  jjs
	Initial revision

	Revision 1.4  1998/12/02 12:17:06  jcf
	Added Id properly again...

	Revision 1.3  1998/12/02 12:03:51  jcf
	Added Id properly ;)

	Revision 1.2  1998/12/02 12:01:17  jcf
	Added ID and Log.

*/
static const char *rcs_id = "$Id$";

#include <windows.h>

extern HWND hWndMain;
extern char szAppName[];

#include "wassert.h"


#ifdef ASSERT
void AssertFail(char szErr[], char szFileName[], int nLine, char szMessage[])
    {
    char szT[256];
#if !defined(UNDER_CE)
    if (szMessage != NULL)
        wsprintf(szT, "Assert(%s);\nFile %s, line %d.  %s", szErr, szFileName, nLine, szMessage);
    else 
        wsprintf(szT, "Assert(%s);\nFile %s, line %d.", szErr, szFileName, nLine);
    switch (MessageBox(NULL, szT, "ITCC", MB_ABORTRETRYIGNORE | MB_ICONSTOP | MB_APPLMODAL))
        {
        case IDABORT:
        case IDRETRY:
            _asm int 3;
            // Fall Through //
        case IDIGNORE:
            break;

        } // switch
#endif
    } // AssertFail


#endif // ASSERT

