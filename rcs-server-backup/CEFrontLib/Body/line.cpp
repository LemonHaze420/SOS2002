// $Header

// $Log

#define D3D_OVERLOADS

#include "../ConsoleFrontLib.h"

LineInstance::LineInstance(int _ID, Vector *_Start, Vector *_End, int _Colour)
{
	ID = _ID;
	Colour = _Colour;

	Start = *_Start;
	End = *_End;

	Visible = true;

	Last = Next = NULL;
}

LineInstance::~LineInstance()
{
	if (Next) Next->Last = Last;
	if (Last) Last->Next = Next;
}

void LineInstance::SetColour(int _Colour)
{
	Colour = _Colour;
}

void LineInstance::SetPosition(Vector *_Start, Vector *_End)
{
	Start = *_Start;
	End = *_End;
}

void LineInstance::Draw()
{
	if (!Visible) return;

//	renderSetCurrentMaterial(NULL);

#if defined(IGOR)
	// SNARK
	int LineVertices[2];
#else
	D3DTLVERTEX LineVertices[2];
	LineVertices[0] = _D3DTLVERTEX(_D3DVECTOR(Start.x, Start.y, Start.z), 1.0f/Start.z, Colour, 0xFFFFFFFF, 0.5f, 0.5f);
	LineVertices[1] = _D3DTLVERTEX(_D3DVECTOR(End.x, End.y, End.z), 1.0f/Start.z, Colour, 0xFFFFFFFF, 0.5f, 0.5f);
	renderLine((D3DTLVERTEX *)LineVertices, 2);
#endif	
}

void LineInstance::Hide()
{
	Visible = false;
}

void LineInstance::Show()
{
	Visible = true;
}

// ************** Line List class ***************

LineList::LineList()
{
	FirstLine = LastLine = NULL;
	NextLine = 0;
}

LineList::~LineList()
{
	LineInstance *NextLine, *ThisLine = FirstLine;
	while (ThisLine)
	{
		NextLine = ThisLine->Next;
		delete ThisLine;
		ThisLine = NextLine;
	}
}

int LineList::Add(Vector *Start, Vector *End, int Colour)
{
	if (FirstLine)
	{
		LineInstance *OldLastLine = LastLine;
		LastLine = new LineInstance(NextLine++, Start, End, Colour);
		OldLastLine->Next = LastLine;
		LastLine->Last = OldLastLine;
	}
	else
	{
		LastLine = FirstLine = new LineInstance(NextLine++, Start, End, Colour);
	}
	return NextLine-1;
}

void LineList::Hide(int ID)
{
	LineInstance *ThisLine = FindLine(ID);
	if (ThisLine) ThisLine->Hide();
}

void LineList::Show(int ID)
{
	LineInstance *ThisLine = FindLine(ID);
	if (ThisLine) ThisLine->Show();
}

void LineList::SetColour(int ID, int Colour)
{
	LineInstance *ThisLine = FindLine(ID);
	if (ThisLine) ThisLine->SetColour(Colour);
}

void LineList::SetPosition(int ID, Vector *Start, Vector *End)
{
	LineInstance *ThisLine = FindLine(ID);
	if (ThisLine) ThisLine->SetPosition(Start, End);
}

LineInstance *LineList::FindLine(int ID)
{
	LineInstance *ThisLine = FirstLine;
	while (ThisLine->ID != ID && ThisLine != NULL) ThisLine = ThisLine->Next;

	return ThisLine;
}

void LineList::Delete(int ID)
{
	LineInstance *ThisLine = FindLine(ID);
	if (ThisLine) delete ThisLine;
}

void LineList::Draw()
{
	LineInstance *ThisLine = FirstLine;
	while (ThisLine)
	{
		ThisLine->Draw();
		ThisLine = ThisLine->Next;
	}
}
