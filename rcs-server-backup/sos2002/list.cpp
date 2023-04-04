/****Wed Oct 22 09:40:36 1997*************(C)*Broadsword*Interactive****
 *
 * Implementation of the voidList, currently as a linked list
 * within an array. This can list void pointers. Anything, in other words.
 *
 ***********************************************************************/

#include <windows.h>
#include "list.h"

voidList::voidList(int sz)
{
	// This will construct the list and allocate the free list.

	freehead=0;	// freelist starts at 0
	head=-1;	// no items in list
	size=sz;
	
	nextlist = new long[size];
	prevlist = new long[size];

	objlist = new void * [size];

	// set up the free list
	
	for(int i=0;i<size;i++)
	{
		nextlist[i]=i+1;
		prevlist[i]=0;	// not used in free list
		objlist[i]=NULL;
	}
	nextlist[size-1]=-1;
}

voidList::~voidList()
{
	delete[] nextlist;
	delete[] objlist;
}

void voidList::AddToFreeList(int slot)
{
	objlist[slot]=NULL;
	nextlist[slot]=freehead;
	freehead=slot;
}


int voidList::Add(void *obj)
{
	// get a free node

	long n;

	if(freehead<0)return -1;	// no free nodes
	n=freehead;
	freehead=nextlist[freehead];

	// n is a free node

	nextlist[n]=head;
	prevlist[n]=-1;
	if(head>=0)prevlist[head]=n;
	head=n;
	objlist[n]=obj;	

	return n;
}

BOOL voidList::Remove(void *obj)
{
	for(long i=0;i<size;i++)
		if(obj==objlist[i])
			return Remove(i);
	return FALSE;
}

BOOL voidList::Remove(int slot)
{
	long q;
	
	if(!objlist[slot])return FALSE;

	if(slot==head)
		head=nextlist[head];
	else
	{
		q=prevlist[slot];
		nextlist[q]=nextlist[slot];
		if((q=nextlist[slot])>=0)
			prevlist[q]=prevlist[slot];
	}
	AddToFreeList(slot);
	return TRUE;		
}

void *voidList::First()
{
	current=head;

	if(head>=0)
		return objlist[head];
	else
		return NULL;
}

void *voidList::Next()
{
	if(current>=0)
	{
		current=nextlist[current];
		if(current>=0)
			return objlist[current];
	}
	return NULL;
}

void voidList::Empty()
{
	freehead=0;
	head=-1;

	for(int i=0;i<size;i++)
	{
		nextlist[i]=i+1;
		prevlist[i]=0;	// not used in free list
		objlist[i]=NULL;
	}
	nextlist[size-1]=-1;
}
