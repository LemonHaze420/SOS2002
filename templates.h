#ifndef _TEMPLATES_H_
#define _TEMPLATES_H_

#include "BAssert.h"
#include <stdlib.h>
#include <string.h>

template <class TYPE>
class DynamicArray
	{
	private:
		TYPE *data;
		long current, maxItems;

		void Increase()
		{
			// up the amount of memory used for this array
			long pos;
			long oldMax = maxItems;
			TYPE *tempData = data;
			maxItems = maxItems * 2 + 1;
			data = (TYPE*)malloc( sizeof( TYPE ) * maxItems );
			memset( data, 0, sizeof( TYPE ) * maxItems );
			for( pos = 0; pos < oldMax; ++pos )
			{
				data[ pos ] = tempData[ pos ];
			}
			if( tempData != NULL )
				free( tempData );
		}
		
	public:
		long GetNumItems() { return current; }
		TYPE GetItem( const long &index ) { Assert( index < current && index >= 0 ); return data[ index ]; }
		bool NotNULL( const long &index ) { Assert( index >= 0 ); if( index >= current ) return false; return data[ index ] != NULL; }
		long FindItemIndex( const TYPE &reference )
			{
			long pos;

			for( pos = 0; pos < current; ++pos )
				{
				if( data[ pos ] == reference )
					return pos;
				}

			Assert( 0 == "could not find the array element" );
			return -1;
			}
	public: // operators
		TYPE& operator[]( const long &index )
		{
			if( current <= index )
			{
				current = index + 1;
				while( maxItems < current )
					Increase();
			}
			Assert( index >= 0 );
			return data[ index ];
		}

	public: // constructors
		DynamicArray() : data( NULL ), current( 0 ), maxItems( 0 ) {}
		void Init() { if( data != NULL ) { free( data ); data = NULL; } current = 0; maxItems = 0; }
		~DynamicArray() { if( data != NULL ) free( data ); }
	};

template <class TYPE>
class TStack
	{
	private:
		enum { EMPTY = -1, DEFAULT_MAX = 512 };
		TYPE *data;
		long maxLen, top;

	public:
		TStack() : maxLen( DEFAULT_MAX ), top( EMPTY ) { data = new TYPE[ maxLen ]; Assert( data != NULL ); }
		TStack( long size ) : maxLen( size ), top( EMPTY ) { data = new TYPE[ maxLen ]; Assert( data != NULL ); }
		~TStack() { if( data != NULL ) { delete []data; } }

	public:
		bool IsValid() { if( top >= maxLen ) return false; if( data == NULL ) return false; return true; }
		void Clear() { top = EMPTY; }
		void Push( const TYPE &val ) { data[ ++top ] = val; }
		TYPE Pop() { return data[ top-- ]; }
		TYPE Top() { return data[ top ]; }
		void Skip() { top--; }
		long GetNumItems() { return top + 1; }
	};

template <class TYPE, long MaxSize>
	class ConstTStack
	{
	private:
		enum { EMPTY = -1 };
		TYPE data[ MaxSize ];
		long top;

	public:
		ConstTStack() : top( EMPTY ) { }
		~ConstTStack() { }

	public:
		bool IsValid() { if( top >= MaxSize ) return false; return true; }
		void Clear() { top = EMPTY; }
		void Push( const TYPE &val ) { data[ ++top ] = val; }
		TYPE Pop() { return data[ top-- ]; }
		TYPE Top() { return data[ top ]; }
		void Skip() { --top; }
		long GetNumItems() { return top + 1; }
		void UnPop() {  ++top; }
	};

template <class TYPE>
class Llist
	{
	private:
		typedef struct _Link
			{
			TYPE item;
			_Link *next;
			} Link;
	private:
		Link *start;
		Link *end;

	public:
		Llist() : start( NULL ), end( NULL ) { }
		Llist( const TYPE &item ) : start( NULL ), end( NULL )
			{ start = new Link; end = start; start->item = item; start->next = NULL; }
		~Llist()
			{
			while( start != NULL )
				{
				end = start->next;
				delete start;
				start = end;
				}
			}
	public:
		void AddItem( const TYPE &item ) {
			if( start != NULL ) { end->next = new Link; end = end->next; end->next = NULL; end->item = item; }
			else                { start = end = new Link; start->next = NULL; start->item = item; } }
		void DeleteID( const long &itemID )
			{
			Link *current;
			Link **last;
			long count = 0;
			last = &start;
			current = start;
			while( current != NULL )
				{
				if( count == itemID )
					{
					*last = current->next;
					if( end == current )
						end = NULL;
					delete current;
					return;
					}
				last = &( current->next );
				current = current->next;
				count++;
				}
			}
		void DeleteItem( const TYPE &item )
			{
			Link *current;
			Link **last;
			last = &start;
			current = start;
			while( current != NULL )
				{
				if( current->item == item )
					{
					*last = current->next;
					if( end == current )
						end = NULL;
					delete current;
					return;
					}
				last = &( current->next );
				current = current->next;
				}
			}
		const TYPE &GetItem( const long &itemID )
			{
			Link *current;
			long count = itemID;
			current = start;
			while( current != NULL && count > 0)
				{
				current = current->next;
				count--;
				}
			return current->item;
			}
		long GetID( const TYPE &item )
			{
			Link *current;
			long count = 0;
			current = start;
			while( current != NULL )
				{
				if( current->item == item )
					{
					return count;
					}
				current = current->next;
				count++;
				}
			return -1;
			}
		long GetNumItems()
			{
			Link *current;
			long count = 0;
			current = start;
			while( current != NULL )
				{
				current = current->next;
				count++;
				}
			return count;
			}
	};
		
template <class TYPE>
class Queue
	{
	private:
		typedef struct _Link
			{
			TYPE item;
			_Link *previous;
			} Link;

	private:
		Link *start;
		Link *end;

	public:
		Queue() : start( NULL ), end( NULL ) { }
		~Queue()
			{
			while( end != NULL )
				{
				start = end->previous;
				delete end;
				end = start;
				}
			}

	public:
		long GetNumItems()
			{
			Link *current;
			long count = 0;
			current = end;
			while( current != NULL )
				{
				current = current->previous;
				count++;
				}
			return count;
			}
		void Push( const TYPE &item )
			{ 
			Link *temp;
			temp = new Link;
			if( start != NULL )
				{
				start->previous = temp;
				}
			start = temp;
			start->previous = NULL;
			if( end == NULL )
				end = temp;
			temp->item = item;
			}
		const TYPE &Top() { return end->item; }
		TYPE Pop() { TYPE itemTemp = end->item; Link *temp = end->previous; delete end; end = temp; return itemTemp; }
	};

#define WAREHOUSE_NULL_ID (-1)

template <class TYPE>
class Warehouse // returns the ID of the location it stored a pointer to an object of type TYPE.
// lets you free and retreive data using this ID.
{
private:
	DynamicArray<TYPE*> pointerArray;
	TStack<long> freeIDs;
public:
	Warehouse() { }
	~Warehouse() { }
public:
	long GetNumItems()
	{
		Assert( freeIDs.GetNumItems() <= pointerArray.GetNumItems() );
		return pointerArray.GetNumItems() - freeIDs.GetNumItems();
	}
	long AddItem( TYPE *item )
	{
		long ID;
		if( freeIDs.GetNumItems() )
		{
			ID = freeIDs.Pop();
		}
		else
		{
			ID = pointerArray.GetNumItems();
		}
		pointerArray[ ID ] = item;
		return ID;
	}
	TYPE *GetItem( long ID )
	{
		return pointerArray[ ID ];
	}
	TYPE *operator[]( const long &ID )
	{
		Assert( ID < pointerArray.GetNumItems() );
		Assert( pointerArray[ ID ] != NULL );
		return pointerArray[ ID ];
	}
	bool RemoveItem( long ID )
	{
		if( ( ID >= pointerArray.GetNumItems() ) ||
			( pointerArray[ ID ] == NULL ) )
			return false;
		pointerArray[ ID ] = NULL;
		freeIDs.Push( ID );
		return true;
	}
	long GetFirst() // returns the first item in a warehouse. or WAREHOUSE_NULL_ID for failed
	{
		Assert( GetNumItems() >= 0 );
		if( GetNumItems() == 0 )
			return WAREHOUSE_NULL_ID;
		long ID = 0;
		while( pointerArray[ ID ] == NULL )
		{
			++ID;
			Assert( ID < pointerArray.GetNumItems() );
		}
		return ID;
	}
	long GetNext( long ID ) // returns the next, or WAREHOUSE_NULL_ID if no more.
	{
		Assert( GetNumItems() > 0 );
		++ID;
		while( ( ID < pointerArray.GetNumItems() ) && ( pointerArray[ ID ] == NULL ) )
		{
			++ID;
		}
		if( ID >= pointerArray.GetNumItems() )
		{
			return WAREHOUSE_NULL_ID;
		}
		return ID;
	}
};

template <class TYPE>
class PtrIDList // all IDs are 32bit, and all pointers are of type (TYPE*)
	{
	private:
		TYPE **pointerList;
		long *IDList; // when a pointer is null, the ID is an offset to the next null pointer (e.g. an empty chain)
		long current, maxItems;
		long firstEmpty; // the offset of the first empty ID.
		long *offsetList; // the array indices of the pointers at ID n (e.g. to get pointer with ID 4, use pointerList[ offsetList[ 4 ] ];

	public:

	public: // constructors
		PtrIDList() : pointerList( NULL ), IDList( 0 ), current( 0 ), maxItems( 0 ), firstEmpty( 0 ), offsetList( NULL ) {}
		~PtrIDList()
			{
			if( pointerList != NULL )
				free( pointerList );
			if( IDList != NULL )
				free( IDList );
			if( offsetList != NULL )
				free( offsetList );
			}
	};

template <class TYPE>
struct HierarchyNode
	{
	HierarchyNode *parent;
	HierarchyNode *child;
	//HierarchyNode *prevSibling; // asdf use this to speed up stuff when i get a chance.
	HierarchyNode *sibling;
	TYPE *data;
	};

template <class TYPE>
class Hierarchy
	{
	private:
		HierarchyNode<TYPE> *root;

		void DestroyNodeChildren( HierarchyNode<TYPE> *node )
			{
			HierarchyNode<TYPE> *nodePtr;
			while( node != NULL )
				{
				if( node->child != NULL )
					{
					DestroyNodeChildren( node->child );
					}
				nodePtr = node->sibling;
				free( node );
				node = nodePtr;
				}
			}

	public:
		HierarchyNode<TYPE> *GetRoot() { return root; }

		HierarchyNode<TYPE> *FindPreviousSibling( HierarchyNode<TYPE> *node )
			{
			if( node->parent == NULL || node->parent->child == node ) return NULL;
			
			HierarchyNode<TYPE> *oldNodePtr = node->parent->child;
			HierarchyNode<TYPE> *nodePtr = oldNodePtr->sibling;
			while( nodePtr != NULL && nodePtr != node )
				{
				oldNodePtr = nodePtr;
				nodePtr = oldNodePtr->sibling;
				}
			if( nodePtr != NULL )
				{
				Assert( nodePtr == node );
				return oldNodePtr;
				}
			return NULL;
			}

		void DestroyNode( HierarchyNode<TYPE> *node )
			{
			HierarchyNode<TYPE> *nodePtr = FindPreviousSibling( node );

			if( node->child != NULL )
				{
				DestroyNodeChildren( node->child );
				}

			if( nodePtr != NULL )
				{
				nodePtr->sibling;
				}
			}

		HierarchyNode<TYPE> *GetChild( HierarchyNode<TYPE> *node )
			{
			return node->child;
			}

		HierarchyNode<TYPE> *GetChild( HierarchyNode<TYPE> *node, long index )
			{
			HierarchyNode<TYPE> *nodePtr = node->child;
			while( index-- ) { nodePtr = nodePtr->sibling; }
			return nodePtr;
			}

		HierarchyNode<TYPE> *GetSibling( HierarchyNode<TYPE> *node )
			{
			return node->sibling;
			}


		const HierarchyNode<TYPE> *GetChild( const HierarchyNode<TYPE> *node ) const
			{
			return node->child;
			}

		const HierarchyNode<TYPE> *GetChild( const HierarchyNode<TYPE> *node, long index ) const
			{
			const HierarchyNode<TYPE> *nodePtr = node->child;
			while( index-- ) { nodePtr = nodePtr->sibling; }
			return nodePtr;
			}

		const HierarchyNode<TYPE> *GetSibling( const HierarchyNode<TYPE> *node ) const
			{
			return node->sibling;
			}


	public:
		Hierarchy() { root = NULL; }
		~Hierarchy() { DestroyNode( root ); }
	};

template <class TYPE>
long PointerListLength( TYPE **value )
	{
	long length = 0;
	while( *value != NULL )
		{
		++length;
		++value;
		}
	return length;
	}

#endif