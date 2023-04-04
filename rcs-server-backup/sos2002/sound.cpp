// Sound.cpp: implementation of the Sound class.
//
//////////////////////////////////////////////////////////////////////

#include "BS2All.h"
#include "Sound.h"
#include "wave.h"
#include "carsound.h"
#include "initguid.h"
#include "errorlib.h"

#include "eax.h"

/*
	$Header$
	$Log$
	Revision 1.0  2000-02-21 12:34:30+00  jjs
	Initial revision

	Revision 1.4  1998/12/02 12:17:07  jcf
	Added Id properly again...

	Revision 1.3  1998/12/02 12:03:50  jcf
	Added Id properly ;)

	Revision 1.2  1998/12/02 12:01:16  jcf
	Added ID and Log.

*/
static const char *rcs_id = "$Id$";

#define	DBTABLESIZE	1000
DWORD dbtab[DBTABLESIZE+1];
extern void dprintf(char *,...);


void generate_db_table()
{
	float q;
	for(int i=0;i<DBTABLESIZE;i++)
	{
		float g = i*(1/(float)DBTABLESIZE);
		q = 10*log10(g);

		dbtab[i] = q*100;
	}
	dbtab[i]= q*100;	// fill in last value
}
extern bool Paused;


static DWORD __inline db_from_gain(float g)
{
	// do a linear interpolating table lookup

	float i;

	if(Paused)return -10000;

	i=g*DBTABLESIZE;

	// i is now a non-integral index into the table

	int ii=(int)i;
	float a,b;

	a=dbtab[ii];	// get the two values either side
	b=dbtab[ii+1];

	i -= (float)ii;	// get the fractional part

	// work out the table value

	i = a*i + b*(1.0f-i);

	return (DWORD)i;
}


EAX_REVERBPROPERTIES default_reverb_presets[] =
{
	{EAX_PRESET_PLAIN},
	{EAX_PRESET_FOREST},
	{EAX_PRESET_MOUNTAINS},
	{EAX_PRESET_CITY}
};

EAX_REVERBPROPERTIES reverb_presets[] =
{
	{EAX_PRESET_PLAIN},
	{EAX_PRESET_CONCERTHALL},
	{EAX_PRESET_QUARRY},
	{EAX_PRESET_STONECORRIDOR},
};


/*
EAX_REVERBPROPERTIES default_reverb_presets[] =
{
	{EAX_PRESET_PLAIN},
	{EAX_PRESET_PSYCHOTIC},
	{EAX_PRESET_PSYCHOTIC},
	{EAX_PRESET_PSYCHOTIC}
};

EAX_REVERBPROPERTIES reverb_presets[] =
{
	{EAX_PRESET_PLAIN},
	{EAX_PRESET_PSYCHOTIC},
	{EAX_PRESET_PSYCHOTIC},
	{EAX_PRESET_PSYCHOTIC}
};
*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern int defaultreverb;


Sound::Sound()
{
	buf=NULL;
	buf3d=NULL;
	loaded=false;
	is3d=false;
	priority=0;
	mutedcount=0;
	volmult=1.0;
}

Sound::~Sound()
{
	Unload();
}










int WaveLoadFileArchive(
			char *pszFileName,                      // (IN)
			UINT *cbSize,                           // (OUT)
			DWORD *pcSamples,                       // (OUT)
			WAVEFORMATEX **ppwfxInfo,       // (OUT)
			BYTE **ppbData                          // (OUT)
			)
{

	// 1 - attempt to extract the sound file into a memory area.

	ulong handle = arcExtract_wav(pszFileName);

	*ppwfxInfo = NULL;
	if(!handle)return -1000;

	WaveLoadFileFromMemory(arcGetData(handle),arcGetSize(handle),cbSize,pcSamples,ppwfxInfo,ppbData);

	arcDispose(handle);

	return 0;
}









bool Sound::Load(char * fn,bool pitchvary,bool load3d,int pri)
{
#if !defined(UNDER_CE)
	if(buf)
	{
		buf->Release();
	}

	strcpy(name,fn);

	DWORD flags = DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN;
	priority=pri;

	if(pitchvary)flags |= DSBCAPS_CTRLFREQUENCY;
	if(load3d)
	{
		flags &= ~DSBCAPS_CTRLPAN;
		flags |= DSBCAPS_CTRL3D | DSBCAPS_MUTE3DATMAXDISTANCE ;
	}

/* if(soundsys.eax_on && load3d)
		flags |= DSBCAPS_LOCHARDWARE;
	else
		flags |= DSBCAPS_LOCSOFTWARE;
*/
	is3d=load3d;

	UINT	length;
	DWORD	noSamp;
	WAVEFORMATEX	*format;
	BYTE	*dataPtr;
	HRESULT dsrval;
	LPVOID ptr1,ptr2;
	DWORD len1,len2;
	int err;
	
	dprintf("attempting to load %s from file",name);

	char sbuf[256];
	sprintf(sbuf,".\\sounds\\%s",name);

	err=WaveLoadFile(sbuf, &length, &noSamp, &format, &dataPtr);
	
	if(!format)
	{
		// that's file not found, so let's try reading it from an archive.

		dprintf("not in file.. attempting to load %s from archive",name);
		err=WaveLoadFileArchive(name, &length, &noSamp, &format, &dataPtr);

		if(!format)
		{
			soundsys.Error("unable to load file");return false;
		}
	}
	
	DSBUFFERDESC DSBufferParams; INITSTRUCT(DSBufferParams);
	
	
	DSBufferParams.dwFlags = flags;
	DSBufferParams.dwBufferBytes = length; 
	DSBufferParams.lpwfxFormat = format;

	origpitch = format->nSamplesPerSec;

	dprintf("creating buffer: flags=%lx length=%ld cbsize=%d avgbps=%d block=%d chans=%d sps=%d bits/samp=%d tag=%d",
		flags,length,format->cbSize,format->nAvgBytesPerSec,
		format->nBlockAlign,format->nChannels,format->nSamplesPerSec,format->wBitsPerSample,format->wFormatTag);

	dsrval = soundsys.lpds->CreateSoundBuffer(&DSBufferParams, &buf, NULL);
	
	
	if (dsrval != DS_OK)
	{
		soundsys.DSError("LoadSecondary->Create", dsrval );
		buf=NULL;
		return false;
	}
	
	dprintf("locking buffer");
	dsrval = buf->Lock(0, length, &ptr1, &len1, &ptr2, &len2, 0);
	
	if (dsrval != DS_OK)
	{
		dprintf("lock failed ,attempting restore");
		if (soundsys.DSError("LoadSecondary->Lock", dsrval ))
		{
			dsrval = buf->Restore();
			dsrval = buf->Lock(0, length, &ptr1, &len1, &ptr2, &len2, 0);
			if (dsrval != DS_OK)
			{
				soundsys.DSError("Buffer restored, LoadSecondary->Lock", dsrval);
				buf=NULL;
				return false;
			}
		}
	}
	
	dprintf("copying data");
	memcpy(ptr1, dataPtr, length);
	dprintf("freeing ptrs");
	GlobalFree(dataPtr);
	GlobalFree(format);
	
	dprintf("unlocking");
	buf->Unlock(ptr1, len1, ptr2, len2);
	dprintf("unlocked OK");
	if(!soundsys.listener)is3d=false;	// turn off 3D if no listener...


	// EAX code - here we use this buffer to get the EAX interface


	if(soundsys.eax_on && !soundsys.goteax && load3d)
	{
		soundsys.goteax=true;	// even if this doesn't succeed we don't want to do it twice!
		soundsys.usesoftware=true;

		HRESULT rv;
		LPDIRECTSOUND3DBUFFER lp3Dbuf;

		dprintf("getting interface for EAX");
		dsrval = buf->QueryInterface(IID_IDirectSound3DBuffer,(LPVOID *)&lp3Dbuf);
		if(FAILED(dsrval))
		{
			soundsys.DSError("cannot get 3D buffer",dsrval);
		}

		dprintf("getting propset");
		dsrval = lp3Dbuf->QueryInterface(IID_IKsPropertySet,(void **)&soundsys.properties);

		ULONG support=0;

		if(SUCCEEDED(dsrval))
		{
			dprintf("querying props");
			if(FAILED(dsrval=soundsys.properties->QuerySupport(DSPROPSETID_EAX_ReverbProperties,
				DSPROPERTY_EAX_ALL,&support)))
			{
				soundsys.DSError("EAX not present [QS failed]",dsrval);
			}
			if((support & (KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET)) !=
					(KSPROPERTY_SUPPORT_GET|KSPROPERTY_SUPPORT_SET))
			{
				dprintf("EAX not supported");
			}
		}
		else
		{
			soundsys.DSError("unable to get property set",dsrval);
		}

		// let's set it

		dprintf("setting props");


		dsrval = soundsys.properties->Set(DSPROPSETID_EAX_ReverbProperties,
			DSPROPERTY_EAX_ALL,NULL,0,&default_reverb_presets[defaultreverb],sizeof(EAX_REVERBPROPERTIES));

		if(FAILED(dsrval))soundsys.DSError("properties->Set",dsrval);

		dprintf("EAX set and running OK");

	}

	loaded=true;
	
	dprintf("sound loaded OK");
#endif
	return true;
}

bool Sound::Play(Channel *c,bool loop,float vol,float pan,float pitch)
{
	LPDIRECTSOUNDBUFFER lpdsb = buf;

	
	// if this sound is already playing, duplicate the buffer and return that

	if(is3d)
	{
		soundsys.Error("cannot play 3D sound normally");return false;
	}

	if(IsPlaying())
	{
		if(!(lpdsb=Duplicate(c?false:true)))
			return false;

		if(c)c->isdup = true;
	}
	else if(c)c->isdup=false;


	// set the parameters

	pitch *= origpitch;	// pitch is ratio to original recorded pitch, so 1 is normal, 0.5 is half.. 	
	lpdsb->SetFrequency((DWORD)pitch);
	lpdsb->SetPan((DWORD)(pan*10000.0f));	// range -1 to 1

	vol *= volmult; if(vol>1.0f)vol=1.0f;

	lpdsb->SetVolume(db_from_gain(vol));	// range 0 to 1


	// begin playing the sound

	unsigned long flags=0L;
	if(loop)
		flags |= DSBPLAY_LOOPING;

	if(c)
	{
		c->buf = lpdsb;
		c->flags = flags;
	}

	if(! (c && c->muted))
	{
		HRESULT rv = lpdsb->Play(0,0,flags);

		if(SUCCEEDED(rv))
			return true;

		// failed ... try to restore again? 

		soundsys.Error("Attempting restore...");

		if(rv == DSERR_BUFFERLOST)rv = lpdsb->Restore();
		if(SUCCEEDED(rv))
			rv=lpdsb->Play(0,0,flags);
		return SUCCEEDED(rv);
	}
	else return true;
}

bool Sound::PlayNoDuplicate(Channel *c,bool loop,float vol,float pan,float pitch)
{
	if(!IsPlaying())return Play(c,loop,vol,pan,pitch);
	return true;
}

bool Sound::IsPlaying()
{
	DWORD status;

	if(!buf)return false;
	if(mutedcount)return true;
	buf->GetStatus(&status);
	if(status & DSBSTATUS_BUFFERLOST)
		buf->Restore();
	if(status & DSBSTATUS_PLAYING)
		return true;
	else
		return false;
}

LPDIRECTSOUNDBUFFER Sound::Duplicate(bool addtodups)	// should only be true for non-channel sounds
{
	HRESULT rv;
	LPDIRECTSOUNDBUFFER newbuf;

	rv=soundsys.lpds->DuplicateSoundBuffer(buf,&newbuf);
	if(rv!=DS_OK)
	{
		soundsys.DSError("Sound::Duplicate",rv);
		return NULL;
	}
	if(!newbuf)fatalError("weirdness in duplicate!");

	if(addtodups)
		soundsys.AddToDupList(newbuf);

	return newbuf;
}

bool Sound::Play3D(Channel *c, bool loop,float x, float y, float z,
				   float xv, float yv, float zv,float vol,float pitch)
{
	LPDIRECTSOUNDBUFFER lpdsb = buf;

	// if this sound is already playing, duplicate the buffer and return that

	if(!is3d)
	{
		if(c)c->buf3d = NULL;
		return Play(c,loop,vol,0,pitch);
	}

	if(IsPlaying())
	{
		if(!(lpdsb=Duplicate(c?false:true)))
			return false;

		if(c)c->isdup = true;
	}
	else if(c)c->isdup=false;


	// set the parameters

	// try to get the 3D buffer interface
	LPDIRECTSOUND3DBUFFER lp3Dbuf;
	HRESULT rv;

	rv=lpdsb->QueryInterface(IID_IDirectSound3DBuffer,(LPVOID *)&lp3Dbuf);

	if FAILED(rv)
	{
		soundsys.Error("cannot get 3D interface");
		return false;
	}

	lp3Dbuf->SetPosition(x,y,z,DS3D_IMMEDIATE);
	lp3Dbuf->SetVelocity(xv,yv,zv,DS3D_IMMEDIATE);

	pitch *= origpitch;	// pitch is ratio to original recorded pitch, so 1 is normal, 0.5 is half.. 	
	lpdsb->SetFrequency((DWORD)pitch);
	vol *= volmult; if(vol>1.0f)vol=1.0f;
	lpdsb->SetVolume(db_from_gain(vol));	// range 0 to 1
	lp3Dbuf->SetMinDistance(20,DS3D_IMMEDIATE);
	lp3Dbuf->SetMaxDistance(400,DS3D_IMMEDIATE);

	// begin playing the sound

	unsigned long flags=0L;
	if(loop)
		flags |= DSBPLAY_LOOPING;


	if(c)
	{
		c->buf = lpdsb;
		c->buf3d = lp3Dbuf;
		c->flags = flags;
	}

	buf3d = lp3Dbuf;

	if(!(c && c->muted))
	{
		rv = lpdsb->Play(0,0,flags);

		if(SUCCEEDED(rv))
			return true;

		// failed ... try to restore again? 

		if(rv == DSERR_BUFFERLOST)
		{
			OutputDebugString(TEXT("Requiring restore\n"));
			rv = lpdsb->Restore();
		}
		else
		{
			OutputDebugString(TEXT("Play failed, but buffer not lost\n"));
			soundsys.DSError("Play",rv);
			return false;
		}

		if(SUCCEEDED(rv))
		{
			OutputDebugString(TEXT("Restore succeeded\n"));
			rv=lpdsb->Play(0,0,flags);
			if(SUCCEEDED(rv))
			{
				OutputDebugString(TEXT("play succeeded\n"));
			}
			else
				soundsys.DSError("Play",rv);

			return SUCCEEDED(rv);
		}
		else
		{
			soundsys.DSError("Restore",rv);
			return false;
		}
	}
	else return true;
}

bool Sound::Play3DNoDuplicate(Channel *c, bool loop,float x, float y, float z,
				   float xv, float yv, float zv,float vol,float pitch)
{
	if(!IsPlaying())return Play3D(c,loop,x,y,z,xv,yv,zv,vol,pitch);
	return true;	// just pretend we did it
}


void Sound::Unload()
{
	dprintf("deleting sound %s",name);
	if(buf)
	{
		dprintf("stopping");
		buf->Stop();
		dprintf("releasing %lx",buf);
//		buf->Release();
		dprintf("release OK");
		if(buf3d)
		{
			dprintf("releasing 3D");
//			buf3d->Release();
			dprintf("3D released OK");
			buf3d=NULL;
		}
		buf=NULL;
	}
	dprintf("done.");
}


// the methods below should only be used for channel-less looping sounds

void Sound::SetVolume(float v)
{
	DWORD status;
	if(Paused)v=0.0;	// mwahahaha!

	v *= volmult; if(v>1.0f)v=1.0f;
	buf->GetStatus(&status);
	if(status & DSBSTATUS_PLAYING)
		buf->SetVolume(db_from_gain(v));	// range 0 to 1
}

void Sound::SetFrequency(float v)
{
	DWORD status;
	buf->GetStatus(&status);
	if(status & DSBSTATUS_PLAYING)
	{
		v*= (float)(origpitch);
		if(v<0.1f)v=0.1f;
		buf->SetFrequency((DWORD)v);
	}
}

void Sound::SetPan(float v)
{
	DWORD status;
	buf->GetStatus(&status);
	if(status & DSBSTATUS_PLAYING)
		buf->SetPan((DWORD)(v*10000.0f));	// range -1 to 1
}

void Sound::Stop()
{
	DWORD status;
	buf->GetStatus(&status);
	if(status & DSBSTATUS_PLAYING)
		buf->Stop();
}

void Sound::SetPosition(float x, float y, float z)
{
	if(!buf3d)return;

	buf3d->SetPosition(x,y,z,DS3D_DEFERRED);
}

void Sound::SetVelocity(float x, float y, float z)
{
	if(!buf3d)return;

	buf3d->SetVelocity(x,y,z,DS3D_DEFERRED);
}


// really ought to be in carsound.cpp, but I want to keep all the EAX code here

void CarSound::SetReverb(int revtype)
{
	HRESULT dsrval;
	
	if(soundsys.eax_on && revtype!=soundsys.oldrevtype)
	{
		if(revtype)
			dsrval = soundsys.properties->Set(DSPROPSETID_EAX_ReverbProperties,
			DSPROPERTY_EAX_ALL,NULL,0,&reverb_presets[revtype],sizeof(EAX_REVERBPROPERTIES));
		else
			dsrval = soundsys.properties->Set(DSPROPSETID_EAX_ReverbProperties,
			DSPROPERTY_EAX_ALL,NULL,0,&default_reverb_presets[defaultreverb],sizeof(EAX_REVERBPROPERTIES));
		if(FAILED(dsrval))
		{
			soundsys.DSError("SetReverb",dsrval);
		}

		soundsys.oldrevtype = revtype;
	}

}
