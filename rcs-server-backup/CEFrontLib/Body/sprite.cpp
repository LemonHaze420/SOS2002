// $Header

// $Log

#define D3D_OVERLOADS
#include "../ConsoleFrontlib.h"

Sprite::Sprite(int _ID, Render2D *_Material, Vector *_uv, Vector *_wh, Sprite *Previous)
{
	Last = Previous;
	if (Last) Last->Next = this;
	Next = NULL;

	uv.x = _uv->x; uv.y = _uv->y;
	wh.x = _wh->x; wh.y = _wh->y;
	xyz.x = 320.0f;  xyz.y = 240.0f; xyz.z = 0.5f;
	Scale.x =1.0f; Scale.y =1.0f;
	
	Limits.x = 256.0f; Limits.y = 256.0f; Limits.z = 1.0f;
	
	ThisMaterial.tex1 = _Material->tex1;
	ThisMaterial.tex2 = _Material->tex2;
	ThisMaterial.col = 0xffffffff;
	ThisMaterial.alpha = true;
	ThisMaterial.drawalways = true;
	ThisMaterial.flags = 0;
	ThisMaterial.tricount = ThisMaterial.numprimsinblock = 0;
	ThisMaterial.started = false;
	
/*	if (0 && ThisMaterial.tex1)
	{
		Texture *OldTexture = ThisMaterial.tex1;
		ThisMaterial.tex1 = new Texture;
		*(ThisMaterial.tex1) = *(OldTexture);
		dprintf("Using texture 0x%x for sprite %d.", ThisMaterial.tex1, _ID);
	}
*/
	Visible = true;
	ID = _ID;
	Colour = 0xFFFFFFFF;

	// Left justified.
	Justification = 7;
}

Sprite::SetPosition(Vector *_Position, Vector *_Limits, Vector *_Scale, int Rotation)
{
	return 0;
	if (!ThisMaterial.tex1)
	{
		Visible = false;
		return 0;
	}

	if (_Position) 
		{xyz.x = _Position->x; xyz.y = _Position->y; xyz.z = _Position->z;}

	if (_Scale) {Scale.x = _Scale->x; Scale.y = _Scale->y; Scale.z = _Scale->z;}	

	if (_Limits) {Limits.x = _Limits->x ; Limits.y = _Limits->y; Limits.z = _Limits->z;}

	float x1,y1,x2,y2;

	x1 = uv.x/Limits.x;  x2 = x1+wh.x/Limits.x;
	y1 = uv.y/Limits.y;  y2 = y1+wh.y/Limits.y;

	x1 += 1.0f/(2*Limits.x); x2 -= 1.0f/(2*Limits.x);
	y1 += 1.0f/(2*Limits.y); y2 -= 1.0f/(2*Limits.y);

	float Corner[8], CoOrdinate[8];
	Vector ScalingWH;

	switch (Rotation)
	{
	case 0:
		Corner[0] = x1; Corner[1] = y1; Corner[2] = x2; Corner[3] = y1;
		Corner[4] = x2; Corner[5] = y2; Corner[6] = x1; Corner[7] = y2;
		CoOrdinate[0] = xyz.x ; CoOrdinate[1] = xyz.y; CoOrdinate[2] = xyz.x+wh.x-1; CoOrdinate[3] = xyz.y;
		CoOrdinate[4] = xyz.x+wh.x-1 ; CoOrdinate[5] = xyz.y+wh.y; CoOrdinate[6] = xyz.x; CoOrdinate[7] = xyz.y+wh.y;
		ScalingWH = Vector(wh.x, wh.y, wh.z);
		break;
	case 6:
		Corner[0] = x1; Corner[1] = y2; Corner[2] = x1; Corner[3] = y1;
		Corner[4] = x2; Corner[5] = y1; Corner[6] = x2; Corner[7] = y2;
		CoOrdinate[0] = xyz.x ; CoOrdinate[1] = xyz.y; CoOrdinate[2] = xyz.x+wh.y-1; CoOrdinate[3] = xyz.y;
		CoOrdinate[4] = xyz.x+wh.y-1 ; CoOrdinate[5] = xyz.y+wh.x; CoOrdinate[6] = xyz.x; CoOrdinate[7] = xyz.y+wh.x;
		ScalingWH = Vector(wh.y, wh.x, wh.z);
		break;
	case 2:
		Corner[0] = x2; Corner[1] = y2; Corner[2] = x1; Corner[3] = y2;
		Corner[4] = x1; Corner[5] = y1; Corner[6] = x2; Corner[7] = y1;
		CoOrdinate[0] = xyz.x ; CoOrdinate[1] = xyz.y; CoOrdinate[2] = xyz.x+wh.x-1; CoOrdinate[3] = xyz.y;
		CoOrdinate[4] = xyz.x+wh.x-1 ; CoOrdinate[5] = xyz.y+wh.y; CoOrdinate[6] = xyz.x; CoOrdinate[7] = xyz.y+wh.y;
		ScalingWH = Vector(wh.x, wh.y, wh.z);
		break;
	}

	if (!_Scale)
	{
/*		switch (Rotation)
		{
		case 0:
			Vertices[0] = _D3DTLVERTEX(_D3DVECTOR(xyz.x,xyz.y,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[0],Corner[1]);
			Vertices[1] = _D3DTLVERTEX(_D3DVECTOR(xyz.x+wh.x-1,xyz.y,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[2],Corner[3]);
			Vertices[2] = _D3DTLVERTEX(_D3DVECTOR(xyz.x+wh.x-1,xyz.y+wh.y,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[4],Corner[5]);
			Vertices[3] = _D3DTLVERTEX(_D3DVECTOR(xyz.x,xyz.y+wh.y,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[6],Corner[7]);
			break;
		case 6:
			Vertices[0] = _D3DTLVERTEX(_D3DVECTOR(xyz.x,xyz.y,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[0],Corner[1]);
			Vertices[1] = _D3DTLVERTEX(_D3DVECTOR(xyz.x+wh.y-1,xyz.y,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[2],Corner[3]);
			Vertices[2] = _D3DTLVERTEX(_D3DVECTOR(xyz.x+wh.y-1,xyz.y+wh.x,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[4],Corner[5]);
			Vertices[3] = _D3DTLVERTEX(_D3DVECTOR(xyz.x,xyz.y+wh.x,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[6],Corner[7]);
			break;
		case 2:
			Vertices[0] = _D3DTLVERTEX(_D3DVECTOR(xyz.x,xyz.y,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[0],Corner[1]);
			Vertices[1] = _D3DTLVERTEX(_D3DVECTOR(xyz.x+wh.y-1,xyz.y,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[2],Corner[3]);
			Vertices[2] = _D3DTLVERTEX(_D3DVECTOR(xyz.x+wh.y-1,xyz.y+wh.x,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[4],Corner[5]);
			Vertices[3] = _D3DTLVERTEX(_D3DVECTOR(xyz.x,xyz.y+wh.x,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[6],Corner[7]);
			break;
		}
*/
#if defined(IGOR)
		Vertices[0] = Vector(CoOrdinate[0], CoOrdinate[1], 1.0f);
		Vertices[1] = Vector(CoOrdinate[4], CoOrdinate[5], 1.0f);
#else
		Vertices[0] = _D3DTLVERTEX(_D3DVECTOR(CoOrdinate[0],CoOrdinate[1],xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[0],Corner[1]);
		Vertices[1] = _D3DTLVERTEX(_D3DVECTOR(CoOrdinate[2],CoOrdinate[3],xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[2],Corner[3]);
		Vertices[2] = _D3DTLVERTEX(_D3DVECTOR(CoOrdinate[4],CoOrdinate[5],xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[4],Corner[5]);
		Vertices[3] = _D3DTLVERTEX(_D3DVECTOR(CoOrdinate[6],CoOrdinate[7],xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[6],Corner[7]);
#endif	
	}
	else
	{
		Vector UsedXYZ, UsedWH, Centre;

		switch ((int)(_Scale->z))
		{
		// Scale from the left.
		case 4:
			UsedXYZ.x = CoOrdinate[0]; UsedXYZ.y = CoOrdinate[1]; UsedXYZ.z = xyz.z;
			UsedWH.x = ScalingWH.x * _Scale->x; UsedWH.y = ScalingWH.y * _Scale->y;
			break;
		// Pivot around the centre.
		case 5:
/*			UsedXYZ.x = (xyz.x+wh.x/2.0f) - (wh.x*Scale.x/2.0f);
			UsedXYZ.y = (xyz.y+wh.y/2.0f) - (wh.y*Scale.y/2.0f);
			UsedWH.x = (wh.x*Scale.x);
			UsedWH.y = (wh.y*Scale.y);
*/
			Centre.x = (CoOrdinate[0] + CoOrdinate[2])/2.0f;
			Centre.y = (CoOrdinate[1] + CoOrdinate[5])/2.0f;
			UsedXYZ.x = Centre.x - ScalingWH.x*Scale.x/2.0f; 
			UsedXYZ.y = Centre.y - ScalingWH.y*Scale.y/2.0f;
			UsedWH.x = (ScalingWH.x*Scale.x)-1;
			UsedWH.y = (ScalingWH.y*Scale.y);
			break;
		case 7:
			UsedXYZ.x = CoOrdinate[0]; UsedXYZ.y = CoOrdinate[1]; UsedXYZ.z = xyz.z;
			UsedWH.x = ScalingWH.x * _Scale->x; UsedWH.y = ScalingWH.y * _Scale->y;
			break;
		// Scale from the top.
		case 8:
			UsedXYZ.x = (xyz.x+wh.x/2.0f) - (wh.x*Scale.x/2.0f);
			UsedXYZ.y = xyz.y;
			UsedWH.x = (wh.x*Scale.x)-1;
			UsedWH.y = (wh.y*Scale.y)-1;
			break;
		}
#if defined(IGOR)
	// SNARK
#else
		Vertices[0] = _D3DTLVERTEX(_D3DVECTOR(UsedXYZ.x,UsedXYZ.y,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[0],Corner[1]);
		Vertices[1] = _D3DTLVERTEX(_D3DVECTOR(UsedXYZ.x+UsedWH.x,UsedXYZ.y,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[2],Corner[3]);
		Vertices[2] = _D3DTLVERTEX(_D3DVECTOR(UsedXYZ.x+UsedWH.x,UsedXYZ.y+UsedWH.y,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[4],Corner[5]);
		Vertices[3] = _D3DTLVERTEX(_D3DVECTOR(UsedXYZ.x,UsedXYZ.y+UsedWH.y,xyz.z),1.0f/xyz.z,Colour,0xffffffff,Corner[6],Corner[7]);
#endif
	}

	Justify(Justification);

	ThisMaterial.AddRect(Vertices[0].x, Vertices[0].y, Vertices[1].x, Vertices[1].y, 0.8f,
						 uv.x, uv.y, uv.x+wh.x, uv.y+wh.y, Colour);
						 
}

Sprite::Justify(int _Justification)
{
	Justification = _Justification;

	int i;

	switch (Justification)
	{
	case 1:
		for (i=0 ; i<2 ; i++)
#if defined(IGOR)
			Vertices[i].y -= wh.y-1;
#else
			Vertices[i].sy -= wh.y-1;
#endif
		break;
	case 2:
	case 3:
	case 4:
		break;
	case 5:
	case 6:
		for (i=0 ; i<2 ; i++)
#if defined(IGOR)
			Vertices[i].x -= wh.x - 1;
#else
			Vertices[i].sx -= wh.x-1;
#endif
		break;
	case 7:
		break;
	}
}

Sprite::SetColour(int NewColour)
{
	if (!ThisMaterial.tex1)
		return 0;

	Colour = NewColour;
	for (int i=0 ; i<4 ; i++)
#if defined(IGOR)
	{
		Colour = NewColour;
		ThisMaterial.tricount = 0;
		ThisMaterial.AddRect(Vertices[0].x, Vertices[0].y, Vertices[1].x, Vertices[1].y, 0.8f,
						 uv.x, uv.y, uv.x+wh.x, uv.y+wh.y, Colour);
	}
#else
		Vertices[i].color = NewColour;
#endif

	return 0;
}

Sprite *Sprite::Draw()
{
	static int OldTriCount;
	if (Visible)
	{
#if defined(IGOR)
		OldTriCount = ThisMaterial.tricount;
		if (ThisMaterial.tex1)
			ThisMaterial.Draw();
		ThisMaterial.tricount = OldTriCount;
#else
		renderSetCurrentMaterial(&Render2D);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, Vertices,4,D3DDP_DONOTCLIP );
#endif
	}

	return Next;
}

void Sprite::Show()
{
	Visible = true;
}

void Sprite::Hide()
{
	Visible = false;
}

void Sprite::GetDetails(Render2D **_Material, Vector *_uv, Vector *_wh)
{
	*_Material = &ThisMaterial;
	*_uv = uv;
	*_wh = wh;

	return;
}


Sprite::~Sprite()
{
//	delete ThisMaterial.tex1;
	if (Last) Last->Next = Next;
	if (Next) Next->Last = Last;
}

SpriteList::SpriteList()
{
	FirstSprite = NULL;
	LastSprite = NULL;
	SpriteID = -1;
}

Sprite *SpriteList::Find(int ID)
{
	Sprite *ThisSprite = FirstSprite;
	while (ThisSprite && ThisSprite->ID != ID)
		ThisSprite = ThisSprite->Next;

	return ThisSprite;
}

int SpriteList::Add(Render2D *Material, int _u, int _v, int _w, int _h)
{
	if (LastSprite)
	{
		Sprite *OldLast = LastSprite;
		LastSprite = new Sprite(++SpriteID, Material, &Vector((float)_u, (float)_v, 0.0f), &Vector((float)_w, (float)_h, 0.0f), OldLast);
	}
	else
	{
		LastSprite = new Sprite(++SpriteID, Material, &Vector((float)_u, (float)_v, 0.0f), &Vector((float)_w, (float)_h, 0.0f), NULL);
	}
	if (!FirstSprite) FirstSprite = LastSprite;

	return SpriteID;
}

int SpriteList::Clone(int ID)
{
	Sprite *OldLast = LastSprite, *OldSprite = Find(ID);

	Vector wh, uv;
	Render2D *Material;

	OldSprite->GetDetails(&Material, &uv, &wh);
	LastSprite = new Sprite(++SpriteID, Material, &uv, &wh, LastSprite);

	return SpriteID;
}

void SpriteList::SetPosition(int ID, Vector *Position, Vector *Limits, Vector *Scale, int Rotation)
{
	Sprite *ThisSprite = Find(ID);

	if (ThisSprite)
		ThisSprite->SetPosition(Position, Limits, Scale, Rotation);
}

void SpriteList::Justify(int ID, int Justification)
{
	Sprite *ThisSprite = Find(ID);

	if (ThisSprite)
		ThisSprite->Justify(Justification);
}

void SpriteList::SetColour(int ID, int NewColour)
{
	Sprite *ThisSprite = Find(ID);

	if (ThisSprite)
		ThisSprite->SetColour(NewColour);
}

void SpriteList::Delete(int ID)
{
	Sprite *ThisSprite = Find(ID);

	if (FirstSprite == ThisSprite) FirstSprite = ThisSprite->Next;
	if (LastSprite == ThisSprite) LastSprite = ThisSprite->Last;

	delete ThisSprite;
}

void SpriteList::DrawAll()
{
	Sprite *ThisSprite = FirstSprite;
	while (ThisSprite)
		ThisSprite = ThisSprite->Draw();
}

void SpriteList::Hide(int ID)
{
	Sprite *ThisSprite = Find(ID);
	if (ThisSprite)
		ThisSprite->Hide();
}

void SpriteList::Show(int ID)
{
	Sprite *ThisSprite = Find(ID);
	if (ThisSprite)
		ThisSprite->Show();
}

void SpriteList::LoadTexture(Render2D *Material, char *_Archive, char *TextureName)
{
	dprintf("** Antique SpriteList::LoadTexture() called! **");
	Material->tex1 = NULL;
	return;
}

void SpriteList::LoadTexture(Render2D *Material, char *_Archive, char *TextureName, Archive *UsedArchive)
{
	Archive *SpriteArchive;
	if (_Archive)
#if defined(IGOR)
	{	
		dprintf("Can't load texture with a named archive in Igor!");
		Material->tex1 = NULL;
		return;

		SpriteArchive = NewArchive();
		SpriteArchive->Open(_Archive);
	}
#else
		arcPush(_Archive);
#endif
	else
	{
		SpriteArchive = UsedArchive;
	}

#if defined(IGOR)
	Texture *ThisTexture = new Texture;
	dprintf("Loading texture %s...", TextureName);
	ThisTexture = FindNewTexture(SpriteArchive, TextureName, 0);
	ThisTexture->TGet(TEXTURE_CACHE_PRIORITY);
#else
	textureData *ThisTexture = getTexturePtr(TextureName, false, false,false);
#endif
	if (ThisTexture) 
	{
#if defined(IGOR)
		Material->tex1 = ThisTexture;
dprintf("Using texture 0x%x for Material.", Material->tex1);
		Material->tex2 = NULL;
		Material->col = 0xffffffff;
		Material->alpha = true;
		Material->drawalways = true;
		Material->flags = 0;
		Material->tricount = Material->numprimsinblock = 0;
		Material->started = false;
#else
		Material->textureHandle = ThisTexture->textureHandle;
		Material->flags = 0;
#endif
	}

	if (_Archive)
#if defined(IGOR)
		SpriteArchive->Close();
#else
		arcPop();
#endif
}

SpriteList::~SpriteList()
{
	Sprite *ThisSprite = FirstSprite;
	Sprite *OldSprite;

	while (ThisSprite)
	{
		OldSprite = ThisSprite;
		ThisSprite = OldSprite->Next;
		delete OldSprite;
	}
}

