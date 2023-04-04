/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:		wave.h
 *  Content:	wave header
 *
 ***************************************************************************/

/*
	$Header$
	$Log$
	Revision 1.0  2000-03-21 15:29:48+00  jjs
	Initial revision

	Revision 1.1  1998/12/02 12:05:25  jcf
	Initial revision

*/


#ifndef __WAVE_INCLUDED__
#define __WAVE_INCLUDED__
#include "windows.h"

#define WAVEVERSION 1

#ifndef ER_MEM
#define ER_MEM 				0xe000
#endif

#ifndef ER_CANNOTOPEN
#define ER_CANNOTOPEN 		0xe100
#endif

#ifndef ER_NOTWAVEFILE
#define ER_NOTWAVEFILE 		0xe101
#endif

#ifndef ER_CANNOTREAD
#define ER_CANNOTREAD 		0xe102
#endif

#ifndef ER_CORRUPTWAVEFILE
#define ER_CORRUPTWAVEFILE	0xe103
#endif

#ifndef ER_CANNOTWRITE
#define ER_CANNOTWRITE		0xe104
#endif

#ifdef __cplusplus
extern "C"
{
#endif

int WaveOpenFile(char *, HMMIO *, WAVEFORMATEX **, MMCKINFO *,unsigned long);
int WaveStartDataRead(HMMIO *, MMCKINFO *, MMCKINFO *);
int WaveReadFile(HMMIO, UINT, BYTE *, MMCKINFO *, UINT *);
int WaveCloseReadFile(HMMIO *, WAVEFORMATEX **);

int WaveCreateFile(char *, HMMIO *, WAVEFORMATEX *, MMCKINFO *, MMCKINFO *);
int WaveStartDataWrite(HMMIO *, MMCKINFO *, MMIOINFO *);
int WaveWriteFile(HMMIO, UINT, BYTE *, MMCKINFO *, UINT *, MMIOINFO *);
int WaveCloseWriteFile(HMMIO *, MMCKINFO *, MMCKINFO *, MMIOINFO *, DWORD);

int WaveLoadFile(char *, UINT *, DWORD *, WAVEFORMATEX **, BYTE **);
int WaveLoadFileFromMemory(
			char *data,                      // (IN)
			unsigned long mem_block_size,
			UINT *cbSize,                           // (OUT)
			DWORD *pcSamples,                       // (OUT)
			WAVEFORMATEX **ppwfxInfo,       // (OUT)
			BYTE **ppbData                          // (OUT)
			);

int WaveSaveFile(char *, UINT, DWORD, WAVEFORMATEX *, BYTE *);

int WaveCopyUselessChunks(HMMIO *, MMCKINFO *, MMCKINFO *, HMMIO *, MMCKINFO *, MMCKINFO *);
BOOL riffCopyChunk(HMMIO, HMMIO, const LPMMCKINFO);

#ifdef __cplusplus
}
#endif


#endif