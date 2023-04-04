/*
	$Id$
	$Log$
	Revision 1.0  2000-02-21 12:34:26+00  jjs
	Initial revision

	Revision 1.1  1999/03/29 09:23:50  jcf
	Initial revision

	Revision 1.2  1998/12/02 11:35:57  jcf
	Added id and log entries

*/

/****Thu Feb 26 09:43:09 1998*************(C)*Broadsword*Interactive****
 *
 * Parameter file management.
 * A parameter file is a file of format FMT_TXT_PARAM, stored in the
 * archive stack somewhere. It's a text file of the form
 *
 * key : value
 *
 * A key can have multiple values, indicated by multiple lines with
 * identical keys.
 *
 ***********************************************************************/
 

class ParamFile
{
public:
	ParamFile();
	~ParamFile();

	bool ReadFromMemory(char *area,unsigned long size);
	bool Read(char *name);	// read the PF into memory - makes a copy of the data
	bool ReadFromFile(char *name);

	// search the file for the given key - if NULL, look for more occurrences
	// of the previous Find key. Returns the value, or NULL if the key
	// is not found.

	// If length is -1 (or no length is given), the whole key is compared.
	// If a number is specified, only the first n character are compared. If
	// zero is given, all keys match - you can iterate over the entire file.
	
	char *Find(char *key,int len=-1);

	// return the value/key of the last found item

	char *Value() { return (curoff>=0) ? vals[curoff] : NULL; }
	char *Key() { return (curoff>=0) ? keys[curoff] : NULL; }

private:
	char curkey[64];	// current search key
	char *data;	// array of null-sepped strings
	char **keys;	// offsets of key strings in 'data'
	char **vals;	// offsets of value strings in 'data'
	int offset;	// the current search point
	int num;	// number of items
	int curoff;	// last found item
	int lastlen;// last length parameter to Find for when key=NULL
};
