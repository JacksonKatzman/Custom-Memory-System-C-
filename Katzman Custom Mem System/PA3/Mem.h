//-----------------------------------------------------------------------------
// Jackson Katzman
// Optimized C++
//----------------------------------------------------------------------------- 

#ifndef MEM_H
#define MEM_H

#include "Heap.h"

class Mem
{
public:
	static const unsigned int HEAP_SIZE = (50 * 1024);


public:
	Mem();
	Mem(const Mem &) = delete;
	Mem & operator = (const Mem &) = delete;
	~Mem();

	Heap *GetHeap();
	void Dump();

	// implement these functions
	void Free(void * const data);
	void *Malloc(const Type::U32 size);
	FreeHdr * CombineAdjacentFreeBlocks(FreeHdr* above, FreeHdr* below);
	void CreateSecretPointer(FreeHdr * p, Type::U32 size);
	void Initialize();


private:
	Heap * pHeap;
	void	*pRawMem;

};

#endif 

// ---  End of File ---------------
