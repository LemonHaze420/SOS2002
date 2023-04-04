/*
	$Header$
	$Log$
	Revision 1.2  2002-08-07 14:51:15+01  sjr
	<>

	Revision 1.1  2002-07-22 10:06:34+01  jjs
	Test version of SOS2002

	Revision 1.0  2000-02-21 12:34:13+00  jjs
	Initial revision

	Revision 1.5  1999/04/30 16:23:07  jcf
	<>

	Revision 1.4  1999/03/23 10:59:44  jcf
	Start of week checkin

	Revision 1.3  1999/03/17 17:01:53  jcf
	First working threaded version

	Revision 1.2  1999/03/16 17:14:30  jcf
	<>

	Revision 1.1  1999/01/25 17:29:07  jcf
	Initial revision

*/

static const char *rcs_id="$Id$";

extern void dprintf(char *,...);
extern bool Paused;

#define	REPLAY_ON	1	

/****Thu Dec 10 16:02:59 1998*************(C)*Broadsword*Interactive****
 *
 * Controller/replay implementation
 *
 ***********************************************************************/

#define	FORCEFEEDBACK	1
#define DIRECTINPUT_VERSION 0x0700

#include <stdio.h>
#include <stdarg.h>
#include <dinput.h>
#if !defined(UNDER_CE)
#include <time.h>
#include <ctype.h>
#endif

#include <windows.h>

#include "BS2all.h"
#include "ALLcar.h"
 
#include "controller.h"
#include "frontend.h"
//#include "frontlib2.h"

#include "force.h"
#include "overlay.h"

extern car *cararray[],*playercar;
extern struct gamesetup *game;

// joystick code. Replay and controller code after this

#define  REGISTRY_DIRECTORY	"Software\\Hasbro Interactive\\Spirit of Speed 1937"

BOOL FAR PASCAL InitJoystickInput(LPCDIDEVICEINSTANCE pdinst, LPVOID pvRef);
BOOL ReacquireInput(void);
static bool joyispresent=false;	// this must be global
#if !defined(UNDER_CE)
LPDIRECTINPUTDEVICE7	TmpJoy;
LPDIRECTINPUTDEVICE7 JoyDev; 
LPDIRECTINPUTDEVICE7 KeyDev;

LPDIRECTINPUT7  g_lpdi;
#endif
static BOOL joystick_bad=TRUE;
#define dprintf(x)

#if !defined(UNDER_CE)

// convert a key name into a VK_ code

struct concodearray {
	char *name;int val;
};

concodearray concodesen[] =
{
	// non-keyboard controls
	{"xaxis",CONC_XAXIS},
	{"yaxis",CONC_YAXIS},
	{"(1)",CONC_1},
	{"(2)",CONC_2},
	{"(3)",CONC_3},
	{"(4)",CONC_4},
	{"(5)",CONC_5},
	{"(6)",CONC_6},
	{"(7)",CONC_7},
	{"(8)",CONC_8},
	
	{"up",DIK_UP},
	{"down",DIK_DOWN},
	{"left",DIK_LEFT},
	{"right",DIK_RIGHT},
	{"pageup",DIK_PRIOR},
	{"pagedown",DIK_NEXT},
	{"leftshift",DIK_LSHIFT},
	{"leftcontrol",DIK_LCONTROL},
	{"leftalt",DIK_LMENU},
	{"rightshift",DIK_RSHIFT},
	{"rightcontrol",DIK_RCONTROL},
	{"rightalt",DIK_RMENU},
	{"insert",DIK_INSERT},
	{"delete",DIK_DELETE},
	{"home",DIK_HOME},
	{"end",DIK_END},
	{"tab",DIK_TAB},
	{"return",DIK_RETURN},
	{"backspace",DIK_BACK},
	{"space",DIK_SPACE},
	{",",DIK_COMMA},
	{".",DIK_PERIOD},
	{"-",DIK_MINUS},
	{"=",DIK_EQUALS},
	{"[",DIK_LBRACKET},
	{"]",DIK_RBRACKET},
	{";",DIK_SEMICOLON},
	{"'",DIK_APOSTROPHE},
	{"`",DIK_GRAVE},
	{"\\",DIK_BACKSLASH},
	{"/",DIK_SLASH},
	
	
	{"decimal",DIK_DECIMAL},
	{"numpad0",DIK_NUMPAD0},
	{"numpad1",DIK_NUMPAD1},
	{"numpad2",DIK_NUMPAD2},
	{"numpad3",DIK_NUMPAD3},
	{"numpad4",DIK_NUMPAD4},
	{"numpad5",DIK_NUMPAD5},
	{"numpad6",DIK_NUMPAD6},
	{"numpad7",DIK_NUMPAD7},
	{"numpad8",DIK_NUMPAD8},
	{"numpad9",DIK_NUMPAD9},
	{"add",DIK_ADD},
	{"multiply",DIK_MULTIPLY},
	{"subtract",DIK_SUBTRACT},
	{"divide",DIK_DIVIDE},
	{"enter",DIK_NUMPADENTER},
	
	{"a",DIK_A},
	{"b",DIK_B},
	{"c",DIK_C},
	{"d",DIK_D},
	{"e",DIK_E},
	{"f",DIK_F},
	{"g",DIK_G},
	{"h",DIK_H},
	{"i",DIK_I},
	{"j",DIK_J},
	{"k",DIK_K},
	{"l",DIK_L},
	{"m",DIK_M},
	{"n",DIK_N},
	{"o",DIK_O},
	{"p",DIK_P},
	{"q",DIK_Q},
	{"r",DIK_R},
	{"s",DIK_S},
	{"t",DIK_T},
	{"u",DIK_U},
	{"v",DIK_V},
	{"w",DIK_W},
	{"x",DIK_X},
	{"y",DIK_Y},
	{"z",DIK_Z},
	{"0",DIK_0},
	{"1",DIK_1},
	{"2",DIK_2},
	{"3",DIK_3},
	{"4",DIK_4},
	{"5",DIK_5},
	{"6",DIK_6},
	{"7",DIK_7},
	{"8",DIK_8},
	{"9",DIK_9},
	
	{"f1",DIK_F1},
	{"f2",DIK_F2},
	{"f3",DIK_F3},
	{"f4",DIK_F4},
	{"f5",DIK_F5},
	{"f6",DIK_F6},
	{"f7",DIK_F7},
	{"f8",DIK_F8},
	{"f9",DIK_F9},
	{"f10",DIK_F10},
	{"f11",DIK_F11},
	{"f12",DIK_F12},
	{"f13",DIK_F13},
	{"f14",DIK_F14},
	{"f15",DIK_F15},
	
	{"NONE",CONC_NONE},
	
	{NULL,0}
};
	
concodearray concodeswe[] =
{
	// non-keyboard controls
	{"xaxis",CONC_XAXIS},
	{"yaxis",CONC_YAXIS},
	{"(1)",CONC_1},
	{"(2)",CONC_2},
	{"(3)",CONC_3},
	{"(4)",CONC_4},
	{"(5)",CONC_5},
	{"(6)",CONC_6},
	{"(7)",CONC_7},
	{"(8)",CONC_8},
	
	{"up",DIK_UP},
	{"down",DIK_DOWN},
	{"left",DIK_LEFT},
	{"right",DIK_RIGHT},
	{"pageup",DIK_PRIOR},
	{"pagedown",DIK_NEXT},
	{"leftshift",DIK_LSHIFT},
	{"leftcontrol",DIK_LCONTROL},
	{"leftalt",DIK_LMENU},
	{"rightshift",DIK_RSHIFT},
	{"rightcontrol",DIK_RCONTROL},
	{"rightalt",DIK_RMENU},
	{"insert",DIK_INSERT},
	{"delete",DIK_DELETE},
	{"home",DIK_HOME},
	{"end",DIK_END},
	{"tab",DIK_TAB},
	{"return",DIK_RETURN},
	{"backspace",DIK_BACK},
	{"space",DIK_SPACE},
	{",",DIK_COMMA},
	{".",DIK_PERIOD},
	{"-",DIK_MINUS},
	{"=",DIK_EQUALS},
	{"[",DIK_LBRACKET},
	{"]",DIK_RBRACKET},
	{";",DIK_SEMICOLON},
	{"'",DIK_APOSTROPHE},
	{"`",DIK_GRAVE},
	{"\\",DIK_BACKSLASH},
	{"/",DIK_SLASH},
	
	
	{"decimal",DIK_DECIMAL},
	{"numpad0",DIK_NUMPAD0},
	{"numpad1",DIK_NUMPAD1},
	{"numpad2",DIK_NUMPAD2},
	{"numpad3",DIK_NUMPAD3},
	{"numpad4",DIK_NUMPAD4},
	{"numpad5",DIK_NUMPAD5},
	{"numpad6",DIK_NUMPAD6},
	{"numpad7",DIK_NUMPAD7},
	{"numpad8",DIK_NUMPAD8},
	{"numpad9",DIK_NUMPAD9},
	{"add",DIK_ADD},
	{"multiply",DIK_MULTIPLY},
	{"subtract",DIK_SUBTRACT},
	{"divide",DIK_DIVIDE},
	{"enter",DIK_NUMPADENTER},
	
	{"a",DIK_A},
	{"b",DIK_B},
	{"c",DIK_C},
	{"d",DIK_D},
	{"e",DIK_E},
	{"f",DIK_F},
	{"g",DIK_G},
	{"h",DIK_H},
	{"i",DIK_I},
	{"j",DIK_J},
	{"k",DIK_K},
	{"l",DIK_L},
	{"m",DIK_M},
	{"n",DIK_N},
	{"o",DIK_O},
	{"p",DIK_P},
	{"q",DIK_Q},
	{"r",DIK_R},
	{"s",DIK_S},
	{"t",DIK_T},
	{"u",DIK_U},
	{"v",DIK_V},
	{"w",DIK_W},
	{"x",DIK_X},
	{"y",DIK_Y},
	{"z",DIK_Z},
	{"0",DIK_0},
	{"1",DIK_1},
	{"2",DIK_2},
	{"3",DIK_3},
	{"4",DIK_4},
	{"5",DIK_5},
	{"6",DIK_6},
	{"7",DIK_7},
	{"8",DIK_8},
	{"9",DIK_9},
	
	{"f1",DIK_F1},
	{"f2",DIK_F2},
	{"f3",DIK_F3},
	{"f4",DIK_F4},
	{"f5",DIK_F5},
	{"f6",DIK_F6},
	{"f7",DIK_F7},
	{"f8",DIK_F8},
	{"f9",DIK_F9},
	{"f10",DIK_F10},
	{"f11",DIK_F11},
	{"f12",DIK_F12},
	{"f13",DIK_F13},
	{"f14",DIK_F14},
	{"f15",DIK_F15},
	
	{"NONE",CONC_NONE},
	
	{NULL,0}
};

concodearray concodesfr[] =
{
	// non-keyboard controls
	{"xaxis",CONC_XAXIS},
	{"yaxis",CONC_YAXIS},
	{"(1)",CONC_1},
	{"(2)",CONC_2},
	{"(3)",CONC_3},
	{"(4)",CONC_4},
	{"(5)",CONC_5},
	{"(6)",CONC_6},
	{"(7)",CONC_7},
	{"(8)",CONC_8},
	
	{"up",DIK_UP},
	{"down",DIK_DOWN},
	{"left",DIK_LEFT},
	{"right",DIK_RIGHT},
	{"pageup",DIK_PRIOR},
	{"pagedown",DIK_NEXT},
	{"leftshift",DIK_LSHIFT},
	{"leftcontrol",DIK_LCONTROL},
	{"leftalt",DIK_LMENU},
	{"rightshift",DIK_RSHIFT},
	{"rightcontrol",DIK_RCONTROL},
	{"rightalt",DIK_RMENU},
	{"insert",DIK_INSERT},
	{"delete",DIK_DELETE},
	{"home",DIK_HOME},
	{"end",DIK_END},
	{"tab",DIK_TAB},
	{"return",DIK_RETURN},
	{"backspace",DIK_BACK},
	{"space",DIK_SPACE},
	{",",DIK_COMMA},
	{".",DIK_PERIOD},
	{")",DIK_MINUS},
	{"=",DIK_EQUALS},
	{"^",DIK_LBRACKET},
	{"$",DIK_RBRACKET},
	{"m",DIK_SEMICOLON},
	{"u grave",DIK_APOSTROPHE},
	{"2",DIK_GRAVE},
	{"\\",DIK_BACKSLASH},
	{"!",DIK_SLASH},
	
	
	{"decimal",DIK_DECIMAL},
	{"numpad0",DIK_NUMPAD0},
	{"numpad1",DIK_NUMPAD1},
	{"numpad2",DIK_NUMPAD2},
	{"numpad3",DIK_NUMPAD3},
	{"numpad4",DIK_NUMPAD4},
	{"numpad5",DIK_NUMPAD5},
	{"numpad6",DIK_NUMPAD6},
	{"numpad7",DIK_NUMPAD7},
	{"numpad8",DIK_NUMPAD8},
	{"numpad9",DIK_NUMPAD9},
	{"add",DIK_ADD},
	{"multiply",DIK_MULTIPLY},
	{"subtract",DIK_SUBTRACT},
	{"divide",DIK_DIVIDE},
	{"enter",DIK_NUMPADENTER},
	
	{"a",DIK_Q},
	{"z",DIK_W},
	{"e",DIK_E},
	{"r",DIK_R},
	{"t",DIK_T},
	{"y",DIK_Y},
	{"u",DIK_U},
	{"i",DIK_I},
	{"o",DIK_O},
	{"p",DIK_P},
	{"q",DIK_A},
	{"s",DIK_S},
	{"d",DIK_D},
	{"f",DIK_F},
	{"g",DIK_G},
	{"h",DIK_H},
	{"j",DIK_J},
	{"k",DIK_K},
	{"l",DIK_L},
	{"w",DIK_Z},
	{"x",DIK_X},
	{"c",DIK_C},
	{"v",DIK_V},
	{"b",DIK_B},
	{"n",DIK_N},
	{",",DIK_M},
	{"&",DIK_0},
	{"\202",DIK_1},
	{"\"",DIK_2},
	{"'",DIK_3},
	{"(",DIK_4},
	{"-",DIK_5},
	{"\212",DIK_6},
	{"_",DIK_7},
	{"\207",DIK_8},
	{"\205",DIK_9},
	
	{"f1",DIK_F1},
	{"f2",DIK_F2},
	{"f3",DIK_F3},
	{"f4",DIK_F4},
	{"f5",DIK_F5},
	{"f6",DIK_F6},
	{"f7",DIK_F7},
	{"f8",DIK_F8},
	{"f9",DIK_F9},
	{"f10",DIK_F10},
	{"f11",DIK_F11},
	{"f12",DIK_F12},
	{"f13",DIK_F13},
	{"f14",DIK_F14},
	{"f15",DIK_F15},
	
	{"NONE",CONC_NONE},
	
	{NULL,0}
};

concodearray concodesge[] =
{
	// non-keyboard controls
	{"xaxis",CONC_XAXIS},
	{"yaxis",CONC_YAXIS},
	{"(1)",CONC_1},
	{"(2)",CONC_2},
	{"(3)",CONC_3},
	{"(4)",CONC_4},
	{"(5)",CONC_5},
	{"(6)",CONC_6},
	{"(7)",CONC_7},
	{"(8)",CONC_8},
	
	{"up",DIK_UP},
	{"down",DIK_DOWN},
	{"left",DIK_LEFT},
	{"right",DIK_RIGHT},
	{"pageup",DIK_PRIOR},
	{"pagedown",DIK_NEXT},
	{"leftshift",DIK_LSHIFT},
	{"leftcontrol",DIK_LCONTROL},
	{"leftalt",DIK_LMENU},
	{"rightshift",DIK_RSHIFT},
	{"rightcontrol",DIK_RCONTROL},
	{"rightalt",DIK_RMENU},
	{"insert",DIK_INSERT},
	{"delete",DIK_DELETE},
	{"home",DIK_HOME},
	{"end",DIK_END},
	{"tab",DIK_TAB},
	{"return",DIK_RETURN},
	{"backspace",DIK_BACK},
	{"space",DIK_SPACE},
	{",",DIK_COMMA},
	{".",DIK_PERIOD},
	{"\251",DIK_MINUS},
	{"'",DIK_EQUALS},
	{"\201",DIK_LBRACKET},
	{"+",DIK_RBRACKET},
	{"\224",DIK_SEMICOLON},
	{"\204",DIK_APOSTROPHE},
	{"^",DIK_GRAVE},
	{"\\",DIK_BACKSLASH},
	{"-",DIK_SLASH},
	
	
	{"decimal",DIK_DECIMAL},
	{"numpad0",DIK_NUMPAD0},
	{"numpad1",DIK_NUMPAD1},
	{"numpad2",DIK_NUMPAD2},
	{"numpad3",DIK_NUMPAD3},
	{"numpad4",DIK_NUMPAD4},
	{"numpad5",DIK_NUMPAD5},
	{"numpad6",DIK_NUMPAD6},
	{"numpad7",DIK_NUMPAD7},
	{"numpad8",DIK_NUMPAD8},
	{"numpad9",DIK_NUMPAD9},
	{"add",DIK_ADD},
	{"multiply",DIK_MULTIPLY},
	{"subtract",DIK_SUBTRACT},
	{"divide",DIK_DIVIDE},
	{"enter",DIK_NUMPADENTER},
	
	{"q",DIK_Q},
	{"w",DIK_W},
	{"e",DIK_E},
	{"r",DIK_R},
	{"t",DIK_T},
	{"z",DIK_Y},
	{"u",DIK_U},
	{"i",DIK_I},
	{"o",DIK_O},
	{"p",DIK_P},
	{"a",DIK_A},
	{"s",DIK_S},
	{"d",DIK_D},
	{"f",DIK_F},
	{"g",DIK_G},
	{"h",DIK_H},
	{"j",DIK_J},
	{"k",DIK_K},
	{"l",DIK_L},
	{"y",DIK_Z},
	{"x",DIK_X},
	{"c",DIK_C},
	{"v",DIK_V},
	{"b",DIK_B},
	{"n",DIK_N},
	{"m",DIK_M},
	{"0",DIK_0},
	{"1",DIK_1},
	{"2",DIK_2},
	{"3",DIK_3},
	{"4",DIK_4},
	{"5",DIK_5},
	{"6",DIK_6},
	{"7",DIK_7},
	{"8",DIK_8},
	{"9",DIK_9},
	
	{"f1",DIK_F1},
	{"f2",DIK_F2},
	{"f3",DIK_F3},
	{"f4",DIK_F4},
	{"f5",DIK_F5},
	{"f6",DIK_F6},
	{"f7",DIK_F7},
	{"f8",DIK_F8},
	{"f9",DIK_F9},
	{"f10",DIK_F10},
	{"f11",DIK_F11},
	{"f12",DIK_F12},
	{"f13",DIK_F13},
	{"f14",DIK_F14},
	{"f15",DIK_F15},
	
	{"NONE",CONC_NONE},
	
	{NULL,0}
};

concodearray concodesit[] =
{
	// non-keyboard controls
	{"xaxis",CONC_XAXIS},
	{"yaxis",CONC_YAXIS},
	{"(1)",CONC_1},
	{"(2)",CONC_2},
	{"(3)",CONC_3},
	{"(4)",CONC_4},
	{"(5)",CONC_5},
	{"(6)",CONC_6},
	{"(7)",CONC_7},
	{"(8)",CONC_8},
	
	{"up",DIK_UP},
	{"down",DIK_DOWN},
	{"left",DIK_LEFT},
	{"right",DIK_RIGHT},
	{"pageup",DIK_PRIOR},
	{"pagedown",DIK_NEXT},
	{"leftshift",DIK_LSHIFT},
	{"leftcontrol",DIK_LCONTROL},
	{"leftalt",DIK_LMENU},
	{"rightshift",DIK_RSHIFT},
	{"rightcontrol",DIK_RCONTROL},
	{"rightalt",DIK_RMENU},
	{"insert",DIK_INSERT},
	{"delete",DIK_DELETE},
	{"home",DIK_HOME},
	{"end",DIK_END},
	{"tab",DIK_TAB},
	{"return",DIK_RETURN},
	{"backspace",DIK_BACK},
	{"space",DIK_SPACE},
	{",",DIK_COMMA},
	{".",DIK_PERIOD},
	{"'",DIK_MINUS},
	{"\215",DIK_EQUALS},
	{"\212",DIK_LBRACKET},
	{"+",DIK_RBRACKET},
	{"\225",DIK_SEMICOLON},
	{"\205",DIK_APOSTROPHE},
	{"\\",DIK_GRAVE},
	{"\\",DIK_BACKSLASH},
	{"-",DIK_SLASH},
	
	{"<",DIK_OEM_102},
	
	{"decimal",DIK_DECIMAL},
	{"numpad0",DIK_NUMPAD0},
	{"numpad1",DIK_NUMPAD1},
	{"numpad2",DIK_NUMPAD2},
	{"numpad3",DIK_NUMPAD3},
	{"numpad4",DIK_NUMPAD4},
	{"numpad5",DIK_NUMPAD5},
	{"numpad6",DIK_NUMPAD6},
	{"numpad7",DIK_NUMPAD7},
	{"numpad8",DIK_NUMPAD8},
	{"numpad9",DIK_NUMPAD9},
	{"add",DIK_ADD},
	{"multiply",DIK_MULTIPLY},
	{"subtract",DIK_SUBTRACT},
	{"divide",DIK_DIVIDE},
	{"enter",DIK_NUMPADENTER},
	
	{"q",DIK_Q},
	{"w",DIK_W},
	{"e",DIK_E},
	{"r",DIK_R},
	{"t",DIK_T},
	{"y",DIK_Y},
	{"u",DIK_U},
	{"i",DIK_I},
	{"o",DIK_O},
	{"p",DIK_P},
	{"a",DIK_A},
	{"s",DIK_S},
	{"d",DIK_D},
	{"f",DIK_F},
	{"g",DIK_G},
	{"h",DIK_H},
	{"j",DIK_J},
	{"k",DIK_K},
	{"l",DIK_L},
	{"z",DIK_Z},
	{"x",DIK_X},
	{"c",DIK_C},
	{"v",DIK_V},
	{"b",DIK_B},
	{"n",DIK_N},
	{"m",DIK_M},
	{"0",DIK_0},
	{"1",DIK_1},
	{"2",DIK_2},
	{"3",DIK_3},
	{"4",DIK_4},
	{"5",DIK_5},
	{"6",DIK_6},
	{"7",DIK_7},
	{"8",DIK_8},
	{"9",DIK_9},
	
	{"f1",DIK_F1},
	{"f2",DIK_F2},
	{"f3",DIK_F3},
	{"f4",DIK_F4},
	{"f5",DIK_F5},
	{"f6",DIK_F6},
	{"f7",DIK_F7},
	{"f8",DIK_F8},
	{"f9",DIK_F9},
	{"f10",DIK_F10},
	{"f11",DIK_F11},
	{"f12",DIK_F12},
	{"f13",DIK_F13},
	{"f14",DIK_F14},
	{"f15",DIK_F15},
	
	{"NONE",CONC_NONE},
	
	{NULL,0}
};

concodearray *concodes;
#endif
void SetConCodesArray(int l)
{
#if !defined(UNDER_CE)
	switch(l)
	{
	case L_ENGLISH:
		concodes = concodesen;
	case L_WELSH:
		concodes = concodeswe;
		break;
	case L_GERMAN:
		concodes = concodesge;
		break;
	case L_FRENCH:
		concodes = concodesfr;
		break;
	case L_ITALIAN:
		concodes = concodesit;
		break;
	}
#endif
}

int GetConCodeByName(char *name)
{
#if defined(UNDER_CE)
	return 0;
#else
	for(int i=0;;i++)
	{
		if(!concodes[i].name)break;
		if(!stricmp(name,concodes[i].name))return concodes[i].val;
	}

	return atoi(name);
#endif
}

char *GetConNameByCode(int c)
{
#if defined(UNDER_CE)
	return NULL;
#else
	for(int i=0;;i++)
	{
		if(!concodes[i].name)break;
		if(concodes[i].val == c)return concodes[i].name;
	}

	static char buf[32];
	sprintf(buf,"%d",c);
	return buf;
#endif
}


static int conarray[32];	// CON values, indexed by function (CON_ values)


// for example, LoadKeyDef("Throttle",CON_THROTTLE,"Up");

void LoadConDef(char *conname,int num,char *deflt)
{
	char buf[80],regname[64],*s;

	sprintf(regname,"Con-%s",conname);
#if 0
	GetRegistryValue(REGISTRY_DIRECTORY, regname, buf);
#endif
	if(1/*!strcmp(buf,"NULL")*/) s=deflt;
	else s=buf;

	conarray[num] = GetConCodeByName(s);
}

DIJOYSTATE js;
void PollKeys(char *buffer)
{
#if !defined(UNDER_CE)
	HRESULT hr;
	hr = KeyDev->GetDeviceState(256,(LPVOID)buffer);	

	if(FAILED(hr))
	{
		if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			hr = KeyDev->Acquire();
			if(FAILED(hr))
			{
				memset(buffer,0,256);
				return;
			}
			hr = KeyDev->GetDeviceState(sizeof(buffer),(LPVOID)&buffer);	
			if(FAILED(hr))
				dprintf("second getdevicestate attempt failed : %lx",hr);
		}
		else dprintf("getdevicestate failed : %lx",hr);
	}
#endif
}


// loads the input controller definitions from the registry
bool InitialiseInputs()
{
#if defined(UNDER_CE)
	return TRUE;
#else
	char buf[40];
#if 0
	GetRegistryValue(REGISTRY_DIRECTORY,"ControlMethod",buf);
#endif
	if(1)//!stricmp("keyboard",buf))
	{
		LoadConDef("throttle",CON_THROTTLE,"a");
		LoadConDef("brake",CON_BRAKE,"z");
		LoadConDef("left",CON_LEFT,",");
		LoadConDef("right",CON_RIGHT,".");
		LoadConDef("gearup",CON_GEARUP,"'");
		LoadConDef("geardown",CON_GEARDN,"/");
		LoadConDef("more",CON_MORE,"leftshift");
		LoadConDef("glance",CON_GLANCE,"space");
	}
	else
	{
		LoadConDef("throttle",CON_THROTTLE,"yaxis");
		LoadConDef("brake",CON_BRAKE,"yaxis");
		LoadConDef("left",CON_LEFT,"xaxis");
		LoadConDef("right",CON_RIGHT,"xaxis");
		LoadConDef("gearup",CON_GEARUP,"NONE");
		LoadConDef("geardown",CON_GEARDN,"NONE");
		LoadConDef("more",CON_MORE,"leftshift");
		LoadConDef("glance",CON_GLANCE,"space");
	}

	if(conarray[CON_LEFT]<0)return false;
	else return true;	// return true to inhibit forcefeedback
#endif
}

/*
	External function used by front end.

	Returns 0 if nothing is being pressed, or a CONC_ or DIK_ code. along with
	a name. The front end, when it gets a non-zero value, must set the registry value for
	that function with the number returned. For instance,

	Con-throttle : -2
	Con-brake: -2
	Con-left: -1
	Con-right: -1
	Con-gearup: 56
	...

	Note that paired items, such as throttle/brake, must both be set to the same value
	when the value is -ve (indicating an analog controller).
*/

// the gubbins - see the following function for the interface
int doGetAControl()
{
#if !defined(UNDER_CE)
	// first, poll keys

	char buf[256];
	PollKeys(buf);

	for(int i=0;i<256;i++)
	{
		if(buf[i] & 0x80)return i;
	}

	// hm. no keys, let's try the joystick

	HRESULT hRes;

	if (JoyDev)
	{
	JoyDev->Poll();
	hRes = JoyDev->GetDeviceState(sizeof(DIJOYSTATE), &js);

	for(i=0;i<8;i++)
	{
		if(js.rgbButtons[i] & 0x80)return CONC_1 + i;
	}

	if(abs(js.lX)>100)return CONC_XAXIS;	
	if(abs(js.lY-255)>100)return CONC_YAXIS;
	}
#endif
	return 0;
}

// pass a pointer to a string buffer
int GetAControl(char *name)
{
	int i;
	if(i=doGetAControl())
		strcpy(name,GetConNameByCode(i));
	return i;
}







bool force_feedback = false;
bool use_periodic=false;	// use the periodic 'rev buzz'. Slows things down?


LPDIRECTINPUTEFFECT force_effects[NUM_EFFECTS];

//
//	NOTE - revolting though it may seem, to save calls we actually call these routines from
//	CarSound::DoSurfaceSound. Sorry.
//

void StopEffects()
{
	if(force_feedback)
	{
		if(force_effects[EFF_TRIANGLE])force_effects[EFF_TRIANGLE]->Stop();
		if(force_effects[EFF_CONDITION])force_effects[EFF_CONDITION]->Stop();
	}
}



void Jolt(int i,DWORD intensity,float duration,DWORD direction)
{
	if(force_feedback && !Paused)
	{
		DIEFFECT diEffect;
		LPDIRECTINPUTEFFECT e = force_effects[i];
		if(!e)return;

		DWORD lDirection[] = {direction*100,0};
		DWORD dwAxes[2] = { DIJOFS_X, DIJOFS_Y };

		diEffect.dwSize = sizeof(diEffect);
		diEffect.dwGain = use_periodic ? intensity : intensity *2;
		diEffect.rglDirection = (long *)&lDirection;
		diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS; 
		diEffect.cAxes = 2; 
		diEffect.dwDuration = duration * DI_SECONDS;
		diEffect.rgdwAxes = (unsigned long *)(&dwAxes);

		HRESULT hr = e->SetParameters(&diEffect,DIEP_DIRECTION | DIEP_GAIN | DIEP_DURATION | DIEP_START);
		if(FAILED(hr))dprintf("jolt failed %lx",hr);
	}
}

void SetForceFeedback(long left,long right,long offset,float revs)
{
	HRESULT hr;
	DWORD stat;

	if(force_feedback)
	{
		DICONDITION diCondition;
		DIPERIODIC diPeriodic;

		DIEFFECT diEffect;

		left = right = 0;

		memset(&diCondition,0,sizeof(DICONDITION));
		diCondition.lNegativeCoefficient = left;
		diCondition.lPositiveCoefficient = right;
		diCondition.lOffset = offset;

		memset(&diEffect,0,sizeof(DIEFFECT));
		diEffect.dwSize = sizeof(DIEFFECT);
		diEffect.cbTypeSpecificParams = sizeof(DICONDITION);
		diEffect.lpvTypeSpecificParams = &diCondition;

		if(force_effects[EFF_CONDITION])
		{
			hr=force_effects[EFF_CONDITION]->SetParameters(&diEffect,DIEP_TYPESPECIFICPARAMS);

			if(FAILED(hr))fatalError("damper : setcond failed %lx",hr);


			force_effects[EFF_CONDITION]->GetEffectStatus(&stat);

			if(!(stat & DIEGES_PLAYING))
			{
				hr=force_effects[EFF_CONDITION]->Start(INFINITE,0);
			}
		}

		// now the periodic.

		if(use_periodic && force_effects[EFF_TRIANGLE])
		{
			diPeriodic.dwMagnitude = Paused ? 0 : (revs/4+500);

			diPeriodic.dwPeriod = (128.0f/revs) * DI_SECONDS;
			diPeriodic.lOffset = 0;
			diPeriodic.dwPhase = 0;	

			diEffect.cbTypeSpecificParams = sizeof(DIPERIODIC);
			diEffect.lpvTypeSpecificParams = &diPeriodic;

			hr=force_effects[EFF_TRIANGLE]->SetParameters(&diEffect,DIEP_TYPESPECIFICPARAMS);

			if(FAILED(hr))fatalError("periodic : setcond failed %lx",hr);

			force_effects[EFF_TRIANGLE]->GetEffectStatus(&stat);

			if(!(stat & DIEGES_PLAYING))
			{
				hr=force_effects[EFF_TRIANGLE]->Start(INFINITE,0);
				if(FAILED(hr))fatalError("periodic start failed %lx",hr);
			}
		}
	}
}

		



BOOL CALLBACK EnumerateEffectsFunc(LPCDIEFFECTINFO pdei,LPVOID pvRef)
{
#if !defined(UNDER_CE)
    LPDIRECTINPUTDEVICE7 lpdid = (LPDIRECTINPUTDEVICE7)pvRef;   // pointer to calling device
    LPDIRECTINPUTEFFECT  lpdiEffect;      // pointer to created effect
    DIEFFECT             diEffect;        // params for created effect
    DICONSTANTFORCE      diConstantForce; // type-specific parameters
	HRESULT hr;
	
	dprintf("got an effect: %d",DIEFT_GETTYPE(pdei->dwEffType));
	
	switch(DIEFT_GETTYPE(pdei->dwEffType))
	{
	/* Here you can extract information about support for the 
	effect type (from pdei), and tailor your effects 
	accordingly. For example, the device might not support
		envelopes for this type of effect. */
		
		
	case DIEFT_CONSTANTFORCE:
		
		// Create one or more constant force effects. 
		// For each you have to initialize a DICONSTANTFORCE 
		// and a DIEFFECT structure. 
		if(!force_effects[EFF_SOFT])
		{
			dprintf("creating constant forces");
			
			DICONSTANTFORCE diConstantForce;
			DIENVELOPE diEnvelope;
			
			DWORD    dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
			LONG     lDirection[2] = { 18000, 0 };

			diEffect.dwSize = sizeof(DIEFFECT); 
			diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS; 
			diEffect.dwDuration = 0.5 * DI_SECONDS;
			diEffect.dwSamplePeriod = 0;               // = default 
			diEffect.dwGain = DI_FFNOMINALMAX;         // no scaling
			diEffect.dwTriggerButton = DIEB_NOTRIGGER; // not a button response
			diEffect.dwTriggerRepeatInterval = 0;      // not applicable
			diEffect.cAxes = 2; 
			diEffect.rgdwAxes = (unsigned long *)(&dwAxes);
			diEffect.rglDirection = (long *)(&lDirection); 
			diEffect.lpEnvelope = &diEnvelope; 
			diEffect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
			diEffect.lpvTypeSpecificParams = &diConstantForce;
			
			diEnvelope.dwSize = sizeof(diEnvelope);
			diEnvelope.dwAttackLevel=0;
			diEnvelope.dwAttackTime=0;	// microseconds
			diConstantForce.lMagnitude = DI_FFNOMINALMAX;   // sustain level
			diEnvelope.dwFadeLevel=0;
			diEnvelope.dwFadeTime=200000;
			
			dprintf("soft jolt...");
			hr = lpdid->CreateEffect(pdei->guid,
				&diEffect,
				&(force_effects[EFF_SOFT]),
				NULL);
			if(FAILED(hr))dprintf("failed.. %lx",hr);
			
			diEnvelope.dwAttackLevel=0;
			diEnvelope.dwAttackTime=0;	// microseconds
			diConstantForce.lMagnitude = DI_FFNOMINALMAX;   // sustain level
			diEnvelope.dwFadeLevel=0;
			diEnvelope.dwFadeTime=0;
			
			dprintf("hard jolt...");
			hr = lpdid->CreateEffect(pdei->guid,
				&diEffect,
				&(force_effects[EFF_HARD]),
				NULL);
			if(FAILED(hr))dprintf("failed.. %lx",hr);
			
		}
		break;
		
	case DIEFT_CONDITION:
		{
			DICONDITION diCondition;
			
			DWORD    dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
			LONG     lDirection[2] = { 18000, 0 };
			
			diEffect.dwSize = sizeof(DIEFFECT); 
			diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS; 
			diEffect.dwDuration = 0.5 * DI_SECONDS;
			diEffect.dwSamplePeriod = 0;               // = default 
			diEffect.dwGain = DI_FFNOMINALMAX;         // no scaling
			diEffect.dwTriggerButton = DIEB_NOTRIGGER; // not a button response
			diEffect.dwTriggerRepeatInterval = 0;      // not applicable
			diEffect.cAxes = 2; 
			diEffect.rgdwAxes = (unsigned long *)(&dwAxes);
			diEffect.rglDirection = (long *)(&lDirection); 
			diEffect.lpEnvelope = NULL; 
			diEffect.cbTypeSpecificParams = sizeof(DICONDITION);
			diEffect.lpvTypeSpecificParams = &diCondition;

			diCondition.dwNegativeSaturation = 10000;
			diCondition.dwPositiveSaturation = 10000;
			diCondition.lDeadBand = 0;
			diCondition.lOffset = 0;
			diCondition.lNegativeCoefficient = 0;
			diCondition.lPositiveCoefficient = 0;

			if(pdei->guid == GUID_Damper)
			{
				dprintf("creating conditional forces");
			
				hr = lpdid->CreateEffect(pdei->guid,
					&diEffect,
					&(force_effects[EFF_CONDITION]),
					NULL);
			}
			if(FAILED(hr))dprintf("failed.. %lx",hr);
		}
			
		break;

	case DIEFT_PERIODIC:
		if(!force_effects[EFF_TRIANGLE])
		{
			DIPERIODIC diPeriodic;
			DWORD dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
			long lDirection[2] = {9000,0};
			
			diPeriodic.dwMagnitude = 10000;
			diPeriodic.dwPeriod = 100000;
			diPeriodic.lOffset = 0;
			diPeriodic.dwPhase = 0;	
			
			diEffect.dwSize = sizeof(diEffect);
			diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
			diEffect.dwDuration = INFINITE;
			diEffect.dwSamplePeriod = 0;
			diEffect.dwGain = DI_FFNOMINALMAX;
			diEffect.dwTriggerButton = DIEB_NOTRIGGER;
			diEffect.dwTriggerRepeatInterval = 0;
			diEffect.cAxes = 2;
			diEffect.rgdwAxes = (unsigned long *)(&(dwAxes[0]));
			diEffect.rglDirection=(long *)(&(lDirection[0]));
			diEffect.lpEnvelope = NULL;
			diEffect.cbTypeSpecificParams = sizeof(DIPERIODIC);
			diEffect.lpvTypeSpecificParams = &diPeriodic;
			
			
			if(pdei->guid == GUID_Triangle)
			{
				HRESULT hr;
				
				dprintf("creating triangle");
				hr = lpdid->CreateEffect(pdei->guid,
					&diEffect,
					&(force_effects[EFF_TRIANGLE]),
					NULL);
				
				if(FAILED(hr))
				{
					dprintf("failed %lx",hr);
				}
			}
			/*
			if(pdei->guid == GUID_Sine)
			{
				HRESULT hr;
				
				dprintf("creating sine");
				hr = lpdid->CreateEffect(pdei->guid,
					&diEffect,
					&(force_effects[EFF_SINE]),
					NULL);
				
				if(FAILED(hr))
				{
					dprintf("failed %lx",hr);
				}
			}
			*/
		}	
		break;
	default:
		break;
		
	}
	
    // And so on for other types of effect 
#endif
    return DIENUM_CONTINUE;
}

#if !defined(UNDER_CE)
void EnumerateEffects(LPDIRECTINPUTDEVICE7 lpdid2)
{
	HRESULT hr = lpdid2->EnumEffects(&EnumerateEffectsFunc,(void *)lpdid2,DIEFT_ALL);
	
	if(FAILED(hr))dprintf("enumerate DirectInput effects failed");
}
#endif

BOOL SetupJoyStick(HINSTANCE hInstance)
{
#if !defined(UNDER_CE)
	HRESULT        hr; 	
	DIPROPDWORD DeadZone;
	DIPROPRANGE diprg;
	HWND win = GetActiveWindow();

	hr = DirectInputCreateEx(hInstance, DIRECTINPUT_VERSION, 
        IID_IDirectInput7, (void**)&g_lpdi, NULL);
	
	if FAILED(hr) 
	{ 
		OutputDebugString("cannot create DINPUT interface\n");
		return FALSE;//oh arse
	} 

 
	//search all the devices to find a joystick

	if(force_feedback)
	{
		dprintf("looking for a force-feedback device");

		force_feedback = false;

		g_lpdi->EnumDevices(DIDEVTYPE_JOYSTICK, 
							 InitJoystickInput, g_lpdi, DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK); 

		if (!JoyDev)
		{
			OutputDebugString("No force-feedback device found\n");
		
			g_lpdi->EnumDevices(DIDEVTYPE_JOYSTICK, 
								 InitJoystickInput, g_lpdi, DIEDFL_ATTACHEDONLY); 

			if (!JoyDev)
			{
				OutputDebugString("No joystick Found\n");
				return FALSE;
			}
		}
		else
		{
			force_feedback = true;
			EnumerateEffects(JoyDev);
		}
	}
	else
	{
		dprintf("not looking for a force-feedback device");

		g_lpdi->EnumDevices(DIDEVTYPE_JOYSTICK, 
							 InitJoystickInput, g_lpdi, DIEDFL_ATTACHEDONLY); 

		if (!JoyDev)
		{
			OutputDebugString("No joystick Found\n");
			return FALSE;
		}
	}


	//we should now have a properly set up joystick
	//now set up the parameters returned by the joystick - xval
	diprg.diph.dwSize       = sizeof(diprg); 
	diprg.diph.dwHeaderSize = sizeof(diprg.diph); 
	diprg.diph.dwObj        = DIJOFS_X; 
	diprg.diph.dwHow        = DIPH_BYOFFSET;
	//set min and max values returned
	diprg.lMin              = -127; 
	diprg.lMax              = 127; 
 
	if(JoyDev->SetProperty(DIPROP_RANGE, &diprg.diph) 
                                != DI_OK) 
	{ 
		OutputDebugString("IDirectInputDevice::SetProperty(DIPH_RANGE) FAILED\n"); 
		JoyDev->Release(); 
		return FALSE; 
	} 

	//then the y axis
	diprg.diph.dwObj        = DIJOFS_Y; 
	diprg.lMin              = 0; 
	diprg.lMax              = 510; 

	if(JoyDev->SetProperty(DIPROP_RANGE, &diprg.diph) 
                                != DI_OK) 
	{ 
		OutputDebugString("IDirectInputDevice::SetProperty(DIPH_RANGE) FAILED\n"); 
		JoyDev->Release(); 
		return FALSE; 
	} 

	// then set dead zone for x
	DeadZone.diph.dwSize =	sizeof(DIPROPDWORD);
	DeadZone.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	DeadZone.diph.dwObj = DIJOFS_X;
	DeadZone.diph.dwHow = DIPH_BYOFFSET;

	DeadZone.dwData = 0500;	//25%
	
	if (JoyDev->SetProperty(DIPROP_DEADZONE, &DeadZone.diph)
		!= DI_OK)
   { 
      OutputDebugString("IDirectInputDevice::SetProperty(DIPH_DEADZONE) FAILED\n"); 
      JoyDev->Release(); 
      return FALSE; 
   } 

	//and y
		DeadZone.diph.dwSize =	sizeof(DIPROPDWORD);
	DeadZone.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	DeadZone.diph.dwObj = DIJOFS_Y;
	DeadZone.diph.dwHow = DIPH_BYOFFSET;

	DeadZone.dwData = 0500;//10%
	
	if (JoyDev->SetProperty(DIPROP_DEADZONE, &DeadZone.diph) != DI_OK)
	{ 
      OutputDebugString("IDirectInputDevice::SetProperty(DIPH_DEADZONE) FAILED\n"); 
      JoyDev->Release(); 
      return FALSE; 
	} 

	if (!ReacquireInput())
	{
		joystick_bad = TRUE;
		return FALSE;
	}

	joystick_bad = FALSE;
	OutputDebugString("got joystick ok\n");
#endif
	return TRUE;
}

BOOL FAR PASCAL InitJoystickInput(LPCDIDEVICEINSTANCE pdinst, LPVOID pvRef) 
{ 
#if !defined(UNDER_CE)
   LPDIRECTINPUT7 pdi;
   HRESULT hr;
   
   pdi = (LPDIRECTINPUT7)pvRef; 
   
   dprintf("Got %s %s",pdinst->tszInstanceName,pdinst->tszProductName);
   dprintf("%lx %lx %lx %lx",pdinst->guidFFDriver.Data1,pdinst->guidFFDriver.Data2,pdinst->guidFFDriver.Data3,pdinst->guidFFDriver.Data4);
 
   // create the DirectInput joystick device 
   if (pdi->CreateDeviceEx(pdinst->guidInstance, IID_IDirectInputDevice7,
                                 (LPVOID *)&JoyDev, NULL) != DI_OK) 
   { 
      OutputDebugString("IDirectInput::CreateDevice FAILED\n");
      return DIENUM_CONTINUE; 
   } 

   //set it's behaviour
   if (JoyDev->SetDataFormat(&c_dfDIJoystick) != DI_OK) 
   { 
      OutputDebugString("IDirectInputDevice::SetDataFormat FAILED\n"); 
      JoyDev->Release(); 
      return DIENUM_CONTINUE; 
   } 

   //done that - set coop level
   if(JoyDev->SetCooperativeLevel(GetActiveWindow(), 
      DISCL_EXCLUSIVE | DISCL_FOREGROUND) != DI_OK) 
   { 
      OutputDebugString("IDirectInputDevice::SetCooperativeLevel  FAILED\n"); 
      JoyDev->Release(); 
      return DIENUM_CONTINUE; 
   } 
#endif
   return DIENUM_STOP;
}


BOOL ReacquireInput(void) 
{
#if defined(UNDER_CE)
	return(TRUE);
#else
	//grab the joystick and cling on so no bugger else can play with it
    HRESULT hRes; 
 
    if (JoyDev) 
    { 
       // acquire the device 
       hRes = JoyDev->Acquire(); 
 
       if (SUCCEEDED(hRes)) 
       { 
          return TRUE; 
       } 
       else 
       { 
          // acquisition failed 

		   switch(hRes)
		   {
			   case DIERR_INVALIDPARAM:
					OutputDebugString("DIERR_INVALIDPARAM\n"); 
					break;
			   case DIERR_NOTINITIALIZED:
					OutputDebugString("DIERR_NOTINITIALIZED\n"); 
					break;
				case DIERR_OTHERAPPHASPRIO:
					OutputDebugString("DIERR_OTHERAPPHASPRIO\n"); 
					break;
				default:
					OutputDebugString("Unknown error\n");
					break;
		   }

          return FALSE; 
       } 
    } 
    else 
    { 
       // we don't have a current device 
       return FALSE; 
    } 
#endif
} 



// this reads the joystick axes and the button states

void ProcessJoystick(LocalController *con,bool analog_accel,bool analog_turn)
{
#if !defined(UNDER_CE)
    HRESULT                 hRes; 
    int i;
	static BOOL wasDown1 = FALSE, wasDown2 = FALSE;

	// this becomes true when something is received from the wheel,
	// indicating that it's being used.

	static bool gotsomething=false;

	int v,turn;
	unsigned char brake,throttle;

	if(joystick_bad)
	{
		joyispresent=false;
		return;
	}


	js.lY = 0;
	js.lX = 0;
	for(i=0;i<32;i++)
	{
		js.rgbButtons[i]=0;
	}

	JoyDev->Poll();

	hRes = JoyDev->GetDeviceState(sizeof(DIJOYSTATE), &js);

	if (hRes != DI_OK) 
	{ 
		//this shouldn't happen but...
		if(hRes == DIERR_INPUTLOST) 
			if (!ReacquireInput())
				return;
			else
			{
				ProcessJoystick(con,analog_accel,analog_turn);
			}
	}

	if(analog_turn)
	{
		if(conarray[CON_LEFT] == CONC_XAXIS)
			v = js.lX;
		else
			v = js.lY;

		con->turn = -(float)(con->wheel_curve[v+128]) / 129.0;	// impose wheel curve
	}

	if(analog_accel)
	{
		if(conarray[CON_THROTTLE] == CONC_XAXIS)
			v = js.lX;
		else
			v = js.lY;

		if (v <= 255)
			throttle = 255 - v;
		else
			throttle = 0;
		if (v >= 255)
			brake = v - 255;
		else
			brake = 0;

		con->throttle = ((float)throttle)/255.0;
		con->brake = ((float)brake)/255.0;

	}

	for(i=0;i<8;i++)
	{
		if(js.rgbButtons[i])break;
	}

	if(i!=8 && (conarray[CON_GEARUP] == CONC_NONE))
		conarray[CON_GEARUP] = CONC_1 + i;
	else if(i!=8 && (conarray[CON_GEARDN] == CONC_NONE) && (CONC_1+i != conarray[CON_GEARUP]))
		conarray[CON_GEARDN] = CONC_1 + i;
#endif
}






static long timebuf;
ReplayBufferManager replaybuffermanager;

#define	REPLAY_INTERVAL	10


Controller::Controller()
{
	replaybuffer=NULL;
	replaycount=0;
	pitkeys=0;
}


// Network controller

NetController::NetController() : Controller()
{
	type = CONTROLLER_NETWORK;
}


ctrlpckt NetController::GetControlPacket()
{
	packet.zero();
	return packet;
}



/// attempt to initialise DirectInput

void InitialiseDirectInput(void *instance,int ff)
{
#if !defined(UNDER_CE)
	for(int i=0;i<NUM_EFFECTS;i++)
		force_effects[i]=NULL;

	switch(ff)	// set up which level of force feedback we're having
	{
	case 0:
		force_feedback = false;
		use_periodic = false;
		break;
	case 1:
		force_feedback = true;
		use_periodic = false;
		break;
	case 2:
		force_feedback = true;
		use_periodic = true;
		break;
	}

	if(SetupJoyStick((HINSTANCE)instance))
	{
		dprintf("setting joystick true");
		joyispresent=true;
	}

	// create the keyboard device

	HRESULT hr;

	hr = g_lpdi->CreateDeviceEx(GUID_SysKeyboard,IID_IDirectInputDevice7,(LPVOID *)&KeyDev,NULL);
	if(FAILED(hr))fatalError("unable to get keyboard device - error code %lx",hr);

	hr = KeyDev->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))fatalError("unable to set keyboard format - error code %lx",hr);

	hr = KeyDev->SetCooperativeLevel(GetActiveWindow(),DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(hr))fatalError("unable to set keyboard coop mode - error code %lx",hr);

	hr = KeyDev->Acquire();
	//if(FAILED(hr))fatalError("unable to acquire keyboard - error code %lx",hr);	
#endif
}

void ShutdownDirectInput()
{
#if !defined(UNDER_CE)
	g_lpdi->Release();
#endif
}

// The LocalController class


void LocalController::SendControlPacket(ctrlpckt packet)
{
}

// Keyboard controller


#define	TURNDELTA 0.03f		// how quickly keyboard makes the wheels turn
#define TURNSPEEDUP 2.0f		// how much the SHIFT key speeds up the turning
#define TURNSPEEDDOWN 0.3f   // how much the ALT key slows down the turning
#define	TURNRETURNDELTA	0.1f	// how quickly they return after the turns keys are released
#define	THROTTLEDELTA 0.1f
#define	BRAKEDELTA 0.1f


LocalController::LocalController(char *keybuf)
{
	turn = 0;
	throttle=0;
	brake=0;
	dampedturn=0;
	gearupoldstate=false;
	geardownoldstate=false;
	gearupbutton = geardownbutton = -1;
	automatic = false;

	lastspinctr=0;
	keybuffer = keybuf;

	shiftstate=false;

	type = CONTROLLER_LOCAL;

	// build wheel curve


	for(int i=-128;i<128;i++)
	{
		float x = (float)i;

		x = (x*x)/128;
		if(i<0)x = -x;

		x*=0.98f;

		wheel_curve[i+128] = (char)x;
	}
}



bool LocalController::GetBooleanControlVal(int code)
{
	// the keybuffer will have already been filled with data by the call to PollKeys from main.cpp

	code = conarray[code];

	if(code==CONC_XAXIS)return abs(js.lX)>80;
	else if(code==CONC_YAXIS)return abs(js.lY)>80;
	else if(code>=CONC_1)
	{
		return (js.rgbButtons[code-CONC_1] & 0x80)? true : false;
	}
	else return keybuffer[code]&0x80;

}

void LocalController::BuildControlPacket()
{
	if(!keybuffer)return;	// return, because the DirectInput keybuffer has not yet been initialised

	packet.zero();

	float mult=1;
	
	bool analog_accel = false;
	bool analog_turn = false;
	
	if(conarray[CON_THROTTLE]<0)analog_accel=true;
	if(conarray[CON_LEFT]<0)analog_turn=true;
	
	analogthrottle=analog_accel;

	ProcessJoystick(this,analog_accel,analog_turn);

	shiftstate = GetBooleanControlVal(CON_MORE);

	gearupstate = GetBooleanControlVal(CON_GEARUP);
	geardownstate = GetBooleanControlVal(CON_GEARDN);
	glancestate = GetBooleanControlVal(CON_GLANCE);


	if(shiftstate) mult=TURNSPEEDUP;

	if ((car->wskid[0]>0.0f && car->wskid[1]>0.0f)||(car->wskid[2]>0.0f && car->wskid[3]>0.0f)) mult=2.0f;

	if(!analog_accel)
	{
		throttlestate = GetBooleanControlVal(CON_THROTTLE);
		brakestate = GetBooleanControlVal(CON_BRAKE);

		if(brakestate)
		{
			brake+=BRAKEDELTA;
			if(brake>1)brake=1;
		}
		else brake=0;
		
		if(throttlestate){throttle+=THROTTLEDELTA;if(throttle>1)throttle=1;}else throttle=0;

	}
	if(!analog_turn)
	{
		leftstate = GetBooleanControlVal(CON_LEFT);
		rightstate = GetBooleanControlVal(CON_RIGHT);
		float vel=car->velocity.Mag();
		float turnmax=1.0f-sqrt(vel)*0.12f;

		if(turnmax<0.1f) turnmax=0.1f;

		float rotslow=0.6f+turnmax*0.4f;
		
		if(shiftstate) {rotslow*=2.0f ; turnmax*=2.0f;}		
		
		if(turnmax>1.0f) turnmax=1.0f;
		if(rotslow>1.0f) rotslow=1.0f;


		if(leftstate)
		{
			if (turn<0)
			{
				turn+=TURNRETURNDELTA*mult*rotslow;
			}
			else
			{
				turn += TURNDELTA*mult*rotslow;
				if(turn>turnmax)turn =turnmax;
			}
		}
		else if(rightstate)
		{
			if (turn>0)
			{
				turn-=TURNRETURNDELTA*mult*rotslow;
			}
			else
			{
				turn -= TURNDELTA*mult*rotslow;
				if(turn<-turnmax)turn=-turnmax;
			}
		} 
		
		
		float remult=vel*0.05f;
		if (remult>2.0f) remult=2.0f;
		
		if(!(leftstate || rightstate))
		{
			if(turn>0)
			{
				turn-=TURNRETURNDELTA*mult*remult;
				if(turn<0)turn=0;
			}
			else
			{
				turn+=TURNRETURNDELTA*mult*remult;
				if(turn>0)turn=0;
			}
		}
	}

	if(!gearupoldstate && gearupstate)gearchange=1;
	else if(!geardownoldstate && geardownstate)gearchange=-1;
	else gearchange=0;

#define	Q(x)	((x)?"true":"false")


	packet.MakePacketData(throttle,brake,turn);

	lastspinctr++;
	car->lastchangedup++;

	if(automatic && car->curgear>=2 && !(car->gearchange))
	{
		if(!car->wspin && car->wskid[2]<0.0 && car->wskid[3]<0.0 && !car->flying[2] && !car->flying[3])
		{
			if(lastspinctr>1)
			{
				if (car->revs>car->uprevlim[car->curgear] && car->lastchangedup>10)      { lastspinctr=0;packet.flags = CF_GEARUP;car->lastchangedup=0;}
				if (car->revs<car->lorevlim[car->curgear] && car->curgear>2)      { lastspinctr=0;packet.flags = CF_GEARDN; }

			}
		}
	}

	if(gearchange>0)
	{
		car->lastchangedup=0;
		packet.flags|=CF_GEARUP;
	}
	else if(gearchange<0)
		packet.flags|=CF_GEARDN;

	if(automatic_gearup)
	{
		car->lastchangedup=0;
		packet.flags |= CF_GEARUP;
		automatic_gearup=false;
	}

	if(glancestate)packet.flags |= CF_REARGLANCE;

}



ctrlpckt LocalController::GetControlPacket()
{
	// assumes a packet has been build with BuildControlPacket
	Record(packet);
	SendControlPacket(packet);
	gearupoldstate=gearupstate;
	geardownoldstate=geardownstate;

	ctrlpckt p = packet;
	packet.flags &= ~(CF_GEARUP|CF_GEARDN);	// quickest way to fix a problem

	return p;
}





//////////////////// replay code

unsigned long replaymemoryused=0;

void replay::Save(HFILE h)
{
#if !defined(UNDER_CE)
	replaylist *p;

	dprintf("saving %d blocks",numblks);
#if REPLAY_ON
	_lwrite(h,(char *)&numblks,sizeof(numblks));
	for(p=head;p;p=p->next)
	{
		_lwrite(h,p->data,REPLAYBLOCKSIZE);
	}
#endif
#endif
}

void replay::Load(HFILE h)
{
#if !defined(UNDER_CE)
	if(head)fatalError("attempting to load a replay into an already existing replay buffer");
#if REPLAY_ON
	unsigned long blkct;
	_lread(h,&blkct,sizeof(blkct));
	dprintf("loading %d blocks",blkct);
	for(int i=0;i<blkct;i++)
	{
		dprintf("loading block %d",i);

		replaylist *p = addnewblock();
		_lread(h,p->data,REPLAYBLOCKSIZE);
	}
#endif
#endif
}


void replay::add(void *block,unsigned long size)
{
#if !defined(UNDER_CE)
	if(!head)
	{
		// nothing there yet, so create an initial block

		addnewblock();	// automatically assigns tail
	}

	if(sizeoftail+size > REPLAYBLOCKSIZE)
	{
		// this would overrun, so allocate a new block

		addnewblock();
	}

	memcpy(tail->data+sizeoftail,block,size);
	sizeoftail+=size;
#endif
}

bool replay::read(void *data,unsigned long size)
{
	if(!blk)blk=head;

	if(idx+size>REPLAYBLOCKSIZE)
	{
		blk=blk->next;
		if(!blk)
		{
			fatalError("overrun in replay buffer read");
			return false;
		}
		idx=0;
	}
	memcpy((void *)data,blk->data+idx,size);
	idx+=size;
	return true;
}





bool DoesReplayExist()
{
#if !defined(UNDER_CE)
	OFSTRUCT of;
	HFILE h;

	h=OpenFile(".\\savegames\\replay.dat",&of,OF_READ);

	if(h==HFILE_ERROR)return false;	// no file
	_lclose(h);
#endif
	return true;
}



void SaveReplayControllers(int numcars)
{
#if !defined(UNDER_CE)
	car *car;
	OFSTRUCT of;
	HFILE h;
	int plcar = 0;

	h=OpenFile(".\\savegames\\replay.dat",&of,OF_CREATE+OF_WRITE);
	_lwrite(h,(const char *)game,sizeof(*game));

	if(h!=HFILE_ERROR)
	{
		for(int i=0;i<numcars;i++)
		{
			car = cararray[i];
			if(car == playercar)
			{
				plcar=i;
			}
		}
		
		dprintf("writing out playercar=%d",plcar);
		_lwrite(h,(const char *)&plcar,sizeof(int));

		for(i=0;i<numcars;i++)
		{
			car = cararray[i];
			Controller *r = (Controller *)(car->controller);
			dprintf("writing controller %d",i);
			r->Save(h);
		}
	}
	_lclose(h);
#endif
}

void LoadReplayHeader(gamesetup *g)
{
#if !defined(UNDER_CE)
	OFSTRUCT of; 
	HFILE h;

	h=OpenFile(".\\savegames\\replay.dat",&of,OF_READ);

	if(h==HFILE_ERROR)return;	// no file
	_lread(h,(void *)g,sizeof(*g));
	_lclose(h);

	dprintf("replay header loaded, track %s, %d cars",g->trackname,g->numcars);
	for(int i=0;i<g->numcars;i++)
	{
		dprintf("car %d : %s",i,g->cars[i].carname);
	}
#endif
}



//
//	assumes the cars have already been created with replay controllers.
//	returns an integer which is the player car index in cararray.
//

int LoadReplayControllers(int numcars)
{
#if defined(UNDER_CE)
	return 0;
#else
	car  *c;
	OFSTRUCT of;
	HFILE h;
	int plcar=0;

	h=OpenFile(".\\savegames\\replay.dat",&of,OF_READ);

	if(h==HFILE_ERROR)return -1;	// no file
	_llseek(h,sizeof(*game),0);
	_lread(h,(void *)&plcar,sizeof(int));

	for(int i=0;i<numcars;i++)
	{
		c = cararray[i];

		if(c->controller->type == CONTROLLER_REPLAY)
		{
			dprintf("loading replay controller for car %d",i);

			ReplayController *r = (ReplayController *)(c->controller);
			r->Load(h);
		}
	}
	_lclose(h);
	return plcar;
#endif
}

#define	YROT_INTERVAL	10
#define	POSITION_INTERVAL	20
#define	PHYSDATA_INTERVAL	120

void Controller::Record(ctrlpckt &p)
{
	unsigned short f=0;
#if REPLAY_ON

	if(Paused)return;


	if(!(replaycount%YROT_INTERVAL))
		f |= REPF_YROTANDVEL;
	if(!(replaycount%POSITION_INTERVAL))
		f |= REPF_POSITION;
	if(!(replaycount&PHYSDATA_INTERVAL))
		f |= REPF_PHYSDATA;

	if(car->engdead)f|=REPF_BANG;

	f|= (pitkeys<<8);	// add the pitkeys in

	replaybuffer->add(&f,sizeof(f));	// save flags

	// now save the data blocks

	replaybuffer->add(&p,sizeof(p));	// the usual packet
	
	if(f & REPF_YROTANDVEL)
	{
		yrotblock blk;
		blk.yRot = car->yRotationAngle;
		blk.vel = car->velocity;
		blk.angvel = car->angvel;
		blk.revs = car->revs;

		replaybuffer->add(&blk,sizeof(blk));
	}
	if(f & REPF_POSITION)
	{
		positionblock blk;
		blk.maintransform = car->mainObject->transform;

		replaybuffer->add(&blk,sizeof(blk));
	}
	if(f & REPF_PHYSDATA)
	{
		replayphysblock blk;
		blk.fuel = car->fuel;
		blk.oil = car->oil;
		blk.tyresback = car->tyresback;
		blk.tyresfront = car->tyresfront;

		replaybuffer->add(&blk,sizeof(blk));
	}

	replaycount++;
#endif
}



//
//	Replay controller class
//


// Replay controller

ReplayController::ReplayController() : Controller()
{
	ct=0;
	type = CONTROLLER_REPLAY;
}


ctrlpckt ReplayController::GetControlPacket()
{
	long size;
	static ctrlpckt packet;

	packet.accbrake=0;
	packet.steer=0;
	packet.flags=0;
	
	// get the buffer size;

	// get the next packet and modify the car accordingly

	if(ct<replaycount)
	{

		// get the flags

		unsigned short f;
		replaybuffer->read(&f,sizeof(f));

		pitkeys = (f>>8);	// get the pitkeys out

		// get the packet

		replaybuffer->read(&packet,sizeof(packet));
		if(f & REPF_YROTANDVEL)
		{
			yrotblock blk;
			replaybuffer->read(&blk,sizeof(blk));
			car->yRotationAngle = blk.yRot;
			car->velocity = blk.vel;
			car->angvel = blk.angvel;
			car->revs = blk.revs;
		}

		if(f & REPF_POSITION)
		{
			// handle positional packet
			positionblock blk;
			replaybuffer->read(&blk,sizeof(blk));
			car->mainObject->transform = blk.maintransform;
			car->mainObject->inheritTransformations();
		}
		if(f & REPF_PHYSDATA)
		{
			replayphysblock blk;
			replaybuffer->read(&blk,sizeof(blk));
			car->oil = blk.oil;
			car->fuel = blk.fuel;
			car->tyresfront = blk.tyresfront;
			car->tyresback = blk.tyresback;
		}

		if(f & REPF_BANG)
		{
			car->blow_engine = true;
		}
		ct++;
	}
	else 
	{
		packet.flags |= CF_FINISH;
		Paused=true;
	}

	return packet;
}


void ReplayController::Load(HFILE h)
{
#if !defined(UNDER_CE)
	unsigned long q = _llseek(h,0,1);

	_lread(h,(void *)&replaycount,sizeof(int));
	replaybuffer->Load(h);
#endif
}

void Controller::Save(HFILE h)
{
#if !defined(UNDER_CE)
	dprintf("writing %ld bytes",replaycount*sizeof(replay));

	_lwrite(h,(const char *)&replaycount,sizeof(int));
	replaybuffer->Save(h);
#endif
}



