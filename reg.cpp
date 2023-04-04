#include <windows.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>


//#include "frontlib2.h"


/****************************************************************************
 *
 *
 *    
 ****************************************************************************/

// Yuk. This is written in C.  when I don`t need to maintain backward compatibility,
// I'll do it in C++.   IMG

File *RegistryFile=NULL;

void GetRegistryValue(char *RegistryKey, char *KeyString, char *KeyValue)
{
	// Open the "Registry" file.
	if (!RegistryFile)
		RegistryFile = new File("NotTheRegistry.txt", NULL);

	strcpy(KeyValue, RegistryFile->ReadLabel(KeyString));
}

void SetRegistryValue(char *RegistryKey, char *KeyString, char *KeyValue)
{
	if (!RegistryFile)
		RegistryFile = new File("NotTheRegistry.txt", NULL);

	RegistryFile->WriteLabel(KeyString, KeyValue);
}

void DumpRegistry(char *Filename)
{
	if (!RegistryFile)
		return;

	// OK, we have a registry. Now let's copy it.
	FILE *DestinationFile, *SourceFile;

	// Open the source and destination files.
	DestinationFile = fopen(Filename, "wb");
	SourceFile = fopen(RegistryFile->GetFullName(), "rb");

	// Get the size of the file.
	struct stat FileStat;
	stat(RegistryFile->GetFullName(), &FileStat);

	// Load the file data into RAM.
	if (!FileStat.st_size)
		return;

	char *FileData = (char *)malloc(FileStat.st_size);
	
	fread(FileData, 1, FileStat.st_size, SourceFile);
	// And dump it out again.
	fwrite(FileData, 1, FileStat.st_size, DestinationFile);

	// Free everything.
	fclose(SourceFile);
	fclose(DestinationFile);
	free(FileData);
}

void RestoreRegistry(char *Filename)
{
	FILE *DestinationFile, *SourceFile;
	char RegistryName[256];

	strcpy(RegistryName, RegistryFile->GetFullName());

	delete RegistryFile;

	// Open the source and destination files.
	DestinationFile = fopen(RegistryName, "wb");
	SourceFile = fopen(Filename, "rb");

	// Get the size of the file.
	struct stat FileStat;
	stat(Filename, &FileStat);

	// Load the file data into RAM.
	if (!FileStat.st_size)
		return;

	char *FileData = (char *)malloc(FileStat.st_size);
	
	fread(FileData, 1, FileStat.st_size, SourceFile);
	// And dump it out again.
	fwrite(FileData, 1, FileStat.st_size, DestinationFile);

	// Free everything.
	fclose(SourceFile);
	fclose(DestinationFile);
	free(FileData);

	RegistryFile = new File("NotTheRegistry.txt", NULL);
}
