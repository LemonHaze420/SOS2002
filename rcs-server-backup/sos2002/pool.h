/*
	$Id$
	$Log$
	Revision 1.1  1998/12/02 11:38:19  jcf
	Initial revision

*/

// Pool.h: interface for the Pool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POOL_H__7B7EF261_FB8E_11D1_9D7E_00C0DFE30C5D__INCLUDED_)
#define AFX_POOL_H__7B7EF261_FB8E_11D1_9D7E_00C0DFE30C5D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class voidPool
{
protected:
	int itemsize;	// size of each item
	int itemct;		// size of pool
	char *map;		// allocation map of boolean chars
	char *pool;		// pointer to actual data

public:
	voidPool(int sz,int count)
	{
		itemsize=0;
		itemct=0;

		map = (char *)malloc(count);
		memset(map,0,count);

		itemsize = sz;
		itemct = count;
	}

	~voidPool()
	{
		free(map);free(pool);
	}

	void *Allocate()
	{
		for (int i=0;i<itemct;i++)
		{
			if(!map[i])
			{
				map[i]=1;
				return (void *)(pool+itemsize*i);
			}
		}
		panic("out of items in a pool");return NULL;
	}

	void Free(void *item)
	{
		int n = (((char *)item)-pool)/itemsize;
		map[n]=0;
	}

	void Empty()
	{
		memset(map,0,itemct);
	}

};




template<class T> class Pool : public voidPool
{
public:
	Pool(int count) : voidPool(sizeof(T),count)
	{
		pool = (char *)(new T [count]);
	}

	T *Allocate() {
		T *o = (T *)voidPool::Allocate();
		o->Init();
		return o;
	}
	void Free(T *item) { voidPool::Free((void *)item); }
	void Empty() { voidPool::Empty(); }

};





#endif // !defined(AFX_POOL_H__7B7EF261_FB8E_11D1_9D7E_00C0DFE30C5D__INCLUDED_)
