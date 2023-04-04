// $Header

// $Log

#define D3D_OVERLOADS
#include "../ConsoleFrontLib.h"


TwoDFont::TwoDFont(char *TextureName, RECT *Dimensions, char *LCTextureName)
{
#if defined (IGOR)
	// SNARK
#else
	textureData *ThisTexture = getTexturePtr(TextureName, false, false, false);
	ThisMaterial[0].flags = 0;
	ThisMaterial[0].textureHandle = ThisTexture->textureHandle;
#endif

	if (LCTextureName)
	{
#if defined(IGOR)
// SNARK
#else
		ThisTexture = getTexturePtr(LCTextureName, false, false, false);
		ThisMaterial[1].flags = 0;
		ThisMaterial[1].textureHandle = ThisTexture->textureHandle;
#endif
	}
	else
		ThisMaterial[1] = ThisMaterial[0];
	int i;
	for (i=0 ; i<256 && Dimensions[i].bottom != -1 ; i++);
	LastLetter = i-1;

	LetterRect = Dimensions;
}

RECT *TwoDFont::GetDimensions(unsigned char Letter)
{

	if (Letter <= LastLetter)
		return &LetterRect[Letter];
	else
		return NULL;
}

TwoDFont::~TwoDFont()
{
}

Word::Word(int _ID, TwoDFont *Font, char *Letters, Vector *Position, RECT *BoundingBox, Word *_Previous, int Colour)
{
	ID = _ID;
	Previous = _Previous;
	Next = NULL;

	LastPosition = *Position;

	int X=0;
	char *LetterPointer = Letters;

	float x1,y1,x2,y2;
	Vector wh;
	
	RECT *Dimensions;
	ThisMaterial = &(Font->ThisMaterial[0]);
	LCMaterial = &(Font->ThisMaterial[1]);
	unsigned char ThisLetter;

	NumberOfLetters=0;
	while (ThisLetter = *LetterPointer)
	{
		if (*LetterPointer == 'ø') 
			ThisLetter = 155;
		if (*LetterPointer == '×') 
			ThisLetter = 158;

		if (ThisLetter == ' ')
			X += (Font->GetDimensions('N'))->right;
		else if (Dimensions = Font->GetDimensions(ThisLetter))
		{
			x1 = Dimensions->left/256.0f;
			x2 = x1+Dimensions->right/256.0f;
			y1 = Dimensions->top/256.0f;
			y2 = y1+Dimensions->bottom/256.0f;	
			
			x1 += 1.0f/512.0f; x2 -= 1.0f/512.0f;
			y1 += 1.0f/512.0f; y2 -= 1.0f/512.0f;

			wh.x = (float)Dimensions->right;
			wh.y = (float)Dimensions->bottom;

#if defined (IGOR)
// SNARK
#else
			Vertices[NumberOfLetters][0] = _D3DTLVERTEX(_D3DVECTOR(Position->x+X, Position->y, Position->z), 1.0f/Position->z, Colour, 0xFFFFFFFF, x1,y1);
			Vertices[NumberOfLetters][1] = _D3DTLVERTEX(_D3DVECTOR(Position->x+wh.x+X-1, Position->y, Position->z), 1.0f/Position->z, Colour, 0xFFFFFFFF, x2,y1);
			Vertices[NumberOfLetters][2] = _D3DTLVERTEX(_D3DVECTOR(Position->x+wh.x+X-1, Position->y+wh.y-1, Position->z), 1.0f/Position->z, Colour, 0xFFFFFFFF, x2,y2);
			Vertices[NumberOfLetters][3] = _D3DTLVERTEX(_D3DVECTOR(Position->x+X, Position->y+wh.y-1, Position->z), 1.0f/Position->z, Colour, 0xFFFFFFFF, x1,y2);
#endif
			if (ThisLetter >= 'a' && ThisLetter <= 'z' || ThisLetter == ':')
			{
				UseLCMaterial[NumberOfLetters] = true;
				X += (int)wh.x;
			}
			else
			{
				UseLCMaterial[NumberOfLetters] = false;
				X += (int)wh.x;
			}
			NumberOfLetters++;
		}
		LetterPointer++;
	}
	Justification = 7;

	Visible = true;
}

Word::~Word()
{
	if (Previous) Previous->Next = Next;
	if (Next) Next->Previous = Previous;
}

void Word::Draw()
{
	if (!Visible)
		return;

	int Counter=0;
	while (Counter < NumberOfLetters)
	{
#if defined (IGOR)
// SNARK
#else
		renderSetCurrentMaterial(ThisMaterial);
		if (UseLCMaterial[Counter])
			renderSetCurrentMaterial(LCMaterial);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, Vertices[Counter++],4,D3DDP_DONOTCLIP );
#endif
	}
}

void Word::Justify(int Boundary)
{
	if (Justification == Boundary)
		return;
		
	float Width, Height;

#if defined(IGOR)
	// SNARK
#else
	Width = Vertices[NumberOfLetters-1][1].sx - Vertices[0][0].sx;
	Height = Vertices[NumberOfLetters-1][3].sy - Vertices[0][0].sy;

	// Bring the word back to the centre.
	for (int i=0 ; i<NumberOfLetters ; i++)
	{
		for (int j=0 ; j<4 ; j++)
		{
			switch (Justification)
			{
			case 1:
				break;
			case 2:
			case 3:
				break;
			case 4:
				Vertices[i][j].sx -= Width*0.5f; 
				break;
			case 5:
				break;
			case 6:
			case 7:
				Vertices[i][j].sx -= Width*0.5f; 
				Vertices[i][j].sy -= Height*0.5f;
				break;
			case 8:
			case 9:
				break;
			}
		}
	}
	
	Justification = Boundary;
	
	// And move it to the new place.
	for (i=0 ; i<NumberOfLetters ; i++)
	{
		for (int j=0 ; j<4 ; j++)
		{
			switch (Justification)
			{
			case 1:
			case 2:
			case 3:
			case 4:
				Vertices[i][j].sx -= Width*0.5f;
				break;
			case 5:
//				Vertices[i][j].sx -= Width;//*0.5f;
//				Vertices[i][j].sy -= Height*0.5f;
				break;
			case 6:
			case 7:
			case 8:
			case 9:
				break;
			}
		}
	}
#endif
}

void Word::SetPosition(Vector *Position, RECT *BoundingBox, int *Colour)
{
	float dX, dY;

	if (Position)
	{
		dX = Position->x - LastPosition.x;
		dY = Position->y - LastPosition.y;
	}

	int Counter=0;

	while (Counter < NumberOfLetters)
	{
		for (int i=0 ; i<4; i++)
		{
#if defined(IGOR)
			// SNARK
#else
			if (Position)
			{
				Vertices[Counter][i].sx += dX;
				Vertices[Counter][i].sy += dY;
			}
			if (Colour)
				Vertices[Counter][i].dcColor = *Colour;
#endif
		}
		Counter++;
	}
	if (Position) LastPosition = *Position;
}

void Word::Hide()
{
	Visible = false;
}

void Word::Show()
{
	Visible = true;
}

void Word::GlowWord(DWORD NewColour, int GlowTimePeriod, int LetterAppearPeriod)
{
}

WordList::WordList()
{
	FirstWord = NULL;
}

WordList::~WordList()
{
}

int WordList::AddWord(TwoDFont *Font, char *Letters, Vector *Position, RECT *BoundingBox, int Colour)
{
	int ID=0;
	if (FirstWord)
	{
		Word *ThisWord = FirstWord;
		ID++;
		while (ThisWord->Next) 
		{
			ThisWord = ThisWord->Next;
			if (ID < ThisWord->ID) ID = ThisWord->ID;
			ID++;
		}
		ThisWord->Next = new Word(ID, Font, Letters, Position, BoundingBox, ThisWord, Colour);
	}
	else
	{
		FirstWord = new Word(ID, Font, Letters, Position, BoundingBox, NULL, Colour);
	}

	return ID;
}

void WordList::SetPosition(int ID, Vector *Position, RECT *BoundingBox)
{
	Word *ThisWord = FirstWord;
	while (ThisWord && ThisWord->ID != ID) ThisWord = ThisWord->Next;

	if (ThisWord) ThisWord->SetPosition(Position, BoundingBox, NULL);
}

void WordList::SetColour(int ID, int Colour)
{
	Word *ThisWord = FirstWord;
	while (ThisWord && ThisWord->ID != ID) ThisWord = ThisWord->Next;

	if (ThisWord) ThisWord->SetPosition(NULL, NULL, &Colour);
}

void WordList::Delete(int ID)
{
	Word *ThisWord = FirstWord;
	while (ThisWord && ThisWord->ID != ID) ThisWord = ThisWord->Next;

	if (FirstWord == ThisWord) FirstWord = NULL;
	if (ThisWord) delete ThisWord;
}

void WordList::Justify(int ID, int Boundary)
{
	Word *ThisWord = FirstWord;
	while (ThisWord && ThisWord->ID != ID) ThisWord = ThisWord->Next;

	ThisWord->Justify(Boundary);
}

void WordList::DrawAll()
{
	Word *ThisWord = FirstWord;
	while (ThisWord)
	{
		ThisWord->Draw();
		ThisWord = ThisWord->Next;
	}
}

void WordList::Hide(int ID)
{
	Word *ThisWord = FirstWord;
	while (ThisWord && ThisWord->ID != ID) ThisWord = ThisWord->Next;

	if (ThisWord) ThisWord->Hide();
}

void WordList::Show(int ID)
{
	Word *ThisWord = FirstWord;
	while (ThisWord && ThisWord->ID != ID) ThisWord = ThisWord->Next;

	if (ThisWord) ThisWord->Show();
}

void WordList::GlowWord(int ID, DWORD NewColour, int GlowTimePeriod, int LetterAppearPeriod)
{
	Word *ThisWord = FirstWord;
	while (ThisWord && ThisWord->ID != ID) ThisWord = ThisWord->Next;

	if (ThisWord) ThisWord->GlowWord(NewColour, GlowTimePeriod, LetterAppearPeriod);
}
