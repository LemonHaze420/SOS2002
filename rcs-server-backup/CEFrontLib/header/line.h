// $Header

// $Log

#ifndef __LINELIST_H__
#define __LINELIST_H__

class LineInstance
{
public:
	LineInstance(int _ID, Vector *_Start, Vector *_End, int _Colour);
	~LineInstance();

	void SetColour(int _Colour);
	void SetPosition(Vector *_Start, Vector *_End);
	
	void Draw();
	void Hide();
	void Show();

	LineInstance *Next, *Last;
	int ID;
private:
	Vector Start, End;
	int Colour;
	bool Visible;
};

class LineList
{
public:
	LineList();
	~LineList();

	int Add(Vector *Start, Vector *End, int Colour=0xFFFFFFFF);
	void Delete(int ID);
	void Hide(int ID);
	void Show(int ID);
	void SetColour(int ID, int Colour);
	void SetPosition(int ID, Vector *Start, Vector *End);

	void Draw();
private:
	LineInstance *FindLine(int ID);

	LineInstance *FirstLine, *LastLine;
	int NextLine;
};

#endif
