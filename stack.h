/* Stack.h

	 Class to manipulate fonts.
 	 Author:	Ian Gledhill
 	 Date:	25th June 1999

     (C) Broadsword Interactive Ltd. */

#ifndef __STACK_H__
#define __STACK_H__

// The stack works as a linked list.
// It emulates a FILO by creating new nodes at
// the beginning of the list.

class StackNode
{
public:
	StackNode(void *_Data, StackNode *_NextNode);

	// Return the address of the next link.
	StackNode *NextLink();

	// Return the data held.
	void *GetData();
private:
	// Here's the data.
	void *Data;

	// And here's the next link in the chain.
	StackNode *NextNode;
};

class Stack
{
public:
	Stack();

	// Push an address onto the stack.
	Push(void *_Data);

	// And pop it off again.
	void *Pop();

private:
	StackNode *NodeList;
};

#endif