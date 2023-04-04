/****Thu Jan 08 11:38:04 1998*************(C)*Broadsword*Interactive****
 *
 * Archive stack object
 *
 ***********************************************************************/

 
#include <windows.h>
#include <stdio.h>

#include "\boris\tarcman/tarc.h"
#include "\boris\archive.h"
#include "arcstack.h"

BroadArchiveStack archivestack;

BroadArchiveStack::BroadArchiveStack()
{
	stackct=0;
}

BroadArchiveStack::~BroadArchiveStack()
{
	;
}

char *BroadArchiveStack::MallocAndExtract(char *name, unsigned long id,unsigned long *size)
{
	int i=stackct;
	struct tarc_index *ti;
	char *data;

	do
	{
		if(ti = stack[--i].Stat(name,id))
		{
			data = (char *)malloc(ti->size);
			if(data)
			{
				if(size)*size = ti->size;
				stack[i].Extract(name,id,data);
			}
			return data;
		}
	} while(i);

	return NULL;
}

unsigned long BroadArchiveStack::ExtractIntoArea(char *name,unsigned long id,char *area,int asize)
{
	int i=stackct;
	struct tarc_index *ti;
	char *data;

	do
	{
		if(ti = stack[--i].Stat(name,id))
		{
			if(ti->size > asize)return 0xffffffff;
					
			stack[i].Extract(name,id,area);
			return ti->size;
		}
	} while(i);
	return 0;
}

BOOL BroadArchiveStack::Push(char *name)
{
	if(stack[stackct].Open(name))
	{
		stackct++;
		return TRUE;
	}
	return FALSE;
}

void BroadArchiveStack::Pop()
{
	if(stackct)
		stack[--stackct].Close();
}


