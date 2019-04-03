//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"

#include "Mem.h"

#define PTR_FIX(x)  (unsigned int)(x == 0) ? 0: ((unsigned int)x - (unsigned int)h) 
#define UNUSED_VAR(p) ((void *)p)


TEST( Partial_allocate , MemoryShakeOut )
{
	FileIO::Open("Student_", "Test_04");

	fprintf(FileIO::GetHandle(),"Test4: \n\n");

	fprintf(FileIO::GetHandle()," 1) allocate regular size block        \n");

	fprintf(FileIO::GetHandle(),"     Mem mem;                          \n");
	fprintf(FileIO::GetHandle(),"     mem.Initialize();                 \n");
   	fprintf(FileIO::GetHandle(),"     void *p = mem.Malloc( 200 );      \n");
	fprintf(FileIO::GetHandle(),"     mem.Dump();                       \n");

	Mem mem1;
	mem1.Initialize();
   	void *p1 = mem1.Malloc( 0x200 );
	mem1.Dump();

	// Shut up warnings
	UNUSED_VAR(p1);

	FileIO::Close();


	// ----  GENERAL CHECK ------------------------------------------------------

	// ---- Part A: -------------------------------------------------------------
	
	Mem mem;
	mem.Initialize();
   	void *p = mem.Malloc( 0x200 );

	// ---- Verify A: -----------------------------------------------------------

		Heap *h = mem.GetHeap();

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x40 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x250 );
		CHECK_EQUAL( PTR_FIX(h->pNextFit), 0x250);
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x40);

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 1 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0x200);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 1 );
		CHECK_EQUAL( h->mStats.currUsedMem, 0x200 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 1 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xc5a0 );

		CHECK_EQUAL( h->mStats.sizeHeap, 0xc800 );

		CHECK_EQUAL( PTR_FIX( h->mStats.heapTopAddr), 0x40 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), 0xc800 );


	// ---- Heap Walk ------------------------------------------------------

		// ---- Heap Hdr -------------------------------------------------------

			// HeapHdr Start
			CHECK_EQUAL( PTR_FIX( (Type::U32)h->mStats.heapTopAddr-sizeof(Heap) ), 0 );
			// HeapHdr End
			CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap));

		// ----  Block walk ------------------------------------------------------
		
			Type::U32 hdrStart;
			Type::U32 hdrEnd;
			Type::U32 blkStart;
			Type::U32 blkEnd; 

		// ---- USED HDR -------------------------------------------

			// Check type
			UsedHdr *used = (UsedHdr *) h->mStats.heapTopAddr;
			// Should be USED
			CHECK_EQUAL( (int)used->mBlockType, (int)BlockType::USED );
			// Above FreeHdr?
			CHECK_EQUAL(used->mAboveBlockFree, false);
		
			hdrStart = (Type::U32)used;
			hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
			// Hdr Start
			CHECK_EQUAL( PTR_FIX(hdrStart), 0x40  );
			// Hdr End
			CHECK_EQUAL( PTR_FIX(hdrEnd), 0x50 );
			// Prev
			CHECK_EQUAL( used->pUsedPrev, 0 );
			// Next
			CHECK_EQUAL( used->pUsedNext, 0 );
			// Hdr Size
			CHECK_EQUAL( hdrEnd - hdrStart, sizeof(UsedHdr) );
	
		// ---- USED BLOCK ----------------------------------------------------

			blkStart = hdrEnd;
			blkEnd   = blkStart + used->mBlockSize; 
		
			CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
			CHECK_EQUAL( PTR_FIX(blkEnd), 0x250);
			CHECK_EQUAL( PTR_FIX(p), PTR_FIX(hdrEnd));
			CHECK_EQUAL( used->mBlockSize, 0x200 );
	
		// ---- FREE HDR -------------------------------------------

			// Check type
			FreeHdr *free = (FreeHdr *) blkEnd;
			// Should be USED
			CHECK_EQUAL( (int)free->mBlockType, (int)BlockType::FREE );
			// Above FreeHdr
			CHECK_EQUAL(free->mAboveBlockFree, false);
		
			hdrStart = (Type::U32)free;
			hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
			// Hdr Start
			CHECK_EQUAL( PTR_FIX(hdrStart), 0x250 );
			// Hdr End
			CHECK_EQUAL( PTR_FIX(hdrEnd), 0x260 );
			// Prev
			CHECK_EQUAL( free->pFreePrev, 0 );
			// Next
			CHECK_EQUAL( free->pFreeNext, 0 );
			// Hdr Size
			CHECK_EQUAL( hdrEnd - hdrStart, sizeof(FreeHdr) );
	
		// ---- FREE BLOCK ----------------------------------------------------

			blkStart = hdrEnd;
			blkEnd   = blkStart + free->mBlockSize; 
		
			CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
			CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800);
			CHECK_EQUAL( free->mBlockSize, 0xc5a0 );
}

// ---  End of File ---------------
