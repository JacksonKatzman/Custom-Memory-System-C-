//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"

#include "Mem.h"

#define PTR_FIX(x)  (unsigned int)(x == 0) ? 0 : ((unsigned int)x - (unsigned int)h) 

TEST(Initialize, MemoryShakeOut)
{
	FileIO::Open("Student_", "Test_01");

	fprintf(FileIO::GetHandle(), "Test 1: \n\n");

	fprintf(FileIO::GetHandle(), "     Mem mem;               \n");
	fprintf(FileIO::GetHandle(), "     mem.Initialize();      \n");
	fprintf(FileIO::GetHandle(), "     mem.Dump();            \n");

	Mem mem;
	mem.Initialize();
	mem.Dump();

	FileIO::Close();

	// ----  GENERAL CHECK ------------------------------------------------------

		Heap *h = mem.GetHeap();

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL(PTR_FIX(h), 0);

		// Heap Start / Heap Bottom
		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x40);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL(h->pUsedHead, 0);
		CHECK_EQUAL(PTR_FIX(h->pFreeHead), 0x40);
		CHECK_EQUAL(PTR_FIX(h->pNextFit), 0x40);

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL(h->mStats.peakNumUsed, 0);
		CHECK_EQUAL(h->mStats.peakUsedMemory, 0);

		CHECK_EQUAL(h->mStats.currNumUsedBlocks, 0);
		CHECK_EQUAL(h->mStats.currUsedMem, 0);

		CHECK_EQUAL(h->mStats.currNumFreeBlocks, 1);
		CHECK_EQUAL(h->mStats.currFreeMem, 0xc7b0);

		CHECK_EQUAL(h->mStats.sizeHeap, 0xc800);

		CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), 0x40);
		CHECK_EQUAL(PTR_FIX(h->mStats.heapBottomAddr), 0xc800);


	// ---- Heap Walk ------------------------------------------------------

		// ---- Heap Hdr -------------------------------------------------------

			// HeapHdr Start
			CHECK_EQUAL(PTR_FIX((Type::U32)h->mStats.heapTopAddr - sizeof(Heap)), 0);
			// HeapHdr End
			CHECK_EQUAL(PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap));


		// ----  Block walk ------------------------------------------------------

			Type::U32 hdrStart;
			Type::U32 hdrEnd;
			Type::U32 blkStart;
			Type::U32 blkEnd;

		// ---- HDR -------------------------------------------

			// Check type
			FreeHdr *free = (FreeHdr *)h->mStats.heapTopAddr;
			// Should be FREE
			CHECK_EQUAL((int)free->mBlockType, (int)BlockType::FREE);
			// Above FreeHdr
			CHECK_EQUAL(free->mAboveBlockFree, false);

			hdrStart = (Type::U32)free;
			hdrEnd = (Type::U32)free + sizeof(FreeHdr);

			// Hdr Start
			CHECK_EQUAL(PTR_FIX(hdrStart), 0x40);
			// Hdr End
			CHECK_EQUAL(PTR_FIX(hdrEnd), 0x50);
			// Prev
			CHECK_EQUAL(free->pFreePrev, 0);
			// Next
			CHECK_EQUAL(free->pFreeNext, 0);

			// Hdr Size
			CHECK_EQUAL(hdrEnd - hdrStart, sizeof(FreeHdr));

		// ---- BLOCK ----------------------------------------------------

			blkStart = hdrEnd;
			blkEnd = blkStart + free->mBlockSize;

			CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
			CHECK_EQUAL(PTR_FIX(blkEnd), 0xc800);
			CHECK_EQUAL(free->mBlockSize, 0xc7b0);

}

// ---  End of File ---------------
