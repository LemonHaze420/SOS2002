/* FatalError.h

  usage:
  dprintf(const char *template, ...)

  See printf for more details.

  (C) Broadsword Interactive Ltd.

  Author:	Ian Gledhill
  Date:		22nd June 1999
*/

#include <Windows.h>

#include <stdio.h>

#include "ErrorLib.h"

void __cdecl FatalError(char *ErrorText);
void __cdecl fatalError(char *ErrorText);		// calls FatalError().
void __cdecl NonFatalError(char *ErrorText);	// Same as FatalError but returns.