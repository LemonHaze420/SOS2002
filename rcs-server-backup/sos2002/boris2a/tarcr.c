/****Thu Dec 11 16:49:29 1997*************(C)*Broadsword*Interactive****
 *
 * Code necessary for reading from a tarc
 *
 ***********************************************************************/

#if defined(_WINDOWS)
#include <windows.h>
#else
#include <stdio.h>
#endif

#include "tarc.h"

/*
	$Header$
	$Log$
	Revision 1.0  2002-07-18 12:03:36+01  jjs
	Initial revision

	Revision 1.2  1998/12/02 12:09:40  jcf
	Did the Id wrong again :)

	Revision 1.1  1998/12/02 12:08:15  jcf
	Initial revision

*/

static const char *rcs_id = "$Id$";

/****Thu Dec 11 16:52:02 1997*************(C)*Broadsword*Interactive****
 *
 * Read the index block of a tarc. Returns 100+ if not ok, otherwise 0
 *
 ***********************************************************************/

int TARC_ReadHeaderAndIndex(struct tarc *t)
{
    struct tarc_index *ti;
#if defined(_WINDOWS)
    _llseek(t->h,0,0);	// rewind the file
    _lread(t->h,&(t->hd),sizeof(struct tarc_header)); // get header
#else
    fseek(t->h,0,SEEK_SET);
    fread((void *)&(t->hd),sizeof(struct tarc_header),1,t->h);
#endif
    
    TARC_Crypt((char *)&(t->hd),sizeof(struct tarc_header),KeyHeader);
    
    if(t->hd.magic != TARC_MAGIC && t->hd.magic!=TARC_MAGIC_NOCRYPT) return 100;	// not a tarc
    
    // allocate the index
    
    if(t->hd.filect)
    {	
		int qq;

        ti=(struct tarc_index *)
                  malloc(sizeof(struct tarc_index)*t->hd.filect);
        
        // and read it in
#if defined(_WINDOWS)    
        _llseek(t->h,t->hd.index,0);
        _lread(t->h,ti,t->hd.filect*sizeof(struct tarc_index));
#else
        qq=fseek(t->h,t->hd.index,SEEK_SET);
		qq=ftell(t->h);
        qq=fread((void *)ti,1,sizeof(struct tarc_index)*t->hd.filect,t->h);
		if(ferror(t->h))
			perror("oops");
#endif
        TARC_Crypt((char *)ti,sizeof(struct tarc_index)*t->hd.filect,
                   KeyIndex);
        
        t->i = ti;
    }
    else t->i=NULL;
    return 0;
} 




/****Thu Dec 11 16:49:58 1997*************(C)*Broadsword*Interactive****
 *
 * Open a tarc for reading and read the index block.
 *
 ***********************************************************************/

int TARC_OpenR(struct tarc *t,char *fname)
{
#if defined(_WINDOWS)
    OFSTRUCT of;
    HFILE h;
#else
	FILE *h;
#endif
    int rv;
    
    strcpy(t->fn,fname);
#if defined(_WINDOWS)
    h=OpenFile(fname,&of,OF_READ);
    if(h==HFILE_ERROR)return -1;
#else
    h=fopen("fname","rb");
    if(!h)return -1;
#endif
    
    t->h = h;
    
    if(rv= TARC_ReadHeaderAndIndex(t))return rv;
    t->writable=0;
    return 0;
}

/****Fri Dec 12 10:41:43 1997*************(C)*Broadsword*Interactive****
 *
 * Used to determine whether an index entry matches a format/name pair.
 * Returns nz if it does.
 *
 ***********************************************************************/

int TARC_EntryMatches(struct tarc_index *i,unsigned long id,char *name,short version)
{
    if(name && stricmp(name,i->tname))return 0;
    
    if((version!= TARCV_DEL) && (i->flags & TARCF_DELETED))return 0;
    
    if((version == TARCV_DEL) && !(i->flags & TARCF_DELETED))
        return 0;
    else if((version == TARCV_LATEST) && (i->flags & TARCF_SUPERCEDED))
        return 0;
    else if((version == TARCV_OLD) && !(i->flags & TARCF_SUPERCEDED))
        return 0;
    else if((version >= 0) && (i->version!=version))
        return 0;
    
#define CHECKBITS(bits)	( (id & (bits)) && ((id & (bits)) != (i->id & (bits))))
    
    if(CHECKBITS(0xff))return 0;
    if(CHECKBITS(0xff00))return 0;
    if(CHECKBITS(0xff0000))return 0;
    if(CHECKBITS(0xff000000))return 0;
    
    return 1;
}

/****Mon Dec 15 14:35:20 1997*************(C)*Broadsword*Interactive****
 *
 * Close a tarc
 *
 ***********************************************************************/

void TARC_CloseR(struct tarc *t)
{
#if defined(_WINDOWS)
    _lclose(t->h);
#else
    fclose(t->h);
#endif
    if(t->i)
    {
        free(t->i);t->i = NULL;
    }
}

struct tarc_index *TARC_GetIndexData(struct tarc *t,char *name,unsigned long id,
                                     short version)
{
    unsigned long i;
    struct tarc_index *ti;
    
    ti = t->i;
    
    for(i=0;i<t->hd.filect;i++)
    {
        if(TARC_EntryMatches(ti,id,name,version))
        {
            return ti;
        }
        ti++;
    }
    return NULL;
}



/****Thu Dec 11 16:50:33 1997*************(C)*Broadsword*Interactive****
 *
 * Extract a file from a tarc into a given memory block. This extracts
 * the first block with the same tname which matches the format.
 * It returns the size of the block, or 0 if nothing was found. If the
 * buffer pointer is NULL, it does no reading but still returns the size.
 *
 ***********************************************************************/

unsigned long TARC_Extract(struct tarc *t,char *name,unsigned long id,
                           short version,char *buf)
{
    struct tarc_index *ti;
    
    ti = TARC_GetIndexData(t,name,id,version);
    
    if(ti && buf)
    {
        TARC_ExtractEntry(t,ti,buf);
        return ti->size;
    }
    else return 0;
}




/****Fri Dec 12 14:09:19 1997*************(C)*Broadsword*Interactive****
 *
 * This does the donkey work for the extraction.
 *
 ***********************************************************************/

void TARC_ExtractEntry(struct tarc *t,struct tarc_index *ti,void *buf)
{
#if defined(_WINDOWS)
    _llseek(t->h,ti->index,0);
    _lread(t->h,buf,ti->size);
#else
    fseek(t->h,ti->index,SEEK_SET);
    fread((void *)buf,ti->size,1,t->h);
#endif
    
    if(t->hd.magic!=TARC_MAGIC_NOCRYPT)
        TARC_Crypt(buf,ti->size,KeyBody);
}



/****Mon Dec 15 12:39:26 1997*************(C)*Broadsword*Interactive****
 *
 * Really really really dumb encryption
 *
 ***********************************************************************/

char *KeyHeader="H12f4J4o^5$Ep8&a^7kl%5I4$n$3œnIS3e[2]Z1A;b'e[7]t'7;h8";

char *KeyBody=
"sjdklfjskldjfweortu2304048n02384v90285pnerugeiyn237 4b97897(&(&œ(*$&("
"23905b02n840yyyyyyyyy2034v20b8!9089034b8230480IOUIOSUDhqjw4289742byAsidyaify"
"09e8rb02802n3v5902808)(*s90e8naviosasfuwniorub034b082904ap][]35[24[5]2452-40"
"sledgehammersledgehammeraardvarksledgehammerfish2304820384v20389090)(*)*()*3"
"loosefishbadeggsadlumpscapegraceskldfjlksdjfw2*(&(*&W(*&(œ$&(œ*$&(*&!(((!&&e"
"23io58b29050238nv28903800*n(Nd)*)(w*v02380428304280368349068b306830498304n68";

char *KeyIndex=
"lqoaks'))!(****$90128390jdhfksksjdhfJH&%*$(%$&*%*%*$&%("
"klsd89090sd8reckonanyonewillguesskasjd3-230-3239490-490";

void TARC_Crypt(char *buf,unsigned long size,char *key)
{
    unsigned long i,q;
    
    q=strlen(key);
    for(i=0;i<size;i++)
    {
        // I really really really really hate to do this.
        // I am *quite* aware that this is the world's worst
        // encryption algorithm, thank you very much. Since
        // we're only trying to dissuade casual browsers,
        // I'm not too bothered.
        
        buf[i] ^= key[i%q];
    }
}

