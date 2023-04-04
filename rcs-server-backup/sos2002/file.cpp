/* File.cpp

	 Class to manipulate files on the disk.
 	 Author:	Ian Gledhill
 	 Date:	26th July 1999

     (C) Broadsword Interactive Ltd. */


#include "FrontLib2.h"
#include <errorlib.h>
#include <sys/types.h>
#include <sys/stat.h>


File::File(char *Filename, FrontEndClass *_ParentFrontEnd)
{
	sprintf(FullPathName, Filename);

	ParentFrontEnd = _ParentFrontEnd;

	// Open the file for reading (as writing would create the file and we'd
	// not know where the file was).

	ANSIFile = fopen(FullPathName, "r");
	
	if (!ANSIFile && ParentFrontEnd)
	{
#if defined(JJS)
		// If we've not found the file, look in the include path.
		sprintf(FullPathName, "%s\\%s", ParentFrontEnd->GetTextInclude(), Filename);
		ANSIFile = fopen(FullPathName, "r");
#endif
	}

	if (!ANSIFile)
	{
		// We need to create a new file.
		FileData = NULL;
		return;
	}

	// We may need to read *and write* the file so close it and open it again.
	fclose(ANSIFile);

	ANSIFile = fopen(FullPathName, "rb");

	// Get the size of the file.
	struct stat FileStat;

	stat(FullPathName, &FileStat);

	// Load the file data into RAM.
	if (!FileStat.st_size)
	{
		FileData = NULL;
		return;
	}
	FileData = (char *)malloc(FileLength = FileStat.st_size);
	fread(FileData, 1, FileLength, ANSIFile);
}

File::~File()
{
	// Nothing to free...
	if (!ANSIFile)
		return;
	// We need to flush the buffers by writing the memory back to the file.
	free(FileData);
	fclose(ANSIFile);
}

char *File::ReadLabel(char *LabelName)
{
	// Wot no file? Return a NULL string.
	if (!ANSIFile || !FileData)
		return "NULL";
	
	// OK, we need to find the string after the label requested, and return it.
	char *CurrentPointer = FileData, *LabelStart;
	char ThisLabel[256];
	int LabelLength;
	bool Searching = true;

	// Return NULL if we don`t find it.
	sprintf(ReadString, "NULL");

	do
	{
		LabelLength = 0;
		LabelStart = CurrentPointer;

		// Bypass leading spaces.
		while (*CurrentPointer == 32) CurrentPointer++; 

		while (*CurrentPointer++ != 32) LabelLength++;

		// CurrentPointer now points to the text after the space (because of 
		// the post-increment above).
		strncpy(ThisLabel, LabelStart, LabelLength);
		ThisLabel[LabelLength] = 0;

		// If this is the label we want...
		if (!stricmp(ThisLabel, LabelName))
		{
			int ValueLength;
			if (strchr(CurrentPointer,13) - FileData < FileLength)
				ValueLength = strchr(CurrentPointer,13) - CurrentPointer;
			else
				ValueLength = FileLength + FileData - CurrentPointer;
			// Copy the bytes up to the newline to LabelValue (to be returned).
			strncpy(ReadString, CurrentPointer, ValueLength);
			ReadString[ValueLength] = 0;
			Searching = false;
		}
		
		// find the newline and add 2.
		CurrentPointer = strchr(CurrentPointer, 13) + 2;

		// bypass any empty lines if we're not at the end of the line.
		if (CurrentPointer < FileData+FileLength)
			while (*CurrentPointer == 13) CurrentPointer+=2;
		else
			Searching = false;

	} while (CurrentPointer - FileData < FileLength && Searching && (int)CurrentPointer > 2);

	return ReadString;
}

void File::WriteLabel(char *LabelName, char *LabelValue)
{
	char *CurrentPointer = FileData;

	// Close the old file.
	if (ANSIFile)
		fclose(ANSIFile);
	ANSIFile = fopen(FullPathName, "w");

	char CopyLine[256];
	int LabelLength;

	if (FileData)
	{
		do
		{
			// Bypass the leading spaces.
			while (*CurrentPointer == 32) CurrentPointer++;

			LabelLength = strchr(CurrentPointer, 32) - CurrentPointer;
			if (LabelLength == (int)strlen(LabelName) && !strnicmp(LabelName, CurrentPointer, LabelLength))
			{
				// We've got a match
				bool test=true;
			}
			else
			{
				// Nope, not here. Output to a new file.
				strncpy(CopyLine, CurrentPointer, strchr(CurrentPointer, 13) - CurrentPointer);
				*(CopyLine + (int)strchr(CurrentPointer, 13) - (int)CurrentPointer) = 0;
				fprintf(ANSIFile, "%s\n", CopyLine);
			}
			
			// find the newline and add 2.
			CurrentPointer = strchr(CurrentPointer, 13) + 2;
			
			// bypass any empty lines.
			while (*CurrentPointer == 13) CurrentPointer+=2;
			
		} while (CurrentPointer - FileData < FileLength && *CurrentPointer && (int)CurrentPointer > 2);
	}

	sprintf(CopyLine, "%s %s\n", LabelName, LabelValue);
	fprintf(ANSIFile,  "%s", CopyLine, ANSIFile);

	// Now close and open the file again.
	// Not optimal, but safest, cos the file is definitely
	// closed cleanly this way.
	fclose(ANSIFile);
	
	ANSIFile = fopen(FullPathName, "rb");

	// Get the size of the file.
	struct stat FileStat;

	stat(FullPathName, &FileStat);

	// Load the file data into RAM.
	if (!FileStat.st_size)
	{
		FileData = NULL;
		return;
	}
	FileData = (char *)malloc(FileLength = FileStat.st_size);
	fread(FileData, 1, FileLength, ANSIFile);

	return;
}

char *File::GetFullName()
{
	return FullPathName;
}