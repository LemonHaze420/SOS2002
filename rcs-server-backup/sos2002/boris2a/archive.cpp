/****Mon Dec 15 14:27:00 1997*************(C)*Broadsword*Interactive****
 *
 * Implementation for the TARC wrapper class.
 *
 ***********************************************************************/

#include <windows.h>
#include <stdio.h>

#include "\boris\tarcman/tarc.h"
#include "archive.h"


BroadArchive::BroadArchive()
{
	t = new tarc;
	isopen=FALSE;
}

BroadArchive::~BroadArchive()
{
	if(isopen)Close();
	delete t;
}


void BroadArchive::Close()
{
	if(isopen)
	{
		TARC_CloseR(t);
		isopen=FALSE;
	}
}

BOOL BroadArchive::Open(char *filename)
{
	if(TARC_OpenR(t,filename))
		return FALSE;
	else
	{
		isopen=TRUE;
		return TRUE;
	}
}

unsigned long BroadArchive::Extract(char *name,unsigned long id,char *buffer)
{
	extern void Wibble();

	if(!isopen)return FALSE;
	size=TARC_Extract(t,name,id,TARCV_LATEST,buffer);


	return size;
}

struct tarc_index *BroadArchive::Stat(char *name,unsigned long id)
{
	if(!isopen)return NULL;

	return TARC_GetIndexData(t,name,id,TARCV_LATEST);
}
