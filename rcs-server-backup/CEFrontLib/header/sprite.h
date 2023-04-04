// $Header

// $Log

#ifndef __SPRITE_H__
#define __SPRITE_H__

class Sprite
{
public:
	Sprite(int _ID, Render2D *_Material, Vector *_uv, Vector *_wh, Sprite *Previous);
	~Sprite();

	// Set the position of the sprite.
	SetPosition(Vector *_Position, Vector *_Limits, Vector *_Scale=NULL, int Rotation=0);

	// Justify the sprite.
	Justify(int _Justification);

	// Set the colour.
	SetColour(int NewColour);

	// Draw the sprite in question.
	// Returns the addres of the next sprite in the list.
	Sprite *Draw();

	// Get the details of the sprite.
	void GetDetails(Render2D **_Material, Vector *_uv, Vector *_wh);

	// Show/Hide the sprite.
	void Show();
	void Hide();

	Sprite *Next, *Last;
	int ID;

private:
	Render2D ThisMaterial;
	Vector xyz, Scale, Limits;
	Vector uv, wh;

	int Colour;
	int Justification;

	// The vertices
#if !defined(IGOR)
	D3DTLVERTEX Vertices[4];
#else
	Vector Vertices[4];
#endif

	bool Visible;
};

class SpriteList
{
public:
	SpriteList();
	~SpriteList();

	// Load a texture.
	void LoadTexture(Render2D *Material, char *_Archive, char *TextureName);
	void LoadTexture(Render2D *Material, char *_Archive, char *TextureName, Archive *UsedArchive);

	// Add a new sprite to the list.
	// Returns SpriteID, -1 is failure.
	int Add(Render2D *Material, int _u, int _v, int _w, int _h);

	// Clone a sprites
	int Clone(int ID);

	// Hide or draw a sprite
	void Hide(int ID);
	void Show(int ID);

	// Set the position.
	// Note Scale has x and y values, but Z represents the mode. Pass in an integer according
	// to the keypad: i.e. 5 for centre, 1 for SW, 9 for NE. This is the central point of scaling.
	// X and Y should vary from 0 to 1.
	void SetPosition(int ID, Vector *Position, Vector *Limits=NULL, Vector *Scale=NULL, int Rotation=0);

	// Justify the sprite to a particular margin
	void Justify(int ID, int Justification);

	// Set the colour filter.
	void SetColour(int ID, int NewColour);

	// Delete a sprite.
	void Delete(int ID);

	// Draw the sprites.
	void DrawAll();

	Render2D *UsedMaterials[20];
private:
	// Find a sprite with a given ID.
	Sprite *Find(int ID);

	Sprite *FirstSprite, *LastSprite;

	int SpriteID;
};

#endif __SPRITE_H__