//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"

#include "Mem.h"

#define PTR_FIX(x)  (unsigned int)(x == 0) ? 0: ((unsigned int)x - (unsigned int)h) 

TEST( Allocate_3_100_middle_up_free , MemoryShakeOut )
{
	FileIO::Open("Student_", "Test_10");


	fprintf(FileIO::GetHandle(),"Test10: \n\n");

	fprintf(FileIO::GetHandle()," 1) used                                              \n");
	fprintf(FileIO::GetHandle()," 2) used                                              \n");
	fprintf(FileIO::GetHandle()," 3) used     (make sure it's consumes 100 percent of heap)             \n");

   	fprintf(FileIO::GetHandle()," 4) middle up   \n\n");


	fprintf(FileIO::GetHandle(),"     Mem mem;                     \n");
	fprintf(FileIO::GetHandle(),"     mem.Initialize();            \n");

   	fprintf(FileIO::GetHandle(),"     void *a = mem.Malloc( 0x4280 );       \n");
   	fprintf(FileIO::GetHandle(),"     void *b = mem.Malloc( 0x4290 );       \n");
	fprintf(FileIO::GetHandle(),"     void *c = mem.Malloc( 0x4280 );       \n");
       
	fprintf(FileIO::GetHandle(),"     mem.Dump();                        \n");
	fprintf(FileIO::GetHandle(),"     mem.Free( b );                     \n");
	fprintf(FileIO::GetHandle(),"     mem.Dump();                        \n");
	fprintf(FileIO::GetHandle(),"     mem.Free( a );                     \n");
	fprintf(FileIO::GetHandle(),"     mem.Dump();                        \n");
	fprintf(FileIO::GetHandle(),"     mem.Free( c );                     \n");
	fprintf(FileIO::GetHandle(),"     mem.Dump();                        \n");

	// Part A:
	Mem mem1;
	mem1.Initialize();

   	void *a1 = mem1.Malloc( 0x4280 );
   	void *b1 = mem1.Malloc( 0x4290 );
	void *c1 = mem1.Malloc( 0x4280 );

	mem1.Dump();
	
	// Part B:
	mem1.Free( b1 );
	mem1.Dump();

	// Part C:
	mem1.Free( a1 );
	mem1.Dump();

	// Part D:
	mem1.Free( c1 );
	mem1.Dump();
	
	FileIO::Close();

	// ----  GENERAL CHECK ------------------------------------------------------

	// ---- Part A: -------------------------------------------------------------
	
	Mem mem;
	mem.Initialize();

   	void *a = mem.Malloc( 0x4280 );
   	void *b = mem.Malloc( 0x4290 );
	void *c = mem.Malloc( 0x4280 );

	// ---- Verify A: -----------------------------------------------------------

		Heap *h = mem.GetHeap();

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x40 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL( h->pFreeHead, 0x0);
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x8570 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 3 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0xc790);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 3);
		CHECK_EQUAL( h->mStats.currUsedMem, 0xc790);
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 0 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0x0 );

		CHECK_EQUAL( h->mStats.sizeHeap, 0xc800 );

		CHECK_EQUAL( PTR_FIX( h->mStats.heapTopAddr), 0x40 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), 0xc800 );

	// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL( PTR_FIX( (Type::U32)h->mStats.heapTopAddr-sizeof(Heap) ), 0 );
		// HeapHdr End
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


	// ----  Block walk ------------------------------------------------------
		
		Type::U32 hdrStart;
		Type::U32 hdrEnd;
		Type::U32 blkStart;
		Type::U32 blkEnd; 
		Type::U32 secret;

	// ---- USED HDR 1 -------------------------------------------

		// Check type
		UsedHdr *used = (UsedHdr *) h->mStats.heapTopAddr;
		// Should be USED
		CHECK_EQUAL( (int)used->mBlockType, (int)BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x40  );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x50 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x42d0);
		// Next
		CHECK_EQUAL( used->pUsedNext, 0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(UsedHdr) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x42d0);
		CHECK_EQUAL( used->mBlockSize, 0x4280 );
	
	// ---- UsedHdr HDR 2 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( (int)used->mBlockType, (int)BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x42d0 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x42e0 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x8570 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x40 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(UsedHdr) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x8570);
		CHECK_EQUAL( used->mBlockSize, 0x4290 );

	// ---- UsedHdr HDR 3 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( (int)used->mBlockType, (int)BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x8570 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x8580 );
		// Prev
		CHECK_EQUAL( used->pUsedPrev, 0);
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x42d0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(UsedHdr) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL( used->mBlockSize, 0x4280 );

		CHECK_EQUAL((PTR_FIX(h->pNextFit)), PTR_FIX(0));

	// ---- Part B: -------------------------------------------------------------
	
	mem.Free( b );
	
	// ---- Verify B: -----------------------------------------------------------

		h = mem.GetHeap();

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x40 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x42d0 );
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x8570 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 3 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0xc790);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 2);
		CHECK_EQUAL( h->mStats.currUsedMem, 0xc790 - 0x4290 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 1 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0x4290 );

		CHECK_EQUAL( h->mStats.sizeHeap, 0xc800 );

		CHECK_EQUAL( PTR_FIX( h->mStats.heapTopAddr), 0x40 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), 0xc800 );

	// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL( PTR_FIX( (Type::U32)h->mStats.heapTopAddr-sizeof(Heap) ), 0 );
		// HeapHdr End
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


	// ---- HDR 1 -------------------------------------------

		// Check type
		used = (UsedHdr *) h->mStats.heapTopAddr;
		// Should be USED
		CHECK_EQUAL( (int)used->mBlockType, (int)BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x40  );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x50 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x8570 );
		// Next
		CHECK_EQUAL( used->pUsedNext, 0x0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(UsedHdr) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x42d0 );
		CHECK_EQUAL( used->mBlockSize, 0x4280 );
	
	// ---- HDR 2 -------------------------------------------

		// Check type
		FreeHdr *free = (FreeHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( (int)free->mBlockType, (int)BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x42d0 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x42e0 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( free->pFreeNext, 0x0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(FreeHdr) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x8570 );
		CHECK_EQUAL( free->mBlockSize, 0x4290 );

		secret = *((Type::U32 *)(blkEnd - 4));
		CHECK_EQUAL(PTR_FIX(secret), PTR_FIX(hdrStart));

		CHECK_EQUAL(PTR_FIX(h->pNextFit), PTR_FIX(hdrStart));

	// ---- Hdr 3 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be free
		CHECK_EQUAL( (int)used->mBlockType, (int)BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x8570 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x8580 );
		// Prev
		CHECK_EQUAL( used->pUsedPrev, 0);
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x40 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(FreeHdr) );

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL( used->mBlockSize, 0x4280 );
	
	// ---- Part C: -------------------------------------------------------------
	
	mem.Free( a );
	
	// ---- Verify C: -----------------------------------------------------------

		h = mem.GetHeap();

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x40 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x40);
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x8570 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 3 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0xc790);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 1);
		CHECK_EQUAL( h->mStats.currUsedMem, 0x4280);
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 1 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0x8520 );

		CHECK_EQUAL( h->mStats.sizeHeap, 0xc800 );

		CHECK_EQUAL( PTR_FIX( h->mStats.heapTopAddr), 0x40 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), 0xc800 );

		// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL( PTR_FIX( (Type::U32)h->mStats.heapTopAddr-sizeof(Heap) ), 0 );
		// HeapHdr End
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );
 

	// ---- HDR 1 -------------------------------------------

		// Check type
		free = (FreeHdr *) h->mStats.heapTopAddr;
		CHECK_EQUAL( (int)free->mBlockType, (int)BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x40  );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x50 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( free->pFreeNext, 0x0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(FreeHdr) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x8570);
		CHECK_EQUAL( free->mBlockSize, 0x8520 );

		secret = *((Type::U32 *)(blkEnd - 4));
		CHECK_EQUAL(PTR_FIX(secret), PTR_FIX(hdrStart));

		CHECK_EQUAL(PTR_FIX(h->pNextFit), PTR_FIX(hdrStart));
	
	// ---- HDR 2 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		CHECK_EQUAL( (int)used->mBlockType, (int)BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x8570 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x8580 );
		// Prev
		CHECK_EQUAL( used->pUsedPrev, 0);
		// Next
		CHECK_EQUAL( used->pUsedNext, 0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(UsedHdr) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL( used->mBlockSize, 0x4280 );
	
	// ---- Part D: -------------------------------------------------------------
	
	mem.Free( c );
	
	// ---- Verify D: -----------------------------------------------------------

		h = mem.GetHeap();

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x40 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x40 );
		CHECK_EQUAL( h->pUsedHead, 0x0 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 3 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0xc790 );

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 0);
		CHECK_EQUAL( h->mStats.currUsedMem, 0x0 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 1 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xc7b0 );

		CHECK_EQUAL( h->mStats.sizeHeap, 0xc800 );

		CHECK_EQUAL( PTR_FIX( h->mStats.heapTopAddr), 0x40 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), 0xc800 );

	// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL( PTR_FIX( (Type::U32)h->mStats.heapTopAddr-sizeof(Heap) ), 0 );
		// HeapHdr End
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


	// ---- FREE HDR 1 -------------------------------------------

		// Check type
		free = (FreeHdr *) h->mStats.heapTopAddr;
		// Should be free
		CHECK_EQUAL( (int)free->mBlockType, (int)BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x40  );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x50 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( free->pFreeNext, 0x0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(FreeHdr) );

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x40 + 0x10 + 0xc7b0);
		CHECK_EQUAL( free->mBlockSize, 0xc7b0 );
	
		secret = *((Type::U32 *)(blkEnd - 4));
		CHECK_EQUAL(PTR_FIX(secret), PTR_FIX(hdrStart));

		CHECK_EQUAL(PTR_FIX(h->pNextFit), PTR_FIX(hdrStart));

}

// ---  End of File ---------------
