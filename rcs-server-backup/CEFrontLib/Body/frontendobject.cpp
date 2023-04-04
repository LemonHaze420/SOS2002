// $Header$

// $Log$
// Revision 1.21  2000-05-09 15:51:19+01  jcf
// Unpleasant hack removed, since ctors now called correctly in SOS_Index.cpp
//
// Revision 1.20  2000-05-09 12:10:03+01  jcf
// Set UseZeroCentre to always true
//
// Revision 1.19  2000-04-19 21:37:56+01  img
// Localisation.
//
// Revision 1.18  2000-04-17 15:53:25+01  jcf
// <>
//
// Revision 1.17  2000-04-17 14:53:32+01  img
// Bug fixes
//
// Revision 1.16  2000-04-12 13:59:45+01  img
// bug fixes
//
// Revision 1.15  2000-04-12 12:21:22+01  img
// Bug fix
//
// Revision 1.14  2000-04-10 17:20:55+01  img
// Bug fix
//
// Revision 1.13  2000-04-07 14:36:52+01  img
// No model deletion (removeModelStore() does that now)
//
// Revision 1.12  2000-03-20 18:00:40+00  img
// Beta build
//
// Revision 1.11  2000-03-10 16:10:32+00  img
// New JUSTIFY_*
//
// Revision 1.10  2000-03-06 13:03:10+00  jjs
// Removed depndancy on \\pc and KATANA. Now uses the DISKNAME value in bs2all.h
//
// Revision 1.9  2000-03-03 14:42:27+00  jjs
// Fixed for Boris3.
//
// Revision 1.8  2000-03-02 14:26:35+00  img
// Controller screen now working
//
// Revision 1.7  2000-03-02 09:49:52+00  img
// Bug in Boris2Emulation fixed.  Player name entry works.
//
// Revision 1.6  2000-02-29 08:43:34+00  jjs
// Added Header & Log lines.
//

/* MeshModel file for the ConsoleFrontLib

	Broadsword Interactive Ltd.

	Author:		Ian Gledhill
	Date:		29/11/1999
	Rev:		0.01 */

#include "ConsoleFrontLib.h"

FrontEndObject::FrontEndObject(char *ArchiveName, char *ObjectName, float Scale, FrontEndObject *ParentFEObject, bool UseZeroCentre)
{
	return;
	if (ArchiveName)
	{
		// Push the archives in the archives directory.
		char ActualArchiveName[256];
#if defined(UNDER_CE)
		sprintf(ActualArchiveName, DISKNAME"\\SOS1937FrontEnd\\%s", ArchiveName);
#else
		sprintf(ActualArchiveName, FRONTEND_DATA "%s", ArchiveName);
#endif
#if defined (IGOR)
		ObjectArchive = NewArchive();
		ObjectArchive->Open(ActualArchiveName);
#else
		arcPush(ActualArchiveName);
#endif
	}

	// Load in the object from the archive.
#if defined(IGOR)
	// SNARK
#else
	ActualModel = getModelPtr( ObjectName, Scale, false, NULL);
#endif
	if(!ActualModel)dprintf("load failed : %s",ObjectName);

	// Create the object
#if defined(IGOR)
	// SNARK
#else
	ActualObject = new Object(ActualModel, 0, false);
#endif
	if (ArchiveName)
	{
		// .. and pop the archive afterwards.
#if defined(IGOR)
		ObjectArchive->Close();
#else
		arcPop();
#endif
	}

	if (ParentFEObject)
	{
		ParentFEObject->Attach(ActualObject);
#if defined(IGOR)
	// SNARK
#else
		ActualObject->setAngle(&vector3(0,0,0));
#endif
		ParentObject = NULL;
		ActualObject->MakeInvisible();
		ActualObject->HasMoved();
		if (UseZeroCentre)
		{
#if defined(IGOR)
	// SNARK
#else
			ActualObject->transform.translation = vector3(0.0f,0.0f,0.0f);
#endif
			Offset = Vector(0.0f,0.0f,0.0f);
		}
		else
		{
#if defined(IGOR)
	// SNARK
#else
			Offset = Vector((ActualModel->boundingBox.hi.x + ActualModel->boundingBox.lo.x) * -0.5f,
				     (ActualModel->boundingBox.hi.y + ActualModel->boundingBox.lo.y) * -0.5f,
				     (ActualModel->boundingBox.hi.z + ActualModel->boundingBox.lo.z) * -0.5f);
#endif
#if defined(IGOR)
	// SNARK
#else
			ActualObject->transform.translation = Offset;
#endif
		}
	}
	else
	{
		// Make the invisible parent object.
#if defined(IGOR)
	// SNARK
#else
		ParentObject = new Object(NULL, 0, false);
		ParentObject->InsertChild(ActualObject);
#endif
#if defined(IGOR)
	// SNARK
#else
		if (UseZeroCentre)
			ActualObject->transform.translation = vector3(0.0f,0.0f,0.0f);
		else
			ActualObject->transform.translation = vector3((ActualModel->boundingBox.hi.x + ActualModel->boundingBox.lo.x) * -0.5f,
														  (ActualModel->boundingBox.hi.y + ActualModel->boundingBox.lo.y) * -0.5f,
														  (ActualModel->boundingBox.hi.z + ActualModel->boundingBox.lo.z) * -0.5f);
#endif
	}
	InChain = false;
}

FrontEndObject::~FrontEndObject()
{
	return;
	if (ActualObject)
	{
		//	ActualObject->detachChild();
#if defined(IGOR)
	// SNARK
#else
		ActualObject->detachSister();
		ActualObject->detachChild();
#endif
		//	if (ActualObject->prevsister) ActualObject->prevsister->detachSister();
		delete ActualObject;
	}
	else
	{
		dprintf("Urgh! model chain corrupted!!!!");
	}
	if (ParentObject)
	{
//		ParentObject->detachChild();
		if (InChain)
		{
#ifdef BORISEMULATIONHACK
			EmulationRemoveObject(ParentObject);
#else
#if defined(IGOR)
	// SNARK
#else
			boris2->removeObject(ParentObject);
#endif
#endif
		}
		delete ParentObject;
	}
}

void FrontEndObject::DeleteModel()
{
//	delete ActualModel;
}

void FrontEndObject::Attach(Object *NewChild)
{
#if defined(IGOR)
	// SNARK
#else
	if (ActualObject->child)
		ActualObject->child->insertSister(NewChild);
	else
		ActualObject->insertChild(NewChild);
#endif
}

void FrontEndObject::Draw()
{
	return;
	if (ParentObject)
	{
		ParentObject->MakeVisible();
	}
	ActualObject->HasMoved();
	ActualObject->MakeVisible();

	if (!InChain && ParentObject)
#ifdef BORISEMULATIONHACK
		EmulationInsertObject(ParentObject);
#else
#if defined(IGOR)
	// SNARK
#else
		boris2->insertObject(ParentObject);
#endif
#endif
	
	InChain = true;
}

void FrontEndObject::Draw(Vector *Position)
{
	return;
	Draw((int)Position->x, (int)Position->y, (float)Position->z);
}

void FrontEndObject::Draw(int x, int y, float z)
{
	return;
#if defined(IGOR)
	// SNARK
#else
	ActualObject->setAngle(&vector3(0,0,0));
#endif
	static Vector WorldVector;
#ifdef BORISEMULATIONHACK
	EmulationGetWorldFromScreen(&WorldVector, (float)x,(float)y,z);
#else
#if defined(IGOR)
	// SNARK
#else
	boris2->getWorldFromScreen(&WorldVector, (float)x,(float)y,z*gWFSHack);
#endif
#endif
	if (ParentObject)
	{
#if defined(IGOR)
	// SNARK
#else
		ParentObject->setAngle(&vector3(0,0,0));
		ParentObject->transform.translation = WorldVector;
	
		ParentObject->changed = true;
		ParentObject->visible = true;
		ParentObject->inheritTransformations();
#endif
		if (!InChain)
		{
#ifdef BORISEMULATIONHACK
			EmulationInsertObject(ParentObject);
#else
#if defined(IGOR)
	// SNARK
#else
			boris2->insertObject(ParentObject);
#endif
#endif
		}
		InChain = true;
	}
#if defined(IGOR)
	// SNARK
#else
	ActualObject->changed = true;
	ActualObject->visible = true;
#endif
}

void FrontEndObject::Hide()
{
	if (ParentObject)
	{	
		ParentObject->MakeVisible();
		if (InChain) 
#ifdef BORISEMULATIONHACK
			EmulationRemoveObject(ParentObject);
#else
#if defined(IGOR)
		{};// SNARK
#else
			boris2->removeObject(ParentObject);
#endif
#endif
	}
	ActualObject->MakeVisible();
	InChain = false;
}

void FrontEndObject::Rotate(Vector *Target)
{
	Rotate(Target->x, Target->y, Target->z);
}

void FrontEndObject::Rotate(float x, float y, float z)
{
#if defined(IGOR)
	// SNARK
#else
	Matrix3x4 RotationMatrixX, RotationMatrixY, RotationMatrixZ, TempMatrix;
	
	float rad_x = (x*PI*2.0f)/360.0f, rad_y = (y*PI*2.0f)/360.0f, rad_z = (z*PI*2.0f)/360.0f;

	float cosx = (float)cos(rad_x), cosy = (float)cos(rad_y), cosz = (float)cos(rad_z);
	float sinx = (float)sin(rad_x), siny = (float)sin(rad_y), sinz = (float)sin(rad_z);

	RotationMatrixX.set3x3submat(1.0f,0.0f,0.0f,
								0.0f,cosx,-sinx,
								0.0f,sinx,cosx);
	RotationMatrixY.set3x3submat(cosy,0.0f,-siny,
								0.0f,1.0f,0.0f,
								siny,0.0f,cosy);
	RotationMatrixZ.set3x3submat(cosz,-sinz,0.0f,
								sinz,cosz,0.0f,
								0.0f,0.0f,1.0f);
	matrix3x4 temp;

	temp.multiply3x3submats(&RotationMatrixY,&RotationMatrixZ);
	TempMatrix = temp;
	temp.multiply3x3submats(&RotationMatrixX,&TempMatrix);
	temp.translation = ActualObject->transform.translation;
	ActualObject->transform = temp;
#endif

	if (ParentObject)
	{
		ParentObject->HasMoved();
#if defined(IGOR)
	// SNARK
#else
		ParentObject->inheritTransformations();
#endif
	}
	else if (ActualObject->GetParent())
	{
		ActualObject->GetParent()->HasMoved();
#if defined(IGOR)
	// SNARK
#else
		ActualObject->GetParent()->inheritTransformations();
#endif
	}
}

void FrontEndObject::MoveInModelSpace(Vector *Position)
{
#if defined(IGOR)
	// SNARK
#else
	ActualObject->transform.translation = *Position + Offset;
#endif
	ActualObject->HasMoved();
	if (ParentObject)
	{
		ParentObject->HasMoved();
	}
	else if (ActualObject->GetParent())
	{
		ActualObject->GetParent()->HasMoved();
		if (ActualObject->GetParent()->GetParent())
		{
			ActualObject->GetParent()->GetParent()->HasMoved();
		}
		else	
			ActualObject->GetParent()->HasMoved();
	}
}

void FrontEndObject::GetDimensions(Vector *BottomLeft, Vector *TopRight)
{
#if defined(IGOR)
	// SNARK
#else
	BottomLeft->x = ActualModel->boundingBox.lo.x;
	BottomLeft->y = ActualModel->boundingBox.lo.y;
	BottomLeft->z = ActualModel->boundingBox.lo.z;

	TopRight->x = ActualModel->boundingBox.hi.x;
	TopRight->y = ActualModel->boundingBox.hi.y;
	TopRight->z = ActualModel->boundingBox.hi.z;
#endif
}

void FrontEndObject::Justify(int Edge)
{
#if defined(IGOR)
	// SNARK
#else
	switch (Edge)
	{
	case JUSTIFY_CENTRE:
		Offset = vector3((ActualModel->boundingBox.hi.x + ActualModel->boundingBox.lo.x) * -0.5f,
			     (ActualModel->boundingBox.hi.y + ActualModel->boundingBox.lo.y) * -0.5f,
			     (ActualModel->boundingBox.hi.z + ActualModel->boundingBox.lo.z) * -0.5f);
		ActualObject->transform.translation = Offset;
	case JUSTIFY_LEFT:
		Offset = vector3(ActualModel->boundingBox.hi.x,
			     (ActualModel->boundingBox.hi.y + ActualModel->boundingBox.lo.y) * -0.5f,
				 (ActualModel->boundingBox.hi.z + ActualModel->boundingBox.lo.z) * -0.5f);
		ActualObject->transform.translation = Offset;
		ActualObject->changed = true;
		ActualObject->parent->inheritTransformations();
		break;
	case JUSTIFY_RIGHT:
		Offset = vector3(ActualModel->boundingBox.lo.x,
			     (ActualModel->boundingBox.hi.y + ActualModel->boundingBox.lo.y) * -0.5f,
				     (ActualModel->boundingBox.hi.z + ActualModel->boundingBox.lo.z) * -0.5f);
		ActualObject->transform.translation = Offset;
		ActualObject->changed = true;
		ActualObject->parent->inheritTransformations();
		break;
	case JUSTIFY_BOTTOM:
		Offset = vector3((ActualModel->boundingBox.hi.x + ActualModel->boundingBox.lo.x) * -0.5f,
			     ActualModel->boundingBox.hi.y,
			     (ActualModel->boundingBox.hi.z + ActualModel->boundingBox.lo.z) * -0.5f);
		ActualObject->transform.translation = Offset;
		ActualObject->changed = true;
		ActualObject->parent->inheritTransformations();
		break;
	case JUSTIFY_TOP:
		Offset = vector3((ActualModel->boundingBox.hi.x + ActualModel->boundingBox.lo.x) * -0.5f,
			     ActualModel->boundingBox.lo.y,
			     (ActualModel->boundingBox.hi.z + ActualModel->boundingBox.lo.z) * -0.5f);
		ActualObject->transform.translation = Offset;
		ActualObject->changed = true;
		ActualObject->parent->inheritTransformations();
		break;
	default:
		break;
	}
#endif
}