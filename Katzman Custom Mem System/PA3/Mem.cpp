//-----------------------------------------------------------------------------
// Jackson Katzman
// Optimized C++
//----------------------------------------------------------------------------- 

#include <malloc.h>
#include <new>

#include "Framework.h"

#include "Mem.h"
#include "Heap.h"
#include "BlockType.h"

#define STUB_PLEASE_REPLACE(x) (x)

#define HEAP_ALIGNMENT			16
#define HEAP_ALIGNMENT_MASK		(HEAP_ALIGNMENT-1)

#define ALLOCATION_ALIGNMENT		16
#define ALLOCATION_ALIGNMENT_MASK	(ALLOCATION_ALIGNMENT-1)

#define UNUSED_VAR(v)  ((void *)v)

#ifdef _DEBUG
#define HEAP_HEADER_GUARDS  16
#define HEAP_SET_GUARDS  	Type::U32 *pE = (Type::U32 *)((Type::U32)pRawMem + HEAP_SIZE); \
								*pE++ = 0xEEEEEEEE;*pE++ = 0xEEEEEEEE;*pE++ = 0xEEEEEEEE;*pE++ = 0xEEEEEEEE;
#define HEAP_TEST_GUARDS	Type::U32 *pE = (Type::U32 *)((Type::U32)pRawMem + HEAP_SIZE); \
								assert(*pE++ == 0xEEEEEEEE);assert(*pE++ == 0xEEEEEEEE); \
								assert(*pE++ == 0xEEEEEEEE);assert(*pE++ == 0xEEEEEEEE);  
#else
#define HEAP_HEADER_GUARDS  0
#define HEAP_SET_GUARDS  	
#define HEAP_TEST_GUARDS			 
#endif
#define TESTNEW 1

// To help with coalescing... not required
struct SecretPtr
{
	FreeHdr *free;
};


Mem::~Mem()
{
	HEAP_TEST_GUARDS
		_aligned_free(this->pRawMem);
}


Heap *Mem::GetHeap()
{
	return this->pHeap;
}

Mem::Mem()
{
	// now initialize it.
	this->pHeap = 0;
	this->pRawMem = 0;

	// Do a land grab --- get the space for the whole heap
	// Since OS have different alignments... I forced it to 16 byte aligned
	pRawMem = _aligned_malloc(HEAP_SIZE + HEAP_HEADER_GUARDS, HEAP_ALIGNMENT);
	HEAP_SET_GUARDS

		// verify alloc worked
		assert(pRawMem != 0);

	// Guarantee alignemnt
	assert(((Type::U32)pRawMem & HEAP_ALIGNMENT_MASK) == 0x0);

	// instantiate the heap header on the raw memory
	Heap *p = new(pRawMem) Heap(pRawMem);

	// update it
	this->pHeap = p;
}


void Mem::Initialize()
{
	if (GetHeap()->pFreeHead == 0)
	{
		void* freeHdrLoc = (void*)((Type::U32)pRawMem + sizeof(Heap));
		FreeHdr *p = new(freeHdrLoc) FreeHdr;
		GetHeap()->pFreeHead = p;
		GetHeap()->pFreeHead->pFreePrev = 0;
		GetHeap()->pFreeHead->pFreeNext = 0;
		GetHeap()->pFreeHead->mBlockSize = HEAP_SIZE - (Type::U32)sizeof(Heap) - (Type::U32)sizeof(FreeHdr);
		GetHeap()->pFreeHead->mBlockType = 0xCC;
		GetHeap()->pFreeHead->mAboveBlockFree = false;
		GetHeap()->pNextFit = p;
		GetHeap()->mStats.currNumFreeBlocks = 1;
		GetHeap()->mStats.currFreeMem = GetHeap()->pFreeHead->mBlockSize;
		GetHeap()->mStats.sizeHeap = HEAP_SIZE;
	}
	GetHeap()->pUsedHead = 0;
}

//Important next step: Dividing subsections on Malloc into separate function calls
//in order to increase readability and organization.
void *Mem::Malloc(const Type::U32 size)
{
	Type::U32 UHSize = sizeof(UsedHdr);
	//Try to allocate to next fit
	if (size <= GetHeap()->pNextFit->mBlockSize && GetHeap()->pNextFit != GetHeap()->pFreeHead)
	{
		FreeHdr *myPrev = GetHeap()->pNextFit->pFreePrev;
		FreeHdr *myNext = GetHeap()->pNextFit->pFreeNext;
		UsedHdr *p = new(GetHeap()->pNextFit) UsedHdr;
		Type::U32 originalBlockSize = GetHeap()->pNextFit->mBlockSize;
		if (GetHeap()->pNextFit != 0)
		{
			Type::U32 available = GetHeap()->pNextFit->mBlockSize;
			if (size <= available)
			{
				p->mBlockSize = size;
				if (GetHeap()->pUsedHead == 0)
				{
					GetHeap()->pUsedHead = p;
				}
				else
				{
					UsedHdr* x = GetHeap()->pUsedHead;
					x->pUsedPrev = p;
					p->pUsedNext = x;
					GetHeap()->pUsedHead = p;
					GetHeap()->pUsedHead->pUsedPrev = 0;
				}
				//Checking if we are just taking all the memory on the heap
				Type::U32 takingup = size;
				if (takingup == available && GetHeap()->mStats.currNumFreeBlocks == 1)
				{
					GetHeap()->pFreeHead = 0;
					GetHeap()->pNextFit = GetHeap()->pFreeHead;
					GetHeap()->mStats.currNumFreeBlocks = 0;
					GetHeap()->mStats.currFreeMem = 0x0;
					GetHeap()->pUsedHead->mAboveBlockFree = false;
				}
				//we've just taken a whole block, just need to correct our free list to match
				else if (takingup == available)
				{
					UsedHdr* nextHeaderLoc = (UsedHdr*)((Type::U32)GetHeap()->pNextFit + (Type::U32)GetHeap()->pNextFit->mBlockSize + sizeof(UsedHdr));
					nextHeaderLoc->mAboveBlockFree = false;
					UsedHdr* cur = GetHeap()->pUsedHead;
					while (cur != 0)
					{
						cur = cur->pUsedNext;
					}
					//					
					GetHeap()->mStats.currNumFreeBlocks--;
					GetHeap()->mStats.currFreeMem += sizeof(FreeHdr);
					if (myNext != 0)
					{
						myNext->pFreePrev = myPrev;
						GetHeap()->pNextFit = myNext;
					}
					else
					{
						GetHeap()->pNextFit = GetHeap()->pFreeHead;
					}
					if (myPrev != 0)
					{
						myPrev->pFreeNext = myNext;
					}
				}
				//if not allocate and make new free header
				else
				{
					void* FreeHdrLoc = (void*)((Type::U32)p + sizeof(UsedHdr) + size);
					FreeHdr *q = new(FreeHdrLoc) FreeHdr;

					GetHeap()->pNextFit = q;
					GetHeap()->pNextFit->mBlockType = 0xCC;
					GetHeap()->pNextFit->mAboveBlockFree = false;
					if (myPrev != 0)
						myPrev->pFreeNext = q;
					if (myNext != 0)
						myNext->pFreePrev = q;
					GetHeap()->pNextFit->pFreeNext = myNext;
					GetHeap()->pNextFit->pFreePrev = myPrev;
					//Next Fit
					Type::U32 freeBlockSize = (originalBlockSize - (size + sizeof(UsedHdr)));
					GetHeap()->pNextFit->mBlockSize = freeBlockSize;
					CreateSecretPointer(q, freeBlockSize + sizeof(FreeHdr));
				}
				//Housekeeping!
				GetHeap()->mStats.currNumUsedBlocks++;
				if (GetHeap()->mStats.currNumUsedBlocks > GetHeap()->mStats.peakNumUsed)
				{
					GetHeap()->mStats.peakNumUsed = GetHeap()->mStats.currNumUsedBlocks;
				}
				GetHeap()->mStats.currUsedMem += size;
				if (GetHeap()->pFreeHead != 0)
				{
					GetHeap()->mStats.currFreeMem -= size + sizeof(UsedHdr);
				}
				if (GetHeap()->mStats.currUsedMem > GetHeap()->mStats.peakUsedMemory)
				{
					GetHeap()->mStats.peakUsedMemory = GetHeap()->mStats.currUsedMem;
				}
				GetHeap()->pUsedHead->mBlockType = 0xAA;

			}
		}

		return (void *)((Type::U32)p + UHSize);// +sizeof(UsedHdr);
	}
	//Allocate to the FREE Header
	else if (size <= GetHeap()->pFreeHead->mBlockSize)
	{
		FreeHdr *useForLater = GetHeap()->pFreeHead->pFreeNext;
		UsedHdr *p = new(GetHeap()->pFreeHead) UsedHdr;
		if (GetHeap()->pFreeHead != 0)
		{
			Type::U32 available = GetHeap()->pFreeHead->mBlockSize;
			if (size <= available)
			{
				p->mBlockSize = size;
				if (GetHeap()->pUsedHead == 0)
				{
					GetHeap()->pUsedHead = p;
				}
				else
				{
					UsedHdr* x = GetHeap()->pUsedHead;
					x->pUsedPrev = p;
					p->pUsedNext = x;
					GetHeap()->pUsedHead = p;
				}
				//Checking if we are just taking all the memory on the heap
				Type::U32 takingup = size;
				if (takingup == available && GetHeap()->mStats.currNumFreeBlocks == 1)
				{
					GetHeap()->pFreeHead = 0;
					GetHeap()->pNextFit = GetHeap()->pFreeHead;
					GetHeap()->mStats.currNumFreeBlocks = 0;
					GetHeap()->mStats.currFreeMem = 0x0;
					GetHeap()->pUsedHead->mAboveBlockFree = false;
				}
				//we've just taken a whole block, just need to correct our free list to match
				else if (takingup == available)
				{
					UsedHdr* nextHeaderLoc = (UsedHdr*)((Type::U32)GetHeap()->pFreeHead + (Type::U32)GetHeap()->pFreeHead->mBlockSize + sizeof(UsedHdr));
					nextHeaderLoc->mAboveBlockFree = false;
					// Print out the used header addresses
					UsedHdr* cur = GetHeap()->pUsedHead;
					while (cur != 0)
					{
						cur = cur->pUsedNext;
					}
					//					
					GetHeap()->mStats.currNumFreeBlocks--;
					GetHeap()->mStats.currFreeMem += sizeof(FreeHdr);
					GetHeap()->pFreeHead = useForLater;
					//Next Fit
					GetHeap()->pNextFit = GetHeap()->pFreeHead;
					if (GetHeap()->pFreeHead != 0)
					{
						GetHeap()->pFreeHead->pFreePrev = 0;
						GetHeap()->pFreeHead->mBlockType = 0xCC;
					}

				}
				//if not allocate and make new free header
				else
				{
					void* FreeHdrLoc = (void*)((Type::U32)p + sizeof(UsedHdr) + size);
					FreeHdr *q = new(FreeHdrLoc) FreeHdr;

					GetHeap()->pFreeHead = q;
					GetHeap()->pFreeHead->mBlockType = 0xCC;
					GetHeap()->pFreeHead->mAboveBlockFree = false;
					GetHeap()->pFreeHead->pFreeNext = useForLater;
					if (useForLater != 0)
						useForLater->pFreePrev = GetHeap()->pFreeHead;
					GetHeap()->pFreeHead->pFreePrev = 0;
					//Next Fit
					GetHeap()->pNextFit = GetHeap()->pFreeHead;
					Type::U32 freeBlockSize = (available - takingup) - sizeof(FreeHdr);
					q->mBlockSize = freeBlockSize;
					CreateSecretPointer(q, freeBlockSize + sizeof(FreeHdr));
				}
				//Housekeeping!
				GetHeap()->mStats.currNumUsedBlocks++;
				if (GetHeap()->mStats.currNumUsedBlocks > GetHeap()->mStats.peakNumUsed)
				{
					GetHeap()->mStats.peakNumUsed = GetHeap()->mStats.currNumUsedBlocks;
				}
				GetHeap()->mStats.currUsedMem += size;
				if (GetHeap()->pFreeHead != 0)
				{
					GetHeap()->mStats.currFreeMem -= size + sizeof(UsedHdr);
				}
				if (GetHeap()->mStats.currUsedMem > GetHeap()->mStats.peakUsedMemory)
				{
					GetHeap()->mStats.peakUsedMemory = GetHeap()->mStats.currUsedMem;
				}
				GetHeap()->pUsedHead->mBlockType = 0xAA;

			}
		}

		return (void *)((Type::U32)p + UHSize);;
	}
	else
	{
		//FIND FIRST FIT - Jumping ahead here but should work in the event that theres more than one Malloc call
		FreeHdr* finder = GetHeap()->pFreeHead->pFreeNext;
		while (finder != 0)
		{
			//Check if it will fit here
			if (finder->mBlockSize >= size)
			{
				Type::U32 finderSize = finder->mBlockSize;
				//relinking our free list
				FreeHdr* prev = finder->pFreePrev;
				FreeHdr* next = finder->pFreeNext;
				if (next != 0)
					next->pFreePrev = prev;
				if (prev != 0)
					prev->pFreeNext = next;
				//make a new used block
				UsedHdr *p = new(finder) UsedHdr;
				p->mBlockSize = size;
				p->pUsedNext = GetHeap()->pUsedHead;
				p->pUsedNext->pUsedPrev = p;
				p->pUsedPrev = 0;
				p->mBlockType = 0xAA;
				GetHeap()->pUsedHead = p;
				GetHeap()->mStats.currNumFreeBlocks--;
				//This is a check to detirmine whether we are eating the entire free block or just a portion.
				//If the entire free block isn't eaten we will need a new free header after this used one.
				if (finderSize > size)
				{
					void* FreeHdrLoc = (void*)((Type::U32)p + sizeof(UsedHdr) + size);
					FreeHdr *q = new(FreeHdrLoc) FreeHdr;
					Type::U32 freeBlockSize = (finderSize - size) - sizeof(FreeHdr);
					q->pFreePrev = prev;
					q->pFreeNext = next;
					if (q->pFreePrev != 0)
						q->pFreePrev->pFreeNext = q;
					if (q->pFreeNext != 0)
						q->pFreeNext->pFreePrev = q;
					q->mBlockSize = freeBlockSize;
					q->mAboveBlockFree = false;
					q->mBlockType = 0xCC;
					GetHeap()->pNextFit = q;
					GetHeap()->mStats.currNumUsedBlocks++;
					GetHeap()->mStats.currNumFreeBlocks++;

					CreateSecretPointer(q, freeBlockSize + sizeof(FreeHdr));

					//Stats housekeeping

					if (GetHeap()->mStats.currNumUsedBlocks > GetHeap()->mStats.peakNumUsed)
					{
						GetHeap()->mStats.peakNumUsed = GetHeap()->mStats.currNumUsedBlocks;
					}
					GetHeap()->mStats.currUsedMem += size;
					if (GetHeap()->pFreeHead != 0)
					{
						GetHeap()->mStats.currFreeMem -= size + sizeof(UsedHdr);
					}

					if (GetHeap()->mStats.currUsedMem > GetHeap()->mStats.peakUsedMemory)
					{
						GetHeap()->mStats.peakUsedMemory = GetHeap()->mStats.currUsedMem;
					}

					return (void *)((Type::U32)p + UHSize);
				}
				return (void *)((Type::U32)p + UHSize);
			}
			//Didn't find one, walk the list!
			finder = finder->pFreeNext;
		}
		return 0;
	}
}

//This will hopefully work to take free blocks that are next to each other and combine them.
//Done by removing the lower free block's header and adding that free space to the one above.
FreeHdr * Mem::CombineAdjacentFreeBlocks(FreeHdr* above, FreeHdr* below)
{
	//Add the sizes together plus the removed header size
	above->mBlockSize += below->mBlockSize + sizeof(FreeHdr);
	if (GetHeap()->mStats.currNumFreeBlocks > 1)
	{
		GetHeap()->mStats.currNumFreeBlocks--;
	}
	GetHeap()->mStats.currFreeMem += sizeof(FreeHdr);
	above->mAboveBlockFree = false;
	return above;
}

void Mem::Free(void * const data)
{
	//Grab the used header we intend to free
	UsedHdr *p = (UsedHdr*)data;
	p = (UsedHdr *)((Type::U32)p - sizeof(UsedHdr));
	bool aboveFree = p->mAboveBlockFree;
	unsigned int bSize = p->mBlockSize;
	UsedHdr* usedBlockBelow = (UsedHdr *)((Type::U32)p + sizeof(UsedHdr) + bSize);
	if (usedBlockBelow->mBlockType == 0xAA)
	{
		usedBlockBelow->mAboveBlockFree = true;
	}
	GetHeap()->mStats.currNumFreeBlocks++;

	//Check if it is the head
	if (p == GetHeap()->pUsedHead)
	{
		if (p->pUsedNext != 0)
		{
			GetHeap()->pUsedHead = p->pUsedNext;

		}
		GetHeap()->pUsedHead->pUsedPrev = 0;
	}
	//If not, relink the list
	else
	{
		if (p->pUsedPrev != 0)
		{
			p->pUsedPrev->pUsedNext = p->pUsedNext;
		}
		if (p->pUsedNext != 0)
		{
			p->pUsedNext->pUsedPrev = p->pUsedPrev;
		}
	}

	FreeHdr *q = new(p) FreeHdr;
	q->pFreeNext = 0;
	q->pFreePrev = 0;
	q->mBlockSize = bSize;
	q->mBlockType = 0xCC;
	q->mAboveBlockFree = false;

	FreeHdr* freeBlockBelow = (FreeHdr *)((Type::U32)q + sizeof(FreeHdr) + bSize);

	void* SecretHeaderPlacement = (void*)((Type::U32)q - 4);
	SecretPtr* secret = new(SecretHeaderPlacement) SecretPtr;
	FreeHdr * freeBlockAbove = secret->free;

	bool haveReference = false;

	if (aboveFree)
	{
		q = CombineAdjacentFreeBlocks(freeBlockAbove, q);
		haveReference = true;
	}
	if (freeBlockBelow->mBlockType == 0xCC)
	{
		if (freeBlockBelow == GetHeap()->pNextFit)
		{
			GetHeap()->pNextFit = q;
		}
		q->pFreeNext = freeBlockBelow->pFreeNext;
		if (!haveReference)
			q->pFreePrev = freeBlockBelow->pFreePrev;
		if (q->pFreePrev != 0)
		{
			q->pFreePrev->pFreeNext = q;
		}
		if (q->pFreeNext != 0)
		{
			q->pFreeNext->pFreePrev = q;
		}
		if (freeBlockBelow == GetHeap()->pFreeHead)
		{
			GetHeap()->pFreeHead = q;
		}
		q = CombineAdjacentFreeBlocks(q, freeBlockBelow);
		if (GetHeap()->mStats.currNumFreeBlocks == 1)
		{
			q->pFreeNext = 0;
			q->pFreePrev = 0;
			GetHeap()->pFreeHead = q;
		}
		haveReference = true;
	}
	else if (((UsedHdr*)freeBlockBelow)->mBlockType == 0xAA)
	{
		((UsedHdr*)freeBlockBelow)->mAboveBlockFree = true;
	}

	if (!haveReference)
	{
		if (GetHeap()->pFreeHead == 0)
		{
			GetHeap()->pFreeHead = q;
			GetHeap()->pFreeHead->pFreeNext = 0;
			GetHeap()->pFreeHead->pFreePrev = 0;
			//Next Fit
			GetHeap()->pNextFit = GetHeap()->pFreeHead;
		}
		else if (q < GetHeap()->pFreeHead)
		{
			q->pFreeNext = GetHeap()->pFreeHead;
			GetHeap()->pFreeHead->pFreePrev = q;
			GetHeap()->pFreeHead = q;
		}
		else
		{
			//Last Resort - We have no idea where we are supposed to be in the linked list of frees AND we are isolated with no reference. 
			//Last Resort - Walk the list to find where we should be?
			FreeHdr *find = GetHeap()->pFreeHead;
			bool found = false;
			while (!found)
			{
				if (q < find)
				{
					q->pFreePrev = find->pFreePrev;
					q->pFreePrev->pFreeNext = q;
					q->pFreeNext = find;
					find->pFreePrev = q;
					found = true;
				}
				else
				{
					if (find->pFreeNext != 0)
						find = find->pFreeNext;
				}
			}
			if (!found)
			{
				find->pFreeNext = q;
				q->pFreePrev = find;
			}
		}
	}

	//Now that everything is done, we need to create a SecretPointer for later methods to find
	CreateSecretPointer(q, q->mBlockSize + sizeof(FreeHdr));
	//House keeping to make sure stats reflect changes
	GetHeap()->mStats.currNumUsedBlocks--;
	if (GetHeap()->mStats.currNumUsedBlocks <= 0)
	{
		GetHeap()->pUsedHead = 0;
		GetHeap()->pFreeHead->pFreeNext = 0;
	}
	GetHeap()->mStats.currUsedMem -= bSize;
	GetHeap()->mStats.currFreeMem += bSize;
	GetHeap()->pFreeHead->mBlockType = 0xCC;
	//Now IF this free block happens to be the ONLY free block then it is the next fit.
	if (q->mBlockSize == GetHeap()->mStats.currFreeMem)
	{
		GetHeap()->pNextFit = q;
	}
}


void Mem::Dump()
{

	fprintf(FileIO::GetHandle(), "\n------- DUMP -------------\n\n");

	fprintf(FileIO::GetHandle(), "heapStart: 0x%p     \n", this->pHeap);
	fprintf(FileIO::GetHandle(), "  heapEnd: 0x%p   \n\n", this->pHeap->mStats.heapBottomAddr);
	fprintf(FileIO::GetHandle(), "pUsedHead: 0x%p     \n", this->pHeap->pUsedHead);
	fprintf(FileIO::GetHandle(), "pFreeHead: 0x%p     \n", this->pHeap->pFreeHead);
	fprintf(FileIO::GetHandle(), " pNextFit: 0x%p   \n\n", this->pHeap->pNextFit);

	fprintf(FileIO::GetHandle(), "Heap Hdr   s: %p  e: %p                            size: 0x%x \n", (void *)((Type::U32)this->pHeap->mStats.heapTopAddr - sizeof(Heap)), this->pHeap->mStats.heapTopAddr, sizeof(Heap));

	Type::U32 p = (Type::U32)pHeap->mStats.heapTopAddr;

	char *type;
	char *typeHdr;

	while (p < (Type::U32)pHeap->mStats.heapBottomAddr)
	{
		UsedHdr *used = (UsedHdr *)p;
		if (used->mBlockType == (Type::U8)BlockType::USED)
		{
			typeHdr = "USED HDR ";
			type = "USED     ";
		}
		else
		{
			typeHdr = "FREE HDR ";
			type = "FREE     ";
		}

		Type::U32 hdrStart = (Type::U32)used;
		Type::U32 hdrEnd = (Type::U32)used + sizeof(UsedHdr);
		fprintf(FileIO::GetHandle(), "%s  s: %p  e: %p  p: %p  n: %p  size: 0x%x    AF: %d \n", typeHdr, (void *)hdrStart, (void *)hdrEnd, used->pUsedPrev, used->pUsedNext, sizeof(UsedHdr), used->mAboveBlockFree);
		Type::U32 blkStart = hdrEnd;
		Type::U32 blkEnd = blkStart + used->mBlockSize;
		fprintf(FileIO::GetHandle(), "%s  s: %p  e: %p                            size: 0x%x \n", type, (void *)blkStart, (void *)blkEnd, used->mBlockSize);

		p = blkEnd;

	}
}



void Mem::CreateSecretPointer(FreeHdr * p, Type::U32 size)
{
	void* SecretHeaderPlacement = (void*)((Type::U32)p + size - 4);
	SecretPtr* secret = new(SecretHeaderPlacement) SecretPtr;
	secret->free = p;
}
// ---  End of File ---------------
