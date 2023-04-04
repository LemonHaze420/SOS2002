// $Header

// $Log

class TwoDFont
{
public:
	TwoDFont(char *TextureName, RECT *Dimensions, char *LCTextureName=NULL);
	~TwoDFont();

	RECT *GetDimensions(unsigned char Letter);
	
	Material ThisMaterial[2];
private:
	RECT *LetterRect;

	int Height, LastLetter;
};

class Word
{
public:
	Word(int _ID, TwoDFont *Font, char *Letters, Vector *Position, RECT *BoundingBox, Word *_Previous, int Colour = 0xFFFFFFFF);
	~Word();

	void Draw();
	void SetPosition(Vector *Position, RECT *BoundingBox, int *Colour);
	void SetColour(int Colour);
	void Justify(int Boundary);

	void Hide();
	void Show();
	void GlowWord(DWORD NewColour, int GlowTimePeriod, int LetterAppearPeriod);

	Word *Next, *Previous;
	int ID;
private:
	// 40 letters...
#if defined(IGOR)
	int Vertices[60][4];
#else
	D3DTLVERTEX Vertices[60][4];
#endif

	// Set to TRUE for lower case, FALSE for lower case.
	bool UseLCMaterial[60];
	Material *ThisMaterial;
	// The material used for the lower case letters.
	Material *LCMaterial;
	int NumberOfLetters;
	Vector LastPosition;
	bool Visible;

	// Set according to the keypad (5=centre)
	int Justification;
};

class WordList
{
public:
	WordList();
	~WordList();

	int AddWord(TwoDFont *Font, char *Letters, Vector *Position, RECT *BoundingBox, int Colour=0xFFFFFFFF);
	void SetPosition(int ID, Vector *Position, RECT *BoundingBox);
	void SetColour(int ID, int Colour);
	void Delete(int ID);
	void Justify(int ID, int Boundary);

	void Hide(int ID);
	void Show(int ID);
	void GlowWord(int ID, DWORD NewColour, int GlowTimePeriod, int LetterAppearPeriod);

	void DrawAll();	
private:
	Word *FirstWord;
};
