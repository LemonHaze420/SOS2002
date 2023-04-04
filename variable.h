/* Variable.h

  	 Class to hold variables.
 	 Author:	Ian Gledhill
 	 Date:	25th June 1999

     (C) Broadsword Interactive Ltd. */

#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#define VARIABLE_FONT		1
#define VARIABLE_BUTTON		2
#define VARIABLE_BITMAP		3
#define VARIABLE_CONSTANT	4
#define VARIABLE_STRING		5
#define VARIABLE_FUNCTION	6
#define VARIABLE_SHADE		7
#define VARIABLE_FILE		8

// The "Variable" is a node in a linked list.
// The string "Name" identifies the variable so that it can 
// be found by other sections.

// All variables are returned as (void *) and should be cast
// by the caller.

class Variable
{
public:
	// Create the variable.
	Variable(char *_Name, void *_Address, int _Type);

	// Identity returns the name of the variable.
	char *Identity();

	// NextLink returns the address of the next node, or NULL
	// if we're at the end of the chain.
	Variable *NextLink();

	// This returns the type of the variable.
	int GetType();
	void SetType(int _Type);

	// Use this to maintain the chain.
	void SetNextLink(Variable *Next);

	// This returns the address held in the variable.
	void *GetValue();
	void SetValue(void *_Value);

private:
	// The name of the variable.
	// This is held as a string and used as the identifier.
	// To simplify things, this is case-insensitive.
	char Name[256];

	// The address of the details stored by the variable.
	void *Address;

	// The type of the variable.
	int Type;

	// The next node in the list.
	Variable *NextVariable;
};

// The VariableStore.
// Use this as an interface to the variables, to find the 
// individual variable that you require. 

class VariableStore
{
public:
	VariableStore();

	// Returns the address of the variable "Name".
	Variable *FindVariable(char *_Name, Variable **PreviousVariable);

	// Call this function to add a new variable.
	// Returns -1 if the variable couldn`t be added for some
	// reason.
	int AddVariable(char *_Name, void *_Address, int VariableType);

	// This function removes the variable from the list.
	void KillVariable(char *_Name);

	// Find the address of the node the variable refers to.
	// Returns -1 if the variable is not found.
	void *ReadVariable(char *_Name);

	// Find the type of the variable.
	int GetVariableType(char *_Name);

	// GetNextOfType goes through all the variables, returning
	// each variable of the type requested.
	// When the end of the list is reached, NULL is returned.
	// The time after that, the beginning of the list is used
	// again.
	void *GetNextOfType(int SearchType);

	// Delete all variables held.
	void DeleteAll();

	// Duplicate one variable into another.
	// This treats most things as pointers, but strings are created
	// again.
	void Duplicate(char *Variable1, char *Variable2);

private:
	// The list of variables.
	Variable *VariableList;

	// LastLookedAt holds the variable that was last used in a 
	// GetNextOfType() call.
	Variable *LastLookedAt;

};

#endif  __VARIABLE_H__