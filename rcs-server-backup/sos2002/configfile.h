// $Header$

// $Log$

#ifndef _ConfigFile_H_
#define _ConfigFile_H_

// Read a parameter in from the config file. 
// The config file is delimited by a question mark
bool ReadParameter(char *_FileName,
				   int _ParamNumber,
				   char *_Out);

// ------------------------------------------------------------------------------------------
// Write new config file.
// Opens a new config file, use AppendConfigFile to add 
// more fields, and FinaliseConfigFile to finish the file.
bool OpenFile(char *_FileName);

// ------------------------------------------------------------------------------------------
// Appends (writes) information into the temp config file.
// Note: Must use OpenConfigFile first!
bool AppendFile(char *_FileName,
				char *_Data);

// ------------------------------------------------------------------------------------------
// Removes the old config file, and puts the new one in place.
bool FinaliseFile(char *_FileName);

// ------------------------------------------------------------------------------------------
// This function renames the config file, and sets the code to run it FirstTimeRunning mode.
void InvalidFile(char *_FileName);

#endif