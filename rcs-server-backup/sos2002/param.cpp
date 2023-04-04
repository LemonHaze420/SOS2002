// $Header$

// $Log$
// Revision 1.0  2000-02-21 12:34:25+00  jjs
// Initial revision
//

/****Thu Feb 26 09:58:00 1998*************(C)*Broadsword*Interactive****
 *
 * Parameter file implementation
 *
 ***********************************************************************/


static const char *rcs_id="$Id$";

#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#if !defined(UNDER_CE)
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include "BS2all.h"
#include "archiveiface.h"
#include "param.h"

void dprintf(char *,...);
#define dprintf(x)

#define isalnum(x) (((x)>='0' && (x)<='9') || ((x)>='A' && (x)<='Z') || ((x)>='a' && (x)<='z'))

ParamFile::ParamFile()
{
	num=0;curoff=-1;lastlen=-1;
	data=NULL;keys=NULL;vals=NULL;offset=0;
}

ParamFile::~ParamFile()
{
//	if(data)
//	{
//		free(data);data=NULL;
//		delete [] keys;
//		delete [] vals;
//	}
}

bool ParamFile::ReadFromMemory(char *area,unsigned long size)
{
	char *p,*q;
	char *curdata;
	unsigned long off=0,stringsize=0L,i;
	int s;
	int addoff;

	// clear out any old data
	
	if(data)
	{
		free(data);data=NULL;
		delete [] keys;
		delete [] vals;
	}
	
	num = 0;

	// get a pointer to the data
	
	p = area;
	
	// preprocess the data by replacing crlf pairs with NULL.
	
	for(off=0;off<size;off++)if(p[off]==0x0d || p[off]==0x0a)p[off]=0;
		
		
	// now go through the data, just counting items
	// first.
	
	for(off=0;off<size;off++)
	{
		// at this point we are pointing to a line.. if it
		// starts with # it's a comment
		
		if(p[off] != '#' && p[off])
		{
			// if it doesn't have a colon, ignore it.
			
			if(q=strchr(p+off,':'))
			{
				// aha.. an item..
				num++;
				addoff = strlen(p+off);
				// work out the size of the key and value
				
				s = off;
				
				// find the key's end
				while(p[s]!=':' && p[s]!=' ' && p[s]!='\t')s++;
				stringsize+= (s-off)+1;
				
				q++;s=0;
				while(*q==' ' || *q=='\t')q++; // find val start
				if(*q=='\"') // quoted string
				{
					q++;
					while(q[s] && q[s]!='\"')s++;q[s--]=0;
				}
				else
				{
					while(q[s] && q[s]!=' ' && q[s]!='\t')s++;
				}
				q[s]=0;	// null terminate the value
				stringsize+= s+1;
				off += addoff;
			}
		}
		
		// next line, please...
		
		while(off<size && p[off])off++;	// skip till NULL or eof
		off++;	// increase the pointer past the extra NULL.
		if(off>=size)break;	// if at eof, end.
	}
	
	// OK, we now have a count of items in 'num' and the string length in 'stringsize'
	// ... create the arrays
	
	data = (char *)malloc(stringsize);
	keys = new char * [num];
	vals = new char * [num];
	
	// reparse the file, filling the data in.
	
	i=0;	// index of current item
	
	curdata=data;

	for(off=0;off<size;off++)
	{
		
		// at this point we are pointing to a line.. if it
		// starts with # it's a comment
		
		if(p[off] != '#' && p[off])
		{
			// if it doesn't have a colon, ignore it.
			
			if(q=strchr(p+off,':'))
			{
				s=off;
				addoff = strlen(p+off);


				while(p[s]!=':' && p[s]!=' ' && p[s]!='\t')s++; // find end of key
				p[s]=0;
				
				keys[i] = curdata;
				
				strcpy(curdata,p+off);
				curdata+=strlen(p+off);*curdata++ = 0;
				
				// find the value start
				
				while(! (isalnum(*q) || *q=='-' || *q=='.')    )q++;
				strcpy(curdata,q);
				vals[i]=curdata;
				curdata+=strlen(q);*curdata++ = 0;
				
				dprintf("param: Got %s = %s",keys[i],vals[i]);
				i++;
				off += addoff;
			}
		}
		
		// next line, please...
		
		while(off<size && p[off])off++;	// skip till NULL or eof
		off++;	// increase the pointer past the extra NULL.
		if(off>=size)break;	// if at eof, end.
	}
	dprintf("all read");
	return true;
}


bool ParamFile::Read(char *name)
{
	dprintf("param: Reading %s (FROM ARCHIVE)",name);

	ulong handle = arcExtract_txt(name);
	if(!handle)return false;
	char *area = arcGetData(handle);
	unsigned long size = arcGetSize(handle);

	bool rv = ReadFromMemory(area,size);

	dprintf("calling arcDispose");
	arcDispose(handle);
	dprintf("arcDispose OK");
	return rv;
}

bool ParamFile::ReadFromFile(char *name)
{
	dprintf("param: Reading %s (FROM FILE)",name);
#if defined(UNDER_CE)
	int size;

	FILE *a=fopen(name,"rb");
	fseek(a,SEEK_END,0);
	size = ftell(a);
	fseek(a,SEEK_SET,0);
	char *d = (char *)malloc(size);
	fread(d,size,1,a);
	fclose(a);

	bool rv = ReadFromMemory(d,size);
#else
	struct _stat sbuf;

	if(_stat(name,&sbuf))return false;

	char *d = (char *)malloc(sbuf.st_size);
	
	FILE *a=fopen(name,"rb");
	fread(d,sbuf.st_size,1,a);
	fclose(a);

	bool rv = ReadFromMemory(d,sbuf.st_size);
#endif
	
	free(d);
	return rv;
}


	

// search the file for the given key - if NULL, look for more occurrences
// of the previous Find key. Returns the value, or NULL if the key
// is not found.

char *ParamFile::Find(char *key,int len)
{
	if(key)
	{
		strcpy(curkey,key);
		offset=0;	// specifying a new search
	}
	else
		if(len==-1)len=lastlen;

	lastlen=len;

	for(;offset<num;offset++)
	{

		if(!len)
		{
			curoff=offset;
			return vals[offset++];	// return all values
		}
		else if(len<0)				// normal search
		{
			if(!strcmp(curkey,keys[offset]))
			{
				curoff=offset;
				return vals[offset++];
			}
		}
		else if(!strncmp(curkey,keys[offset],len))
		{
			curoff=offset;
			return vals[offset++];
		}
			
	}

	curoff=-1;
	return NULL;
}


