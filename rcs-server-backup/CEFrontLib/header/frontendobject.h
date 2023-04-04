// $Header$

// $Log$
// Revision 1.5  2000-08-07 16:20:07+01  img
// Adding options screens...
//
// Revision 1.4  2000-03-13 18:14:52+00  img
// Moved justify_* out.
//
// Revision 1.3  2000-02-29 08:44:43+00  jjs
// Added Header and Log lines
//

/* MeshModel file for the ConsoleFrontLib

	Broadsword Interactive Ltd.

	Author:		Ian Gledhill
	Date:		29/11/1999
	Rev:		0.01 */

#ifndef __FRONTENDOBJECT_H__
#define __FRONTENDOBJECT_H__

#include "ConsoleFrontLib.h"

#define JUSTIFY_CENTRE 0
#define JUSTIFY_LEFT 1
#define JUSTIFY_RIGHT 2
#define JUSTIFY_TOP 3
#define JUSTIFY_BOTTOM 4

class FrontEndObject
{
public:
	// Use ParentObject if MeshModel is relative to another FrontEndObject.
	FrontEndObject(char *ArchiveName, char *ObjectName, float Scale=1.0f, FrontEndObject *ParentFEObject=NULL, bool UseZeroCentre=false);
	~FrontEndObject();

	// Delete the associated model.
	void DeleteModel();

	void Draw();
	void Draw(Vector *Position);
	void Draw(int x, int y, float z=20.0f);
	void Hide();

	void MoveInModelSpace(Vector *Position);

	void Rotate(Vector *Target);
	void Rotate(float x=0.0f, float y=0.0f, float z=0.0f);

	void Attach(Object *NewChild);

	void GetDimensions(Vector *BottomLeft, Vector *TopRight);

	// justify the model according to the parameter. i.e. Left means left-justified text.
	void Justify(int Edge);
private:
	// The boris2 objects.
	// The Parent Object is used to make all operators on the object central.
	Object *ParentObject, *ActualObject;

	MeshModel *ActualModel;

	Vector Offset;

	Archive *ObjectArchive;

	// Set to true if the model is in the model chain.
	bool InChain;
};

#endif