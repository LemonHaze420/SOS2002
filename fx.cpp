// $Header$

// $Log$
// Revision 1.0  2000-02-21 12:34:17+00  jjs
// Initial revision
//


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "BS2all.h"
#include "ALLcar.h"
#include "fx.h"

spriteFX *explosFX,
	*fire1FX,
	*fire2FX,
	*fire3FX,
	*fire4FX,
	*smokeFX,
	*exhaustFX,
	*tyresmokeFX;

void LoadFX()
{
	explosFX = new spriteFX("explos");
	fire1FX = new spriteFX("fire1");
	fire2FX = new spriteFX("fire2");
	fire3FX = new spriteFX("fire3");
	fire4FX = new spriteFX("fire4");
	smokeFX = new spriteFX("smoke");
	tyresmokeFX = new spriteFX("smoke");
	exhaustFX = new spriteFX("exhaust");

	tyresmokeFX->ydrift=true;
	exhaustFX->ydrift = true;
	smokeFX->engydrift=true;
}


void DeleteFX()
{
	delete explosFX;
	delete fire1FX;
	delete fire2FX;
	delete fire3FX;
	delete smokeFX;
	delete exhaustFX;
	delete tyresmokeFX;
}

