// Sound.h: interface for the Sound class.
//
//////////////////////////////////////////////////////////////////////

/*
	$Header$
	$Log$
	Revision 1.0  2000-03-21 15:19:43+00  jjs
	Initial revision

	Revision 1.3  1999/03/23 10:59:47  jcf
	Start of week checkin

	Revision 1.2  1999/01/25 17:27:12  jcf
	general check-in after minor mods

	Revision 1.1  1998/12/02 12:05:26  jcf
	Initial revision

*/

#if !defined(AFX_SOUND_H__01EA9506_35B1_11D2_9D7E_0080C86B40F3__INCLUDED_)
#define AFX_SOUND_H__01EA9506_35B1_11D2_9D7E_0080C86B40F3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdarg.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <math.h>

#define	INITSTRUCT(s)	{memset(&(s),0,sizeof((s)));(s).dwSize=sizeof((s));}
#define	NUMCHANS	32	// maximum number of sounds

#define SOUND_NUMEVENTS 2	// number of event objects


// a playing channel

struct Channel
{
	LPDIRECTSOUNDBUFFER buf;	// the buffer being played
	LPDIRECTSOUND3DBUFFER buf3d;	// 3d interface to above, or NULL if not 3d
	class Sound *sound;				// the Sound it came from
	bool isdup;					// true if the buffer is a duplicate of that in the Sound
	bool muted;
	bool locked;
	unsigned long flags;
	int pri;
};

// stream data

struct _streamdata
{
	LPDIRECTSOUNDBUFFER         lpdsb;
	LPDIRECTSOUNDNOTIFY         lpdsNotify;
	WAVEFORMATEX                *pwfx;
	HMMIO                       hmmio;
	MMCKINFO                    mmckinfoData, mmckinfoParent;
	DSBPOSITIONNOTIFY           rgdsbpn[SOUND_NUMEVENTS];
	HANDLE                      rghEvent[SOUND_NUMEVENTS];
	DSBUFFERDESC                dsbdesc;
};


class SoundSystem  
{

public:
	struct SoundList * soundlist;
	void KillChan(int i);
	LPDIRECTSOUND3DLISTENER listener;
	int GetFreeChannel(int priority);
	bool GetCaps();
	LPDIRECTSOUND	lpds;
	DSCAPS dscaps;
	Channel chans[NUMCHANS];
	int forcechannel;
	bool goteax;
	int oldrevtype;
	char error[128];
	void Error(char *s) { strcpy(error,s); }
	void CreateDS(void *hwnd);	
	void ReleaseDS(void);

	LPKSPROPERTYSET	properties;
	
	struct _streamdata stream;



	bool usesoftware,eax_on;
	Sound * GetSoundOnChannel(int i);
	void DeleteAllSounds();
	Sound * FindOrLoadSound(char *fn,bool pitchvary,bool load3d,int pri);
	Sound * DuplicateOrLoadSound(char *fn,bool pitchvary,bool load3d,int pri);
	bool DSError(char *place,HRESULT errVal);

	void SetListener(float x,float y,float z,
		float xv,float yv,float zv,
		float upx,float upy,float upz,
		float forwx,float forwy,float forwz);

	void SetVelocity(int s,float xv,float yv,float zv);
	void SetPosition(int s,float x,float y,float z);

	void ForceNextChannel(int s) { forcechannel=s; }

	int Play3D(Sound * s, bool loop,float x, float y, float z,
		float xv, float yv, float zv,float vol,float pitch);
	int Play(Sound * s,bool loop,float vol,float pan,float pitch);

	void SetVolume(int i,float v);
	void SetPan(int i,float v);
	void SetFrequency(int i,float v);
	void Stop(int i);

	void Mute(int i);
	void Unmute(int i);

	void Lock(int i) { chans[i].locked=true; }
	void Unlock(int i) {chans[i].locked=false; }

	void StopAllChannels();
	void StopAllSounds();
	void Shutdown();
	bool Initialise(void *hwnd,bool software=false,float rolloff=DS3D_DEFAULTROLLOFFFACTOR);
	bool Initialise(long hwnd,bool software=false,float rolloff=DS3D_DEFAULTROLLOFFFACTOR);
	char *GetError() { return error;}
	void Delete(class Sound *s);

	void AddToDupList(LPDIRECTSOUNDBUFFER buf);
	void AddToDupList3D(LPDIRECTSOUND3DBUFFER buf3d);
	void ScanDupsAndDeleteFinished();


	float						fadelevel;
	float						faderate;
	char *pendingstream;
	char *repeatingstream;

	void CloseDownStreaming();
	BOOL StreamToBuffer(DWORD dwPos);
	BOOL PlayStreamBuffer(LPSTR filename);
	BOOL StopStream(DWORD time);
	void ProcessStreamFade();
	void InitialiseStreaming();
	void RestartStream();
	void WaitUntilStopStreamsComplete();

	SoundSystem();
	virtual ~SoundSystem();

	void Dump();
};

extern SoundSystem soundsys;





class Sound  
{
	friend class SoundSystem;
	int priority;
	bool loaded;

	float origpitch;

public:

	LPDIRECTSOUNDBUFFER buf;

	LPDIRECTSOUNDBUFFER Duplicate(bool addtodups=false);

	LPDIRECTSOUND3DBUFFER buf3d;
	bool is3d;
	int mutedcount;
	char name[128];
	float volmult;	// default volume for this sound. Multiplied by volume passed to Play methods.
					// 1 by default.


	void Unload();

	bool Load(char *fn,bool pitchvary,bool load3d,int pri);
	bool IsPlaying();
	Sound();
	virtual ~Sound();

	bool Play(Channel *c,bool loop,float vol,float pan,float pitch);
	bool PlayNoDuplicate(Channel *c,bool loop,float vol,float pan,float pitch);
	bool Play3D(Channel *c,
			bool loop,float x,float y,float z,float xv,float yv,float zv,float vol,float pitch);
	bool Play3DNoDuplicate(Channel *c, bool loop,float x, float y, float z,
				   float xv, float yv, float zv,float vol,float pitch);
	void SetPosition(float x,float y,float z);
	void SetVelocity(float x,float y,float z);
	void SetVolume(float v);
	void SetFrequency(float v);
	void SetPan(float v);
	void Stop();

};

// list of sounds for the librarian

struct SoundList
{
	Sound s;
	struct SoundList *next;
};



#endif // !defined(AFX_SOUND_H__01EA9506_35B1_11D2_9D7E_0080C86B40F3__INCLUDED_)
