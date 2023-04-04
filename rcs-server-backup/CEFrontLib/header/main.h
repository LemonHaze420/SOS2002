// $Header$

// $Log$
// Revision 1.2  2000-07-04 17:08:24+01  jjs
// Added Japanese support.
//
// Revision 1.1  2000-02-29 08:44:45+00  jjs
// Added Header and Log lines
//

/* Main header file for the ConsoleFrontLib

	Broadsword Interactive Ltd.

	Author:		Ian Gledhill
	Date:		09/11/1999
	Rev:		0.01 */

// The constructor.
#define safe_delete(x) {if(x) delete x; x = NULL;}
#define safe_free(x) {if(x) free(x); x = NULL;}
extern "C" void DoFirmwareResetTest();

class ConsoleFrontLib
{
public:
	// The constructor.
	ConsoleFrontLib();
	~ConsoleFrontLib()
	{
		DeinitialiseMemories();
		safe_delete( MainGame);
		removeModelStore();
		removeTextureStore();

		safe_delete( boris2camera);
		safe_delete( boris2display);

		CloseHandle(hNewDeviceEvent);	hNewDeviceEvent = NULL;
		CloseHandle(hDeviceGoneEvent);	hDeviceGoneEvent= NULL;
	}

	// Start the front end.
	bool Start();

	// Pressed a key.
	void KeyPressed(int ControlPacket);


private:
	// The actual game's front end.
	Game *MainGame;

	camera *MainCamera;

	world *DummyWorld;
	model *TestModel, *TestModel2;
	object *TestObject, *TestObject2;
	DWORD BackgroundBitmap;

	//Load up the necessary models.
	void InitialiseModels();

	Alphabet *MainAlphabet;
	camera *boris2camera;
	display* boris2display;
	HANDLE hNewDeviceEvent;
	HANDLE hDeviceGoneEvent;
};

extern int JapanHelp;
inline void SetJapanText(int type)
{
	JapanHelp = type;
}

