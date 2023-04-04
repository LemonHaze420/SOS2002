// $Header$

// $Log$
// Revision 1.23  2000-09-25 16:05:01+01  img
// Snapshot when converting to Igor
//
// Revision 1.22  2000-09-13 12:04:07+01  img
// TrackDetails mostly working...
//
// Revision 1.21  2000-08-07 16:20:00+01  img
// Adding options screens...
//
// Revision 1.20  2000-07-20 14:49:40+01  img
// TrackScreen started
//
// Revision 1.19  2000-07-04 15:14:23+01  jjs
// Added japanese support.
//
// Revision 1.18  2000-07-04 12:59:18+01  img
// Permedia 3 support!
//
// Revision 1.17  2000-07-04 10:48:40+01  img
// More Abstraction
//
// Revision 1.16  2000-05-26 09:12:31+01  img
// Gotta be final version now...
//
// Revision 1.15  2000-05-04 16:24:58+01  jcf
// Only 2 static screens now.
//
// Revision 1.14  2000-05-04 10:53:33+01  img
// Changed text positions
//
// Revision 1.13  2000-04-28 11:09:18+01  jcf
// <>
//
// Revision 1.12  2000-04-26 10:08:55+01  img
// <>
//
// Revision 1.11  2000-04-07 14:37:12+01  img
// Added help support
//
// Revision 1.10  2000-04-03 14:51:38+01  img
// No change. Wouldn`t let me undo!
//
// Revision 1.9  2000-03-22 09:29:06+00  img
// <>
//
// Revision 1.8  2000-03-21 17:28:18+00  img
// Pre-loading more now...
//
// Revision 1.7  2000-03-09 10:35:09+00  img
// Use 512x512
//
// Revision 1.6  2000-03-08 10:32:34+00  img
// Temporarily use 3df backdrops
//
// Revision 1.5  2000-03-07 13:59:50+00  img
// Uses 5 pvr textures instead of 6 3dfs
//
// Revision 1.4  2000-03-06 13:03:11+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.3  2000-02-29 14:03:00+00  jjs
// Modified to work with Boris3.
//
// Revision 1.2  2000-02-18 16:14:29+00  img
// Pretty much complete for 18th
// _B_ack button implemented, Start button implemented, no crash when no memory card, Controller on FrontScreen and other minor changes.
//
// Revision 1.1  2000-01-19 17:53:09+00  img
// <>
//

/* A Screen entity holds the data for each er- screen.

   1/12/1999 Broadsword Interactive Ltd.

   Ian Gledhill */
#define D3D_OVERLOADS
#include "ConsoleFrontLib.h"

#if defined (UNDER_CE)
extern FrontEndObject *StartButton;
extern TextWord *StartText;
#endif

char *static_screens[5] = {NULL,NULL,NULL,NULL,NULL};
int static_screen_ct=0;

int Counter=0;

void InitStaticScreens()
{
	static_screens[0] = (char *)malloc(100000);
	static_screens[1] = (char *)malloc(100000);
	static_screens[2] = (char *)malloc(100000);
}

Screen::Screen()
{
	UseDefaultBackdrop = true;
	SetBarsHeight(45, 50);
}

Screen::~Screen()
{
}

void Screen::SetCounter(int NewCounter)
{
	Counter = NewCounter;
}

#define BACK 0.9999999999f
#define RBACK 0.000001f
#define NEARBACK 0.99f
#define RNEARBACK 1.0f/0.99f

#if defined(IGOR)
	// SNARK
#else
D3DTLVERTEX smallbackv[5][4]=
{
	{
	_D3DTLVERTEX(_D3DVECTOR(0.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(0.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
};
#endif
#if !defined(PROPER_FRONTEND)

#define LARGEVERTICES 8
D3DTLVERTEX largebackv[LARGEVERTICES][4]=
{
	{
	_D3DTLVERTEX(_D3DVECTOR(0.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(0.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(256.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(0.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(0.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,0.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,128.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,256.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,384.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,0.0f),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,1.0f,1.0f),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,512.0f,0.99999f),RBACK,0xffffffff,0xffffffff,0.0f,1.0f)
	},
};
#else

#define LARGEVERTICES 6
#define X1 1.0f/512.0f
#define X2 1.0f-1.0f/512.0f
#define Y1 1.0f/512.0f
#define Y2 1.0f-1.0f/512.0f
#define Xa1 1.0f/256.0f
#define Xa2 1.0f-1.0f/256.0f

#if defined(IGOR)
	// SNARK
#else
D3DTLVERTEX largebackv[LARGEVERTICES][4]=
{
	{
	_D3DTLVERTEX(_D3DVECTOR(0.0f,0.0f,BACK),RBACK,0xffffffff,0xffffffff,X1,Y1),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,0.0f,BACK),RBACK,0xffffffff,0xffffffff,X2,Y1),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,BACK),RBACK,0xffffffff,0xffffffff,X2,Y2),
	_D3DTLVERTEX(_D3DVECTOR(0.0f,256.0f,BACK),RBACK,0xffffffff,0xffffffff,X1,Y2)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(256.0f,0.0f,BACK),RBACK,0xffffffff,0xffffffff,X1,Y1),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,0.0f,BACK),RBACK,0xffffffff,0xffffffff,X2,Y1),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,BACK),RBACK,0xffffffff,0xffffffff,X2,Y2),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,BACK),RBACK,0xffffffff,0xffffffff,X1,Y2)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,0.0f,BACK),RBACK,0xffffffff,0xffffffff,Xa1,Y1),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,0.0f,BACK),RBACK,0xffffffff,0xffffffff,Xa2,Y1),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,256.0f,BACK),RBACK,0xffffffff,0xffffffff,Xa2,Y2),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,BACK),RBACK,0xffffffff,0xffffffff,Xa1,Y2)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(0.0f,256.0f,BACK),RBACK,0xffffffff,0xffffffff,X1,Y1),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,BACK),RBACK,0xffffffff,0xffffffff,X2,Y1),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,512.0f,BACK),RBACK,0xffffffff,0xffffffff,X2,Y2),
	_D3DTLVERTEX(_D3DVECTOR(0.0f,512.0f,BACK),RBACK,0xffffffff,0xffffffff,X1,Y2)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(256.0f,256.0f,BACK),RBACK,0xffffffff,0xffffffff,X1,Y1),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,BACK),RBACK,0xffffffff,0xffffffff,X2,Y1),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,512.0f,BACK),RBACK,0xffffffff,0xffffffff,X2,Y2),
	_D3DTLVERTEX(_D3DVECTOR(256.0f,512.0f,BACK),RBACK,0xffffffff,0xffffffff,X1,Y2)
	},
	{
	_D3DTLVERTEX(_D3DVECTOR(512.0f,256.0f,BACK),RBACK,0xffffffff,0xffffffff,Xa1,Y1),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,256.0f,BACK),RBACK,0xffffffff,0xffffffff,Xa2,Y1),
	_D3DTLVERTEX(_D3DVECTOR(640.0f,512.0f,BACK),RBACK,0xffffffff,0xffffffff,Xa2,Y2),
	_D3DTLVERTEX(_D3DVECTOR(512.0f,512.0f,BACK),RBACK,0xffffffff,0xffffffff,Xa1,Y2)
	}
};
#endif
#endif

void Screen::DrawFullScreen2D(DWORD _Background[], bool MoreTextures, DWORD Bars[], DWORD BarFill[])
{
	int i;
	static int OldTriCount;

	if (MoreTextures)
	{
		for(i=0;i<LARGEVERTICES;++i)
		{
#if defined(IGOR)
			OldTriCount = ((Render2D *)_Background)[i].tricount;
			((Render2D *)_Background)[i].Draw();
			((Render2D *)_Background)[i].tricount = OldTriCount;
#else
			renderSetCurrentMaterial(&(((Material *)(_Background))[i]));
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, largebackv[i],4,D3DDP_DONOTCLIP );
#endif
		}

		if (Bars)
		{
#if defined(IGOR)
			for (int i=0 ; i<2 ; i++)
			{
				OldTriCount = ((Render2D *)Bars)[i].tricount;
				((Render2D *)Bars)[i].Draw();
				((Render2D *)Bars)[i].tricount = OldTriCount;
				OldTriCount = ((Render2D *)BarFill)[i].tricount;
				((Render2D *)BarFill)[i].Draw();
				((Render2D *)BarFill)[i].tricount = OldTriCount;
			}
#else
			renderSetCurrentMaterial(&(((Material *)(Bars))[0]));
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, TopBar,4,D3DDP_DONOTCLIP );
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, TopFill,4,D3DDP_DONOTCLIP );
			renderSetCurrentMaterial(&(((Material *)(Bars))[1]));
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, BottomBar,4,D3DDP_DONOTCLIP );
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, BottomFill,4,D3DDP_DONOTCLIP );
#endif
		}
	}
	else
	{
		for(i=0;i<5;++i)
		{
#if defined(IGOR)
	// SNARK
#else
			renderSetCurrentMaterial(&(((Material *)(_Background))[i]));
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, smallbackv[i],4,D3DDP_DONOTCLIP );
#endif
		}
	}
}

void CreateBars(int Top, int Bottom, Render2D *_Bars, Render2D *_BarFill)
{
	_BarFill[0].AddRect(	0,0, 640, Top-10, 0.95f,
		2,2, 2,2, 0xFFFFFFFF);
	_BarFill[1].AddRect(	0,490-Bottom, 640, 480, 0.95f,
		2,2, 2,2, 0xFFFFFFFF);
	_Bars[0].AddRect(	0,Top-10,640,Top, 0.95f,
		0,55, 64, 64, 0xFFFFFFFF);
	_Bars[1].AddRect(	0,480-Bottom,640,490-Bottom, 0.95f,
		0,1, 64, 10, 0xFFFFFFFF);
}

void Screen::SetBarsHeight(int TopHeight, int BottomHeight)
{
	Heights[0] = TopHeight;
	Heights[1] = BottomHeight;

#if defined(IGOR)
	Bars[0].tricount = Bars[1].tricount = 0;

	if (Bars[0].tex1)
	{
		CreateBars(TopHeight, BottomHeight, Bars, BarFill);
	}
#else
	TopBar[0] = _D3DTLVERTEX(_D3DVECTOR(0.0f, Heights[0]-10.0f, NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 0.0f, 54.0f/64.0f);
	TopBar[1] = _D3DTLVERTEX(_D3DVECTOR(640.0f, Heights[0]-10.0f, NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 1.0f, 54.0f/64.0f);
	TopBar[2] = _D3DTLVERTEX(_D3DVECTOR(640.0f, (float)Heights[0], NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 1.0f, 1.0f);
	TopBar[3] = _D3DTLVERTEX(_D3DVECTOR(0.0f, (float)Heights[0], NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 0.0f, 1.0f);

	TopFill[0] = _D3DTLVERTEX(_D3DVECTOR(0.0f, 0.0f, NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 0.2f, 0.2f);
	TopFill[1] = _D3DTLVERTEX(_D3DVECTOR(640.0f, 0.0f, NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 0.2f, 0.2f);
	TopFill[2] = _D3DTLVERTEX(_D3DVECTOR(640.0f, Heights[0]-10.0f, NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 0.2f, 0.2f);
	TopFill[3] = _D3DTLVERTEX(_D3DVECTOR(0.0f, Heights[0]-10.0f, NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 0.2f, 0.2f);
	
	BottomBar[0] = _D3DTLVERTEX(_D3DVECTOR(0.0f, 480.0f-Heights[1], NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 0.0f, 1.0f/64.0f);
	BottomBar[1] = _D3DTLVERTEX(_D3DVECTOR(640.0f, 480.0f-Heights[1], NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 0.0f, 1.0f/64.0f);
	BottomBar[2] = _D3DTLVERTEX(_D3DVECTOR(640.0f, 490.0f-Heights[1], NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 0.0f, 10.0f/64.0f);
	BottomBar[3] = _D3DTLVERTEX(_D3DVECTOR(0.0f, 490.0f-Heights[1], NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 0.0f, 10.0f/64.0f);

	BottomFill[0] = _D3DTLVERTEX(_D3DVECTOR(0.0f, 490.0f-Heights[1], NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 0.2f, 0.2f);
	BottomFill[1] = _D3DTLVERTEX(_D3DVECTOR(640.0f, 490.0f-Heights[1], NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 0.2f, 0.2f);
	BottomFill[2] = _D3DTLVERTEX(_D3DVECTOR(640.0f, 480.0f, NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 0.2f, 0.2f);
	BottomFill[3] = _D3DTLVERTEX(_D3DVECTOR(0.0f, 480.0f, NEARBACK), RNEARBACK, 0xFFFFFFFF, 0xFFFFFFFF, 0.2f, 0.2f);
#endif
}

#if !defined (PROPER_FRONTEND)
extern bool ShowingStart;

void Screen::SetStartState(bool State)
{
	if (State)
	{
		StartButton->Draw();
		StartText->Write();
		ShowingStart = true;
	}
	else
	{
		StartButton->Hide();
		StartText->Hide();
		ShowingStart = false;
	}
}
#endif

extern int reallanguage;
#if defined(JAPAN)
extern int JapanHelp;
#endif

void Screen::SetHelpText(char *Text1, char *Text2)
{
#if defined(IGOR)
	// SNARK
#else
	HelpText[0] = new TextWord(Text1, MainAlphabet);
	HelpText[1] = new TextWord(Text2, MainAlphabet);

	HelpText[0]->Justify(JUSTIFY_LEFT); HelpText[1]->Justify(JUSTIFY_LEFT);
	if (reallanguage > 1)
	{
		HelpText[0]->Write(10,417,1800.0f);
		HelpText[1]->Write(10,429,1800.0f);
	}
	else
	{
		HelpText[0]->Write(10,417,1200.0f);
		HelpText[1]->Write(10,434,1200.0f);
	}
#endif
}

void Screen::DeleteHelp()
{
	delete HelpText[0];
	delete HelpText[1];
#if defined(JAPAN)
	JapanHelp = -1;
#endif
}

void Screen::ChangeHelpLine(int Line, bool State)
{
#if defined(IGOR)
	// SNARK
#else
	if (State)
		HelpText[Line]->Write();
	else
		HelpText[Line]->Hide();
#endif
}

Texture* backs[8];

void LoadBackdrop(Render2D *BackdropVariable, char *BackdropName, bool MoreTextures, Render2D *Bars, char *BarName, Render2D *BarFill)
{
	int i;
	char Filename[80];
	char ArchiveName[256];

#if !defined(PROPER_FRONTEND)
	sprintf(ArchiveName, DISKNAME"\\SOS1937FrontEnd\\Backdrops.tc", BackdropName);
	arcPush(ArchiveName);
#else
	sprintf(ArchiveName, FRONTEND_DATA "%s.tc", BackdropName);
#if defined(IGOR)
	Archive *BackdropArchive = NewArchive();
	BackdropArchive->Open(ArchiveName); 
	dprintf("Opened backdrop archive %s.\n", ArchiveName);
#else
	arcPush(ArchiveName);
#endif
#endif

	if (MoreTextures)
	{
#if !defined (PROPER_FRONTEND)
		for (i='a' ; i<='d' ;  i++)
		{
			sprintf(Filename, "%s%c.3df", BackdropName, i);
			backs[i-'a']= getTexturePtr(Filename,false,false);
		}
		for (i=0 ; i<4 ; i++)
		{
			sprintf(Filename, "%s%d.3df", BackdropName, i+2);
			backs[i+4]= getTexturePtr(Filename,false,false);
		}
#else
		for (i='a' ; i<='f' ; i++)
		{
			sprintf(Filename, "%s_%c", BackdropName, i);
#if defined(IGOR)
			backs[i-'a'] = new Texture;
			backs[i-'a'] = FindNewTexture(BackdropArchive, Filename, 0);
			backs[i-'a']->TGet(TEXTURE_CACHE_PRIORITY);
dprintf("Using backdrop texture 0x%x for %s.", backs[i-'a'], Filename);

#else
			backs[i-'a'] = getTexturePtr(Filename,false, false);
#endif
		}
#endif
		for(i=0;i<LARGEVERTICES;++i)
		{
#if defined(IGOR)
			BackdropVariable[i].tex1 = backs[i];
			BackdropVariable[i].tex2 = NULL;
			BackdropVariable[i].col = 0xffffffff;
			BackdropVariable[i].alpha = false;
			BackdropVariable[i].drawalways = true;
			BackdropVariable[i].flags = 0;
			BackdropVariable[i].tricount = BackdropVariable[i].numprimsinblock = 0;
			BackdropVariable[i].started = false;

			switch (i)
			{
			case 0:
			case 1:
				BackdropVariable[i].AddRect((i%3)*256, 0, (i%3)*256+256, (i/3)*256+256, 0.999f,
											0, 0, 255, 255, 0xffffffff);
				break;
			case 3:
			case 4:
				BackdropVariable[i].AddRect((i%3)*256, 256, (i%3)*256+256, 480, 0.999f,
											0, 0, 255, 224, 0xffffffff);
				break;
			case 2:
				BackdropVariable[i].AddRect(512, 0,	  640, 256, 0.97f,
											0,	 0,	  128, 256, 0xffffffff);
				break;
			case 5:
				BackdropVariable[i].AddRect(512, 256, 640, 480, 0.97f,
											0,	 0,	  128, 224, 0xffffffff);
				break;
			}

#else
			BackdropVariable[i].textureHandle = backs[i]->textureHandle;
#endif
		}

		if (Bars)
		{
#if defined(IGOR)
			Texture *BarTexture;
			sprintf(Filename, "%sTop_a", BarName);

			for (int i=0 ; i<2 ; i++)
			{
				BarTexture = new Texture;
				BarTexture = FindNewTexture(BackdropArchive, Filename, 0);
				BarTexture->TGet(TEXTURE_CACHE_PRIORITY);

				BarFill[i].tex1 = Bars[i].tex1 = BarTexture;
				BarFill[i].tex2 = Bars[i].tex2 = NULL;
				BarFill[i].col = Bars[i].col = 0xffffffff;
				BarFill[i].alpha = Bars[i].alpha = true;
				BarFill[i].drawalways = Bars[i].drawalways = true;
				BarFill[i].flags = Bars[i].flags = 0;
				BarFill[i].tricount = BarFill[i].numprimsinblock = Bars[i].tricount = Bars[i].numprimsinblock = 0;
				BarFill[i].started = Bars[i].started = false;

				sprintf(Filename, "%sBottom_a", BarName);
			}

			CreateBars(45, 50, Bars, BarFill);
#else
			sprintf(Filename, "%sTop_a.3df", BarName);
			Texture *BarTexture = getTexturePtr(Filename, false, false, false);
			Bars[0].textureHandle = BarTexture->textureHandle;
			Bars[0].flags = 0;
			sprintf(Filename, "%sBottom_a.3df", BarName);
			BarTexture = getTexturePtr(Filename, false, false, false);
			Bars[1].textureHandle = BarTexture->textureHandle;
			Bars[1].flags = 0;
#endif
		}
	}
	else
	{
		for (i=0 ; i<5 ; i++)
		{
			sprintf(Filename, "%s%d.3df", BackdropName, i+1);
#if defined(IGOR)
	// SNARK
#else
			backs[i]= getTexturePtr(Filename,false,false);
#endif		
		}
		for(i=0;i<5;++i)
		{
#if defined(IGOR)
	// SNARK
#else
			BackdropVariable[i].textureHandle = backs[i]->textureHandle;
			BackdropVariable[i].flags = 0;
#endif
		}
	}

#if defined(IGOR)
	BackdropArchive->Close();
#else
	arcPop();
#endif
}

void Screen::SetDelay(int _Delay)
{
	Delay = _Delay;
}
