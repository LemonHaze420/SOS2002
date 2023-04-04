// SoundSystem.cpp: implementation of the SoundSystem class.
//
//////////////////////////////////////////////////////////////////////

/*
	$Header$
	$Log$
	Revision 1.0  2000-02-21 12:34:30+00  jjs
	Initial revision

	Revision 1.5  1999/03/23 10:59:47  jcf
	Start of week checkin

	Revision 1.4  1998/12/02 12:17:07  jcf
	Added Id properly again...

	Revision 1.3  1998/12/02 12:03:50  jcf
	Added Id properly ;)

	Revision 1.2  1998/12/02 12:01:17  jcf
	Added ID and Log.

*/
static const char *rcs_id = "$Id$";

#include "Sound.h"
#include "list.h"
#include "wave.h"
#include <math.h>
#if !defined(UNDER_CE)
#include <process.h>
#endif
#include "errorlib.h"

SoundSystem soundsys;
void dprintf(char *,...);


// these bits of code manage the duplicate buffer pools.

BroadList<LPDIRECTSOUNDBUFFER> *dupbuflist=NULL;
BroadList<LPDIRECTSOUND3DBUFFER> *dup3Dbuflist=NULL;



void SoundSystem::AddToDupList(LPDIRECTSOUNDBUFFER buf)
{
//	dprintf("adding duplicate buffer %lx",buf);
	dupbuflist->Add(buf);
}

void SoundSystem::AddToDupList3D(LPDIRECTSOUND3DBUFFER buf3d)
{
//	dprintf("adding duplicate 3D buffer %lx",buf3d);
	dup3Dbuflist->Add(buf3d);
}

char *chartab="qwertyuiopasdfghjklzxcvbnm";
void SoundSystem::ScanDupsAndDeleteFinished()
{
	LPDIRECTSOUNDBUFFER buf;
	LPDIRECTSOUND3DBUFFER buf3d;

	static ct=100;

	DWORD status;

	if(!--ct)
	{
		lpds->Compact();ct=100;
	}

	for(buf=dupbuflist->First();buf;buf=dupbuflist->Next())
	{
		buf->GetStatus(&status);
		if(!(status & DSBSTATUS_PLAYING))
		{
//			dprintf("removing buf from list %lx",buf);
			buf->Release();
//			dprintf("buf released");
			dupbuflist->Remove(buf);
//			dprintf("buf removed");
		}
	}

	for(buf3d=dup3Dbuflist->First();buf3d;buf3d=dup3Dbuflist->Next())
	{
	}


}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

char ccode[] = "pcssiTcssf%zf5mckdcle";
SoundSystem::SoundSystem()
{
	for(int i=0;i<NUMCHANS;i++)
	{
		chans[i].buf=NULL;chans[i].buf3d=NULL;
		chans[i].muted=false;
		chans[i].locked=false;
	}

	lpds=NULL;listener=NULL;soundlist=NULL;
	forcechannel=-1;eax_on=false;

	dupbuflist = new BroadList<LPDIRECTSOUNDBUFFER>(256);
	dup3Dbuflist = new BroadList<LPDIRECTSOUND3DBUFFER>(256);

	char *q=ccode;
	while(*q)
	{
		*q -= 97;
		*q++ = (*q<=25 && *q>=0) ? chartab[*q] : 32;
	}
}

SoundSystem::~SoundSystem()
{
	Shutdown();
}

void SoundSystem::Delete(Sound *s)
{
	SoundList *p,*q;

	// first case - head of list?

	if(s == &(soundlist->s))
	{
		p=soundlist;
		soundlist=soundlist->next;
		delete p;return;
	}

	for(p=soundlist;p;p=q)
	{
		q=p->next;
		if(s == &(q->s))
		{
			p->next = q->next;
			delete q;
			return;
		}
	}
}

bool SoundSystem::Initialise(void *hwnd,bool software,float rolloff)
{
	HRESULT rval;

	extern void generate_db_table();

	generate_db_table();

	goteax = false;
	oldrevtype = -1;


	dupbuflist = new BroadList<LPDIRECTSOUNDBUFFER>(256);
	dup3Dbuflist = new BroadList<LPDIRECTSOUND3DBUFFER>(256);
	
	usesoftware=software;
	rval = DirectSoundCreate(NULL,&lpds,NULL);
	if(rval != DS_OK)
	{
		DSError("DirectSoundCreate",rval);return false;
	}

	rval = lpds->SetCooperativeLevel((HWND)hwnd,DSSCL_PRIORITY);
	if(rval != DS_OK)
	{
		DSError("SetCooperativeLevel",rval);return false;
	}

	// get capabilities

	if(!GetCaps())return false;

	// set the hardware buffer format

	DSBUFFERDESC dsbdesc;
	INITSTRUCT(dsbdesc);

	dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D;
	LPDIRECTSOUNDBUFFER lpdsbPrimary;

	if FAILED(lpds->CreateSoundBuffer(&dsbdesc,&lpdsbPrimary,NULL))
	{
		DSError("CreateSoundBuffer(primary)",rval);return false;
	}


	WAVEFORMATEX wfx;
	memset(&wfx,0,sizeof(wfx));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;					// stereo
    wfx.nSamplesPerSec = 44100;			// 44Khz
	wfx.wBitsPerSample = 16;			// 16 bps

    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    rval = lpdsbPrimary->SetFormat(&wfx); 
	
	// now get the 3D stuff
	

	rval=lpdsbPrimary->QueryInterface(IID_IDirectSound3DListener,(LPVOID *)&listener);

	if(listener)
	{
		// set some params in the listener
 
		listener->SetPosition(0,0,0,DS3D_IMMEDIATE);
		listener->SetVelocity(0,0,0,DS3D_IMMEDIATE);

		listener->SetRolloffFactor(rolloff,DS3D_IMMEDIATE);
		listener->SetDopplerFactor(1.0,DS3D_IMMEDIATE);
	}

	InitialiseStreaming();
	
	return true;
}

bool SoundSystem::Initialise(long hwnd,bool software,float rolloff)
{
	return Initialise((HWND)hwnd,software,rolloff);
}


void SoundSystem::Shutdown()
{
	dprintf("SoundSystem shutdown commencing");
	if(lpds)
	{
		CloseDownStreaming();
		dprintf("deleting sounds");
		DeleteAllSounds();
		dprintf("deleting DirectSound");
		lpds->Release();
		lpds=NULL;
		dprintf("clearing duplicate lists");
		delete dupbuflist;
		delete dup3Dbuflist;
	}
	dprintf("SoundSystem shutdown complete");
}

bool SoundSystem::DSError(char *place,HRESULT errVal)
{
	char *s;
	sprintf(error,"%s: ",place);
	s=error+strlen(error);

	switch (errVal)
	{
	case DSERR_ALLOCATED:
		sprintf(s,"Resources already in use");
		break;
	case DSERR_CONTROLUNAVAIL:
		sprintf(s,"Requested controller is not availible");
		break;
	case DSERR_INVALIDPARAM:
		sprintf(s,"Got a parameter wrong fool");
		break;
	case DSERR_INVALIDCALL:
		sprintf(s,"Your call was wrong");
		break;
	case DSERR_GENERIC:
		sprintf(s,"Undocumented error");
		break;
	case DSERR_PRIOLEVELNEEDED:
		sprintf(s,"Insufficient priority for request");
		break;
	case DSERR_NOAGGREGATION:
		sprintf(s,"The object does not support aggregation");
		break;
	case DSERR_NODRIVER:
		sprintf(s,"You don't have a sound card");
		break;
	case DSERR_UNSUPPORTED:
		sprintf(s,"Function called is not supported on this device");
		break;
	case DSERR_OUTOFMEMORY:
		sprintf(s,"Out of memory for sound buffers");
		break;
	case DSERR_BADFORMAT:
		sprintf(s,"Bad wave format !!!???!!!");
		break;
	case DSERR_BUFFERLOST:	// RESTORE BUFFER!
		dprintf("buffer lost!!");
		return true;
	default:
		sprintf(s,"Unknown error message - %d",errVal);
		break;
	}

	dprintf("SOUND ERROR : %s",error);
	NonFatalError("SOUND ERROR : %s",error);

	return false;
	
}

bool SoundSystem::GetCaps()
{
	HRESULT rval;

	if(!lpds)return false;
	INITSTRUCT(dscaps);
	rval = lpds->GetCaps(&dscaps);
	if(rval!=DS_OK)
	{
		DSError("GetCaps",rval);
		return false;
	}
	return true;
}


int SoundSystem::GetFreeChannel(int priority)
{
	DWORD status;
	int found=0,maxpri=10000;

	// if we are forcing which channel we next play on, use that channel

	if(forcechannel>=0)
	{
		if(chans[forcechannel].buf)
		{
			chans[forcechannel].buf->Stop();
			KillChan(forcechannel);
		}
		found=forcechannel;
		forcechannel=-1;
		return found;
	}

	for(int i=0;i<NUMCHANS;i++)
	{
		if(!chans[i].muted && !chans[i].locked)
		{
			if(!chans[i].buf)	// no buffer, so it must be free.
				return i;

			// otherwise, see if it's still playing

			chans[i].buf->GetStatus(&status);
			if(!(status & DSBSTATUS_PLAYING))
			{
				// it's not, hurrah. If this is a cloned buffer, then
				// free it.

				KillChan(i);
				return i;
			}
		}
	}
	
	// no free channels. See if we can steal one.

	for(i=0;i<NUMCHANS;i++)	// get lowest priority channel
		if(!chans[i].locked && (chans[i].pri < maxpri)){found=i;maxpri=chans[i].pri;}
	
	if(priority<maxpri)
	{
		Error("unable to steal a low-priority channel");
		return -1;
	}

	// OK, now steal the channel.

	chans[found].buf->Stop();
	KillChan(found);
	return found;
}


void SoundSystem::SetVolume(int i,float v)
{
	extern bool Paused;

	DWORD status;
	LPDIRECTSOUNDBUFFER b = chans[i].buf;

	if(!lpds)return;

	if(!b)return;

	if(Paused)v=0;

	b->GetStatus(&status);
	if(status & DSBSTATUS_PLAYING)
		b->SetVolume((DWORD)((v-1)*10000.0f));	// range 0 to 1
}

void SoundSystem::SetFrequency(int i,float v)
{
	DWORD status;
	LPDIRECTSOUNDBUFFER b = chans[i].buf;

	if(!lpds)return;

	if(!b)return;
	b->GetStatus(&status);
	if(status & DSBSTATUS_PLAYING)
	{
		v*= (float)(chans[i].sound->origpitch);
		b->SetFrequency((DWORD)v);
	}
}

void SoundSystem::SetPan(int i,float v)
{
	DWORD status;
	LPDIRECTSOUNDBUFFER b = chans[i].buf;

	if(!b || chans[i].buf3d)return;
	b->GetStatus(&status);
	if(status & DSBSTATUS_PLAYING)
		b->SetPan((DWORD)(v*10000.0f));	// range -1 to 1
}

void SoundSystem::Stop(int i)
{
	DWORD status;
	LPDIRECTSOUNDBUFFER b = chans[i].buf;

	if(!lpds)return;

	if(!b)return;
	b->GetStatus(&status);
	if(status & DSBSTATUS_PLAYING)
		b->Stop();
	chans[i].muted=false;
}

int SoundSystem::Play(Sound * s,bool loop,float vol,float pan,float pitch)
{
	if(!lpds)return 0;
	int chan = GetFreeChannel(s->priority);


	if(chan<0)return -1;	// no channels left

	if(!s->loaded)return -2;	// sound not loaded

	chans[chan].buf3d=NULL;
	chans[chan].sound=s;
	chans[chan].muted=false;
	if(s->Play(chans+chan,loop,vol,pan,pitch))
		return chan;
	else
		return -3;
}



int SoundSystem::Play3D(Sound * s, bool loop,float x, float y, float z,
						float xv, float yv, float zv,float vol,float pitch)
{
	if(!lpds)return 0;
	int chan = GetFreeChannel(s->priority);

	if(chan<0)return -1;	// no channels left

	if(!s->loaded)return -2;	// sound not loaded


	chans[chan].muted=false;
	chans[chan].buf3d=NULL;
	chans[chan].sound=s;
	if(s->Play3D(chans+chan,loop,x,y,z,xv,yv,zv,vol,pitch))
		return chan;
	else
		return -3;	
}

void SoundSystem::SetPosition(int s, float x, float y, float z)
{
	if(!lpds)return;
	if(!chans[s].buf3d)return;

	chans[s].buf3d->SetPosition(x,y,z,DS3D_DEFERRED);
}

void SoundSystem::SetVelocity(int s,float xv, float yv, float zv)
{
	if(!lpds)return;
	if(!chans[s].buf3d)return;

	chans[s].buf3d->SetVelocity(xv,yv,zv,DS3D_DEFERRED);

}

void SoundSystem::SetListener(float x,float y,float z,
		float xv,float yv,float zv,
		float upx,float upy,float upz,
		float forwx,float forwy,float forwz)
{
	if(!lpds)return;
	if(listener)
	{
		listener->SetPosition(x,y,z,DS3D_DEFERRED);
		listener->SetVelocity(xv,yv,zv,DS3D_DEFERRED);

		listener->SetOrientation(forwx,forwy,forwz,	// front vector
			upx,upy,upz,	// up vector
			DS3D_DEFERRED);

		listener->CommitDeferredSettings();
	}
}

void SoundSystem::KillChan(int i)
{
	if(!lpds)return;
	if(chans[i].isdup)
	{
//		dprintf("releasing %lx",chans[i].buf);
		chans[i].buf->Release();
	}
	if(chans[i].buf3d)chans[i].buf3d->Release();
	chans[i].buf=NULL;
	chans[i].buf3d=NULL;
	chans[i].sound=NULL;
	chans[i].muted=false;
}

Sound * SoundSystem::GetSoundOnChannel(int i)
{
	if(!lpds)return NULL;
	if(!chans[i].buf || !chans[i].sound)return NULL;

	DWORD status;
	chans[i].buf->GetStatus(&status);
	if(status & DSBSTATUS_PLAYING)
		return chans[i].sound;
	return NULL;

}

Sound * SoundSystem::FindOrLoadSound(char *fn,bool pitchvary,bool load3d,int pri)
{
	SoundList *p;

	if(!lpds)return NULL;
	for(p=soundlist;p;p=p->next)
	{
		if(!strcmp(p->s.name,fn))return &(p->s);
	}

	p=new SoundList;
	if(p->s.Load(fn,pitchvary,load3d,pri))
	{
		p->next=soundlist;
		soundlist=p;
		return &(p->s);
	}
	else
	{
		delete p;
		return NULL;
	}
}

// used for the engine sounds etc.

Sound * SoundSystem::DuplicateOrLoadSound(char *fn,bool pitchvary,bool load3d,int pri)
{
	SoundList *p;
	Sound *s=NULL;

	if(!lpds)return NULL;

	for(p=soundlist;p;p=p->next)
	{
		if(!strcmp(p->s.name,fn))
		{
			s= &(p->s);
			break;
		}
	}


	GetCaps();

	dprintf("free static hw bufs %ld free streaming hw bufs %ld",
		dscaps.dwFreeHw3DStaticBuffers,
		dscaps.dwFreeHw3DStreamingBuffers);
			

	p=new SoundList;

	dprintf("DuplicateOrLoadSound %s %s",fn,load3d ? "3D" : "NOT3D");
	bool dupped = false;

	if(s)
	{

		// if this sound has already been loaded, just make a copy of the existing sound
		// but duplicate the buffer.

		dprintf("sound found, duplicating");
		p->s= *s;	// default copy operator - just copies the memory.
		p->s.buf = s->Duplicate();	// duplicate the buffer
		if(p->s.buf)dupped=true;
	}

	if(!dupped)
	{
		dprintf("sound not found, loading");
		if(!p->s.Load(fn,pitchvary,load3d,pri))return NULL;
	}
	dprintf("sound loaded OK");

	if(load3d)
	{
		LPDIRECTSOUND3DBUFFER lp3Dbuf;
		HRESULT rv;

		dprintf("querying for 3D");
		rv=p->s.buf->QueryInterface(IID_IDirectSound3DBuffer,(LPVOID *)&(p->s.buf3d));

		dprintf("query OK");
		if FAILED(rv)
		{
			soundsys.Error("cannot get 3D interface");
			return NULL;
		}
	}
	else p->s.buf3d=NULL;

	dprintf("linking into list");
	p->next=soundlist;
	soundlist=p;
	dprintf("link OK, returning");
	return &(p->s);
}

void SoundSystem::StopAllSounds()
{
	// this deletes all sounds; channel and non-channel
	SoundList *p;
	if(!lpds)return;

	StopAllChannels();

	for(p=soundlist;p;p=p->next)
	{
		if(p->s.IsPlaying())	// to stop non-channel sounds
			p->s.Stop();
	}	
}


void SoundSystem::DeleteAllSounds()
{
	SoundList *p,*q;

	if(!lpds)return;

	StopAllChannels();

	for(p=soundlist;p;p=q)
	{
		q=p->next;

		if(p->s.IsPlaying())	// to stop non-channel sounds
			p->s.Stop();

		delete p;
	}	
	soundlist=NULL;
}

void SoundSystem::Mute(int i)
{
	DWORD status;

	if(!lpds)return;
	if(!chans[i].muted)
	{
		chans[i].buf->GetStatus(&status);
		if(status & DSBSTATUS_PLAYING)
			chans[i].buf->Stop();
		chans[i].muted=true;
		chans[i].sound->mutedcount++;
	}
}

void SoundSystem::Unmute(int i)
{
	if(!lpds)return;
	if(chans[i].muted)
	{
		chans[i].buf->Play(0,0,chans[i].flags);
		chans[i].muted=false;
		chans[i].sound->mutedcount--;
	}

}

void SoundSystem::StopAllChannels()
{
	int i;
	if(!lpds)return;
	for(i=0;i<NUMCHANS;i++)
	{
		if(chans[i].buf)
		{
			DWORD status;
			chans[i].buf->GetStatus(&status);
			if(status & DSBSTATUS_PLAYING)
				chans[i].buf->Stop();
			KillChan(i);
		}
	}
}

void SoundSystem::Dump()
{
	// debugging command - will dump the parameters of the listener and all active buffers


	dprintf("LISTENER PARAMETERS:");
	

	DS3DLISTENER lparms;

	lparms.dwSize = sizeof(DS3DLISTENER);

	if(listener->GetAllParameters(&lparms) == DS_OK)
	{
		dprintf("POS : %f %f %f    VEL : %f %f %f",
			(float)(lparms.vPosition.x),
			(float)(lparms.vPosition.y),
			(float)(lparms.vPosition.z),
			(float)(lparms.vVelocity.x),
			(float)(lparms.vVelocity.y),
			(float)(lparms.vVelocity.z));

		dprintf("FRN : %f %f %f    TOP : %f %f %f",
			(float)(lparms.vOrientFront.x),
			(float)(lparms.vOrientFront.y),
			(float)(lparms.vOrientFront.z),
			(float)(lparms.vOrientTop.x),
			(float)(lparms.vOrientTop.y),
			(float)(lparms.vOrientTop.z));

		dprintf("DIST : %f ROLLOFF : %f DOPPLER : %f",
			(float)(lparms.flDistanceFactor),
			(float)(lparms.flRolloffFactor),
			(float)(lparms.flDopplerFactor));
	}
	else
		dprintf("unable to get parameters");

	for(int i=0;i<NUMCHANS;i++)
	{
		if(chans[i].buf3d)
		{
			DS3DBUFFER bparms;

			bparms.dwSize = sizeof(DS3DBUFFER);

			dprintf("\nBUFFER %d PARAMETERS:");

			if(chans[i].buf3d->GetAllParameters(&bparms) == DS_OK)
			{
				dprintf("POS : %f %f %f    VEL : %f %f %f",
					(float)(bparms.vPosition.x),
					(float)(bparms.vPosition.y),
					(float)(bparms.vPosition.z),
					(float)(bparms.vVelocity.x),
					(float)(bparms.vVelocity.y),
					(float)(bparms.vVelocity.z));

				dprintf("INSIDECONE : %f   OUTSIDECONE : %f",
					(float)(bparms.dwInsideConeAngle),
					(float)(bparms.dwOutsideConeAngle));


				dprintf("MIND : %f   MAXD : %f",
					(float)(bparms.flMinDistance),
					(float)(bparms.flMaxDistance));
			}
		}
	}
}


// this is the streaming code. We can handle one streamed sound at a time.


void SoundSystem::RestartStream()
{
	if(soundsys.pendingstream)
	{
		dprintf("there's a pending stream, playing it");
		soundsys.PlayStreamBuffer(soundsys.pendingstream);
		soundsys.pendingstream=NULL;
	}
	else if(soundsys.repeatingstream)
	{
		dprintf("there's a repeating stream, playing it");
		soundsys.PlayStreamBuffer(soundsys.repeatingstream);
	}
	else dprintf("no pending or repeating stream");
}


CRITICAL_SECTION streamcritsec;

bool streamthreadrunning=false;

void ProcessStreamFadeThread()
{
	while(streamthreadrunning)
	{
		Sleep(20);
		EnterCriticalSection(&streamcritsec);
		if(soundsys.stream.lpdsNotify)
		{
			if(soundsys.faderate)
			{
				soundsys.fadelevel -= soundsys.faderate;
				if(soundsys.fadelevel<0)soundsys.fadelevel=0;

				DWORD v = (soundsys.fadelevel-1) * 10000.0f;
				soundsys.stream.lpdsb->SetVolume(v);

				if(soundsys.fadelevel<=0)
				{
					// stream faded to silence

					soundsys.stream.lpdsb->Stop();
					soundsys.stream.lpdsNotify->Release();
					soundsys.stream.lpdsNotify=NULL;
					soundsys.stream.lpdsb->Release();
					soundsys.stream.lpdsb=NULL;
				}
			}
			LeaveCriticalSection(&streamcritsec);

			// check events

			DWORD evt;

			evt = WaitForMultipleObjects(SOUND_NUMEVENTS,soundsys.stream.rghEvent,FALSE,0);
			if(evt!=WAIT_TIMEOUT)soundsys.StreamToBuffer(evt);

		}
		else
		{
			LeaveCriticalSection(&streamcritsec);
//			RestartStream();
			}
	}
#if !defined(UNDER_CE)
	_endthread();
#endif
}











typedef void (__cdecl *THREADFUNC)(void *);

void SoundSystem::InitialiseStreaming()
{
	fadelevel=0.0;
	faderate=0.0;
	pendingstream=NULL;repeatingstream=NULL;
	dprintf("beginning streaming");
	InitializeCriticalSection(&streamcritsec);
	memset(&stream,0,sizeof(stream));

	streamthreadrunning=true;
#if !defined(UNDER_CE)
	_beginthread((THREADFUNC)ProcessStreamFadeThread,0,NULL);
#endif
}

static char repeatingstreambuf[256];

BOOL SoundSystem::PlayStreamBuffer(LPSTR filename)
{
	dprintf("entering PlayStreamBuffer for %s",filename);
	EnterCriticalSection(&streamcritsec);

	dprintf("inside critsec");
	if(stream.lpdsNotify)
	{
		// something's playing - must stop it first, or wait for it to stop
		pendingstream=filename;
		dprintf("pending stream set");
		LeaveCriticalSection(&streamcritsec);
		return TRUE;
	}

	strcpy(repeatingstreambuf,filename);repeatingstream=repeatingstreambuf;

	WaveCloseReadFile(&stream.hmmio,&stream.pwfx);


    if (WaveOpenFile(filename, &stream.hmmio, &stream.pwfx, 
            &stream.mmckinfoParent,NULL) != 0)
	{
		dprintf("waveopenfile stream");
		LeaveCriticalSection(&streamcritsec);
        return FALSE;
	}
    if (WaveStartDataRead(&stream.hmmio, &stream.mmckinfoData, 
            &stream.mmckinfoParent) != 0)
	{
		dprintf("wavestartdataread stream");
		LeaveCriticalSection(&streamcritsec);
        return FALSE;
	}



    memset(&stream.dsbdesc, 0, sizeof(DSBUFFERDESC)); 
    stream.dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
    stream.dsbdesc.dwFlags = 0
			| DSBCAPS_CTRLVOLUME
            | DSBCAPS_GETCURRENTPOSITION2   // Always a good idea
            | DSBCAPS_GLOBALFOCUS         // Allows background playing
            | DSBCAPS_CTRLPOSITIONNOTIFY; // Needed for notification
 
    // The size of the buffer is arbitrary, but should be at least
    // two seconds, to keep data writes well ahead of the play
    // position.
 
    stream.dsbdesc.dwBufferBytes = stream.pwfx->nAvgBytesPerSec * 2;  
    stream.dsbdesc.lpwfxFormat = stream.pwfx; 
 
    if FAILED(lpds->CreateSoundBuffer(&stream.dsbdesc, &stream.lpdsb, NULL))
    {
		dprintf("createsoundbuffer stream");
        WaveCloseReadFile(&stream.hmmio, &stream.pwfx);
		LeaveCriticalSection(&streamcritsec);
        return FALSE; 
    }

	
	// setup events

    for (int i = 0; i < SOUND_NUMEVENTS; i++)
    {
        stream.rghEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (NULL == stream.rghEvent[i])
		{
			dprintf("createevent stream");

			LeaveCriticalSection(&streamcritsec);
			return FALSE;
		}
    }

    stream.rgdsbpn[0].dwOffset = 0;	//start of buf
    stream.rgdsbpn[0].hEventNotify = stream.rghEvent[0];
    stream.rgdsbpn[1].dwOffset = (stream.dsbdesc.dwBufferBytes/2);	// middle of buf
    stream.rgdsbpn[1].hEventNotify = stream.rghEvent[1];

    if FAILED(stream.lpdsb->QueryInterface(IID_IDirectSoundNotify, (VOID **)&stream.lpdsNotify))
	{
		dprintf("queryinterface stream");
		LeaveCriticalSection(&streamcritsec);
		return FALSE; 
	}

	if FAILED(stream.lpdsNotify->SetNotificationPositions(SOUND_NUMEVENTS, stream.rgdsbpn))
	{
		dprintf("setnotify stream");
        stream.lpdsNotify->Release();
		LeaveCriticalSection(&streamcritsec);
        return FALSE;
	}

	dprintf("playing");
	stream.lpdsb->Play(0,0,DSBPLAY_LOOPING);
	faderate = 0.0;
	fadelevel = 1.0;

	dprintf("leaving critsec");
	LeaveCriticalSection(&streamcritsec);

	dprintf("leaving PlayStreamBuffer");
	return TRUE;
}


BOOL SoundSystem::StreamToBuffer(DWORD dwPos)
{
    LONG            lNumToWrite;   
	DWORD           dwStartOfs;
    VOID            *lpvPtr1, *lpvPtr2;
	DWORD           dwBytes1, dwBytes2;
    UINT            cbBytesRead;
	static DWORD    dwStopNextTime = 0xFFFF; 

	if(!stream.lpdsb)
		return TRUE;

	dprintf("streaming...");

	EnterCriticalSection(&streamcritsec);
    if (dwStopNextTime == dwPos)   // All data has been played
	{
        stream.lpdsb->Stop();
		dwStopNextTime = 0xFFFF;
		stream.lpdsb->Release();
		stream.lpdsNotify->Release();
		stream.lpdsb=NULL;
		stream.lpdsNotify=NULL;
		LeaveCriticalSection(&streamcritsec);
		dprintf("all data played");
		RestartStream();
		return TRUE;
	} 
    
	if (dwStopNextTime != 0xFFFF)  // No more to stream, but keep playing to end of data
	{
		LeaveCriticalSection(&streamcritsec);
		dprintf("no more to stream");
		return TRUE;
	}

    if (dwPos == 0)
        dwStartOfs = stream.rgdsbpn[SOUND_NUMEVENTS - 1].dwOffset;
    else
        dwStartOfs = stream.rgdsbpn[dwPos-1].dwOffset;


    lNumToWrite = (LONG) stream.rgdsbpn[dwPos].dwOffset - dwStartOfs;
    if (lNumToWrite < 0) lNumToWrite += stream.dsbdesc.dwBufferBytes;

	static int seq=0;

    stream.lpdsb->Lock(dwStartOfs,       // Offset of lock start
                     lNumToWrite,      // Number of bytes to lock
                     &lpvPtr1,         // Address of lock start
                     &dwBytes1,        // Count of bytes locked
                     &lpvPtr2,         // Address of wrap around
                     &dwBytes2,        // Count of wrap around bytes
                     0);               // Flags

    WaveReadFile(stream.hmmio,                // File handle
                 dwBytes1,             // Number of bytes to get
                 (BYTE *) lpvPtr1,     // Destination   
                 &stream.mmckinfoData,        // File chunk info 
                 &cbBytesRead);        // Actual bytes read

    if (cbBytesRead < dwBytes1)        // Reached end of file
    {
        WaveCloseReadFile(&stream.hmmio, &stream.pwfx);

        memset((PBYTE)lpvPtr1 + cbBytesRead,
                (stream.dsbdesc.lpwfxFormat->wBitsPerSample==8) ? 128 : 0,
				dwBytes1 - cbBytesRead);
        dwStopNextTime = dwPos;
    }
 
    stream.lpdsb->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2);
	LeaveCriticalSection(&streamcritsec);

	dprintf("streaming OK");
	return TRUE;
}

void SoundSystem::CloseDownStreaming()
{
	dprintf("closing down streaming");
	streamthreadrunning=false;
	EnterCriticalSection(&streamcritsec);
	WaveCloseReadFile(&stream.hmmio,&stream.pwfx);

	if(stream.lpdsNotify)
	{
		stream.lpdsNotify->Release();
		stream.lpdsNotify=NULL;
	}
	if(stream.lpdsb)
	{
		stream.lpdsb->Release();
		stream.lpdsb=NULL;
	}
	LeaveCriticalSection(&streamcritsec);

	Sleep(200);
	dprintf("streaming closed down");

	DeleteCriticalSection(&streamcritsec);

}


BOOL SoundSystem::StopStream(DWORD fadetime)	// doesn't stop it, Process does that.
{
	dprintf("entering stream stop");
	if(!stream.lpdsb)return TRUE;

	dprintf("stop streaming");
		EnterCriticalSection(&streamcritsec);

	repeatingstream=NULL;

	if(!fadetime)
	{
		fadelevel = -0.001f;	// immediate stop
		faderate=1;
	}
	else
	{
		fadelevel = 1.0f;

		// work out how many ticks the fade will take

		float ft = fadetime;
		ft *= 60.0f / 1000.0f;
		faderate = 0.7f / ft;	// the amount the fade decreases each tick

	}
	LeaveCriticalSection(&streamcritsec);
	dprintf("streams closed down");
	return TRUE;
}

// Interface for the front end

char *FrontEndStream(int mode,DWORD p1,DWORD p2)
{
	static char buf[256],*q;
	extern void check_init_carsounds();

	switch(mode)
	{
	case 0:	// start new stream
		check_init_carsounds();
		if(soundsys.lpds)
		{
			if(!soundsys.PlayStreamBuffer((char *)p1))
				return "playstreambuffer failed";
		}
		break;
	case 1:	// stop stream in ms milliseconds. Any playstreambuffers will be queued until the stop.
		if(soundsys.lpds)soundsys.StopStream(p1);
		break;
	case 2:
		if(soundsys.lpds)
		{
			if(!soundsys.StreamToBuffer(p1))
				return "stream to buffer failed";
		}
		break;
	case 3:
		q = (soundsys.lpds) ? (char *)soundsys.stream.rghEvent : (char *)NULL;
		dprintf("returning %lx",q);
		return q;
	default:
		sprintf(buf,"undefined mode : %d",mode);
		return buf;
	}

	return NULL;
}
