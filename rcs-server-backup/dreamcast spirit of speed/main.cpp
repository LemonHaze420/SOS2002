// $Header$

// $Log$
// Revision 1.0  1999-12-02 17:42:55+00  jjs
// Initial revision
//

#include <shinobi.h>
#include <Nindows.h>

void main(void)
{
	njUserInit();

	while (1) {
		if (njUserMain() < 0) break;
		njWaitVSync();
	}

	njUserExit();
}

void njUserInit(void)
{
	sbInitSystem( NJD_RESOLUTION_640x480_NTSCNI, NJD_FRAMEBUFFER_MODE_RGB565, 1 );
	njInitVertexBuffer( 900000, 0, 200000, 0 ,0);
	njInitPrint( NULL, 0, NJD_TEXFMT_ARGB_1555 );
 	nwInitSystem(0, NJD_PORT_A0);
}

Sint32 njUserMain(void)
{
    njPrintC(NJM_LOCATION(4,4),"RENDER TEXTURE SAMPLE");
	return nwExecute();
}

void njUserExit(void)
{
	nwExitSystem();
	njExitPrint();
	njExitTexture();
	sbExitSystem();
}
