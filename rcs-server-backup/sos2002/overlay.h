// Overlay.h: interface for the Overlay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OVERLAY_H__45FF8922_E6A6_11D2_8609_0080C86B40F3__INCLUDED_)
#define AFX_OVERLAY_H__45FF8922_E6A6_11D2_8609_0080C86B40F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define	CAM_INCAR	0
#define	CAM_OUT1	1
#define	CAM_OUT2	2
#define	CAM_OUT3	3
#define	CAM_OUT4	4
#define	CAM_BUMPER	5
#define CAM_PITSTOP 6

#define	CAM_MAX		5

extern int cameraview;

#define	MAXGLYPHS	32

struct overlayrect
{
	int x,y,w,h;
};

#define	RPMDIAL		0
#define	KPHDIAL		1
#define	TEMPDIAL	2
#define	FUELDIAL	3
#define	OILDIAL		4
#define HOURSDIAL	5
#define MINUTESDIAL 6
#define SECONDSDIAL 7

#define	NUMDIALS	8

struct dash
{
	// the textures for each part of the non-env mapped cockpit are called cockpit0, cockpit1 etc..
	int numcockpitparts;		// number of sections of cockpit (non-environmentally mapped)
	overlayrect cockpitrects[10];	// where they are in the original image
	int cockpitoffsetx,cockpitoffsety;
	int rotatex,rotatey;		// cockpit centre of rotation, for rolling relative to image origin

	// the textures for the env-mapped cockpit parts are called cockenv0,cockenv1 etc.
	int numcockpitpartsenv;		// as above, but for the environmentally mapped section of the cockpit view
	overlayrect cockpitrectsenv[10];

	int numwheelparts;			// number of steering wheel parts
	overlayrect wheelrects[10];		// wheel sections in original image
	DWORD wheeltex[10];
	int wheeloffsetx,wheeloffsety;	// where the wheel's centre is relative to the screen
	int wheelcentrex,wheelcentrey;	// where the wheel's centre is relative to the wheel image
	float wheelscale;

	float camposzmod;
	DWORD cocktex[10];
	DWORD cockenvtex[10];

	// needles for cockpit dials.

	int numneedles;				// the number of different types of needle
	DWORD needletex;

	struct
	{
		int x,y,w,h;			// needle image position within 'needle:3df-1555' bitmap
		int px,py;				// pivot point position within entire bitmap
		float u,v,uw,vh;		// UV coordinates within texture

	} needles[10];

	// shadows for the needles

	struct
	{
		int x,y,w,h;			// needle image position within 'needle:3df-1555' bitmap
		int px,py;				// pivot point position within entire bitmap
		float u,v,uw,vh;		// UV coordinates within texture
	} shadows[10];

	// the individual needles. Some of these may not be present, in which case the needle type is -1
	// the min-max angle values are angles in rads clockwise from the 12 o'clock position..

	struct
	{
		int needle,x,y;
		float minvalue,minangle,maxvalue,maxangle;
	} dials[10];

	bool hasDial;

	int Dialx,Dialy,Dialw,Dialh;
	int DialScX,DialScY;
	DWORD dialtex;
	DWORD dialneedletex;

	int mirrortype,mirrortype2;
	struct
	{
		int x,y,w,h;
	}mirrorcam,mirrorcam2;

	struct 
	{
		int x1,y1,x2,y2,x3,y3,x4,y4;
	}mirrorpos,mirrorpos2;

};

struct flag
{
	int x,y,w,h;
	float u,v,uw,vh;
};

/*
struct flag
{
	int x,y,w,h;
	float u,v,uw,vh;

	DWORD tex;
};
*/
#define	FLAG_BLACK			1
#define	FLAG_BLACKNWHITE	2
#define	FLAG_CHECK			4
#define	FLAG_YELLOW			8

#define	L_ENGLISH	0
#define	L_WELSH		1
#define	L_ITALIAN	2
#define	L_GERMAN		3
#define	L_FRENCH		4

#define	L_MAX	L_FRENCH

class Overlay  
{
public:
	Overlay();
	virtual ~Overlay();

	// this must be called after boris has been initialised. It will push the
	// archive for the given car, load the cockpit overlay and the parameters
	// for the dials, pop the car archive, and load the generic overlay data
	// from the global archive.

	void LoadCarOverlay(int idx,char *carname, bool playercar);
	void Load();
	void Render();			// this will render the glyphs in the order specified in overlay.cpp
	void Shutdown();		// delete the fonts
	void SetJudder();		// set the cockpit judder
	void FadeOut();			// fade out after race
	void DoPaper(bool demo);			// spin in the paper

	void Dprintf(int slot,char *s,...);
	void SwitchToDash(int i);
	void DrawDebugWidget(int x,int y,float height,char *title,float val,float minv,float maxv,float zero=0);


	font *font1,*font2,*font3,*font4,*font5,*cfont;
	int cx,cy;
	unsigned long showflags;

	bool UseDriverAids,qualmode;

	void DrawFlags();
	void DrawCarInfo();

	dash *curdash;

	//Start flag
	bool showstart;
	bool swingflag;
	float flagpos;
	void StartFlag();
	int language;

private:

	dash dashboards[12];
	flag flags[4][6];
	flag pole;

	//Car tyres, fuel, and oil information overlays
	flag number[10];
	flag percent;
	flag infosymbols[3];

	long alphabase;

	DWORD flagtexture;
	DWORD carinfotexture;
	DWORD pausedtexture;

	void LoadDial(dash *d,int i,char *name,class ParamFile &f);
	void RenderDial(int i,float val,int x,int y,float angleoffset,int rotatex,int rotatey,float judder=0.0);
	void RenderDialOut(int i,float val,int x,int y,float angleoffset,int rotatex,int rotatey,float judder=0.0);
	void DrawGeneric(int x,int y,int sx,int sy,int w,int h);
	void DrawGenericTri(int dx,int dy,int sx,int sy,int x1,int y1,int x2,int y2,int x3,int y3);
	void DrawRotatedGeneric(int x,int y,int w,int h,int srcx,int srcy,int srcw,int srch,int rx,int ry,float angle);
	void LoadFlags();

	void LoadCarInfo();
	void RenderCarInfoPercent(int x, int y, float pcnt);


	int cpjudder;
	int fadeout;
	float PaperRotate;

	DWORD generic[2][2];			// texture for the generic elements

	bool oktorun;


	struct
	{
		int x,y,w,h;
	} glyphs[MAXGLYPHS];	// the glyph table. A 'glyph' is a graphical element in an overlay.
							// My terminology. Sorry.
};

extern Overlay overlay;

#endif // !defined(AFX_OVERLAY_H__45FF8922_E6A6_11D2_8609_0080C86B40F3__INCLUDED_)
