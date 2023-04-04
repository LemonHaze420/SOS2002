/* Animation.h
	For providing animations within the front end, e.g. the car selection
	screen.

	Ian Gledhill 25/05/99 Broadsword Interactive Ltd.

	V0.1 */

#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <windows.h>


class Animation
{
public:
	Animation(char *NewName, RECT *StartPosition, RECT *FinishPosition, int Timer);

	void Animate();
	bool Add(char *NewName, RECT *StartPosition, RECT *FinishPosition, int Timer);

	// Remove all children.
	void DestroyAll();

	// Identification
	int		ID;
	char	Name[80];

	float dx, dy, dt;
	int StartTime;
	int Width, Height;

	// Location
	RECT Start, Current, Finish, SrcArea;

	Animation *NextAnimation;
};


#endif __ANIMATION_H__