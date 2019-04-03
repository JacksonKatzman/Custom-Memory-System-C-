//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"

#include "Mem.h"

#define PTR_FIX(x)  (unsigned int)(x == 0) ? 0: ((unsigned int)x - (unsigned int)h)  

TEST( combo_2 , MemoryShakeOut )
{
	FileIO::Open("Student_", "Test_15");

	fprintf(FileIO::GetHandle(),"Test15: \n\n");

	fprintf(FileIO::GetHandle(),"     Mem mem;                     \n");
	fprintf(FileIO::GetHandle(),"     mem.Initialize();            \n");

   	fprintf(FileIO::GetHandle(),"     void *a = mem.Malloc( 0x100 );     \n");
   	fprintf(FileIO::GetHandle(),"     void *b = mem.Malloc( 0x200 );     \n");
	fprintf(FileIO::GetHandle(),"     void *c = mem.Malloc( 0x10 );     \n");
	fprintf(FileIO::GetHandle(),"     void *d = mem.Malloc( 0x200 );     \n");

       
	fprintf(FileIO::GetHandle(),"     mem.Dump();                        \n");
	fprintf(FileIO::GetHandle(),"     mem.Free( d );                     \n");
	fprintf(FileIO::GetHandle(),"     mem.Free( b );                     \n");

	fprintf(FileIO::GetHandle(),"     mem.Dump();                        \n");

	fprintf(FileIO::GetHandle(),"     void *r = mem.Malloc( 0x200 );     \n");
	fprintf(FileIO::GetHandle(),"     mem.Dump();                        \n");

	// Part A:
	Mem mem1;
	mem1.Initialize();

   	void *a1 = mem1.Malloc( 0x100 );
   	void *b1 = mem1.Malloc( 0x200 );
	void *c1 = mem1.Malloc( 0x10 );
	void *d1 = mem1.Malloc( 0x200 );

	mem1.Dump();

	// Part B:
	mem1.Free( d1 );
	mem1.Free( b1 );

	mem1.Dump();

	// Part C:
	void *r1 = mem1.Malloc( 0x200 );
	mem1.Dump();

	// to shut up warnings
	r1 = 0;
	a1 = 0;
	c1 = 0;

	FileIO::Close();

	// ----  GENERAL CHECK ------------------------------------------------------

	// ---- Part A: -------------------------------------------------------------
	
	Mem mem;
	mem.Initialize();

   	void *a = mem.Malloc( 0x100 );
   	void *b = mem.Malloc( 0x200 );
	void *c = mem.Malloc( 0x10 );
	void *d = mem.Malloc( 0x200 );

	// ---- Verify A: -----------------------------------------------------------

		Heap *h = mem.GetHeap();

		CHECK_EQUAL( PTR_FIX(a), 0x50 );
		CHECK_EQUAL( PTR_FIX(b), 0x160 ); 
		CHECK_EQUAL( PTR_FIX(c), 0x370 );
		CHECK_EQUAL( PTR_FIX(d), 0x390 );

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x40 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x590 );
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x380 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 4 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0x510);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 4);
		CHECK_EQUAL( h->mStats.currUsedMem, 0x510 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 1 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xc260 );

		CHECK_EQUAL( h->mStats.sizeHeap, 0xc800 );

		CHECK_EQUAL( PTR_FIX( h->mStats.heapTopAddr), 0x40 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), 0xc800 );

	// ---- Heap Hdr -------------------------------------------------------

		// HeapHdr Start
		CHECK_EQUAL( PTR_FIX( (Type::U32)h->mStats.heapTopAddr-sizeof(Heap) ), 0 );
		// HeapHdr End
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), sizeof(Heap) );


		Type::U32 hdrStart;
		Type::U32 hdrEnd;
		Type::U32 blkStart;
		Type::U32 blkEnd; 
		Type::U32 secret;
	
	// ---- HDR 1 -------------------------------------------

		// Check type
		UsedHdr *used = (UsedHdr *) h->mStats.heapTopAddr;
		// Should be USED
		CHECK_EQUAL( (int)used->mBlockType, (int)BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x40 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x50 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x150 );
		// Next
		CHECK_EQUAL( used->pUsedNext, 0x0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(UsedHdr) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x150 );
		CHECK_EQUAL( used->mBlockSize, 0x100 );

	// ---- HDR 2 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( (int)used->mBlockType, (int)BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x150 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x160 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x360 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x40);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x360 );
		CHECK_EQUAL( used->mBlockSize, 0x200 );
		
	// ---- HDR 3 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( (int)used->mBlockType, (int)BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x360 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x370 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x380 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x150);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x380 );
		CHECK_EQUAL( used->mBlockSize, 0x10 );

	// ---- HDR 4 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( (int)used->mBlockType, (int)BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x380 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x390 );
		// Prev
		CHECK_EQUAL( used->pUsedPrev, 0x0 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x360);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x590 );
		CHECK_EQUAL( used->mBlockSize, 0x200 );
		
	// ---- Hdr 5 -------------------------------------------

		// Check type
		FreeHdr *free = (FreeHdr *) blkEnd;
		// Should be free
		CHECK_EQUAL( (int)free->mBlockType, (int)BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x590 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x5a0 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( free->pFreeNext, 0x0);
	
		// ---- Block ----------------------------------------------------

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL( free->mBlockSize, 0xc260 );

		secret = *((Type::U32 *)(blkEnd - 4));
		CHECK_EQUAL( PTR_FIX(secret), PTR_FIX(hdrStart));

		CHECK_EQUAL(PTR_FIX(h->pNextFit), PTR_FIX(hdrStart));
	
	// ---- Part B: -------------------------------------------------------------
	
	mem.Free( d );
	mem.Free( b );
	
	// ---- Verify B: -----------------------------------------------------------

		h = mem.GetHeap();

		CHECK_EQUAL( PTR_FIX(a), 0x50 );
		CHECK_EQUAL( PTR_FIX(b), 0x160 ); 
		CHECK_EQUAL( PTR_FIX(c), 0x370 );
		CHECK_EQUAL( PTR_FIX(d), 0x390 );

		// Sanit check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x40 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x150 );
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x360 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 4 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0x510);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 2);
		CHECK_EQUAL( h->mStats.currUsedMem, 0x110 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 2 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xc670 );

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
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x40 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x50 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x360 );
		// Next
		CHECK_EQUAL( used->pUsedNext, 0x0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(UsedHdr) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x150 );
		CHECK_EQUAL( used->mBlockSize, 0x100 );

	// ---- HDR 2 -------------------------------------------

		// Check type
		free = (FreeHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( (int)free->mBlockType, (int)BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x150 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x160 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0x380);

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x360 );
		CHECK_EQUAL( free->mBlockSize, 0x200 );
		
	// ---- HDR 3 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( (int)used->mBlockType, (int)BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x360 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x370 );
		// Prev
		CHECK_EQUAL( used->pUsedPrev, 0x0 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x40);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x380 );
		CHECK_EQUAL( used->mBlockSize, 0x10 );

	// ---- HDR 4 -------------------------------------------

		// Check type
		free = (FreeHdr *) blkEnd;
		// Should be free
		CHECK_EQUAL( (int)free->mBlockType, (int)BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x380 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x390 );
		// Prev
		CHECK_EQUAL( PTR_FIX(free->pFreePrev), 0x150 );
		// Next
		CHECK_EQUAL( free->pFreeNext, 0x0);

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0xc800 );
		CHECK_EQUAL( free->mBlockSize, 0xc470 );
	
		secret = *((Type::U32 *)(blkEnd - 4));
		CHECK_EQUAL(PTR_FIX(secret), PTR_FIX(hdrStart));

		CHECK_EQUAL(PTR_FIX(h->pNextFit), PTR_FIX(hdrStart));

	// ---- Part C: -------------------------------------------------------------
	
	void *r = mem.Malloc( 0x200 );
	
	// ---- Verify C: -----------------------------------------------------------


		h = mem.GetHeap();

		CHECK_EQUAL( PTR_FIX(r), 0x390 );

		// Sanity check, make sure everything is heap relative for testing
		CHECK_EQUAL( PTR_FIX(h), 0 );

		// Heap Start / Heap Bottom
		CHECK_EQUAL( PTR_FIX(h->mStats.heapTopAddr), 0x40 );
		CHECK_EQUAL( PTR_FIX(h->mStats.heapBottomAddr), Mem::HEAP_SIZE);

		// UsedHdr / FreeHdr
		CHECK_EQUAL( PTR_FIX(h->pFreeHead), 0x150 );
		CHECK_EQUAL( PTR_FIX(h->pUsedHead), 0x380 );

	// ---- Heap Stats ------------------------------------------------------

		CHECK_EQUAL( h->mStats.peakNumUsed, 4 );
		CHECK_EQUAL( h->mStats.peakUsedMemory, 0x510);

		CHECK_EQUAL( h->mStats.currNumUsedBlocks, 3);
		CHECK_EQUAL( h->mStats.currUsedMem, 0x310 );
		
		CHECK_EQUAL( h->mStats.currNumFreeBlocks, 2 );
		CHECK_EQUAL( h->mStats.currFreeMem, 0xc460 );

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
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x40 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x50 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x360 );
		// Next
		CHECK_EQUAL( used->pUsedNext, 0x0 );
		// Hdr Size
		CHECK_EQUAL( hdrEnd - hdrStart, sizeof(UsedHdr) );
	
		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x150 );
		CHECK_EQUAL( used->mBlockSize, 0x100 );

	// ---- HDR 2 -------------------------------------------

		// Check type
		free = (FreeHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( (int)free->mBlockType, (int)BlockType::FREE );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)free;
		hdrEnd   = (Type::U32)free + sizeof(FreeHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x150 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x160 );
		// Prev
		CHECK_EQUAL( free->pFreePrev, 0x0 );
		// Next
		CHECK_EQUAL( PTR_FIX(free->pFreeNext), 0x590);

		blkStart = hdrEnd;
		blkEnd   = blkStart + free->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x360 );
		CHECK_EQUAL( free->mBlockSize, 0x200 );
		
	// ---- HDR 3 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be USED
		CHECK_EQUAL( (int)used->mBlockType, (int)BlockType::USED );
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, true);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x360 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x370 );
		// Prev
		CHECK_EQUAL( PTR_FIX(used->pUsedPrev), 0x380 );
		// Next
		CHECK_EQUAL( PTR_FIX(used->pUsedNext), 0x40);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x380 );
		CHECK_EQUAL( used->mBlockSize, 0x10 );

	// ---- HDR 4 -------------------------------------------

		// Check type
		used = (UsedHdr *) blkEnd;
		// Should be free
		CHECK_EQUAL( (int)used->mBlockType, (int)BlockType::USED);
		// Above FreeHdr
		CHECK_EQUAL(used->mAboveBlockFree, false);
		
		hdrStart = (Type::U32)used;
		hdrEnd   = (Type::U32)used + sizeof(UsedHdr);
	
		// Hdr Start
		CHECK_EQUAL( PTR_FIX(hdrStart), 0x380 );
		// Hdr End
		CHECK_EQUAL( PTR_FIX(hdrEnd), 0x390 );
		// Prev
		CHECK_EQUAL( used->pUsedPrev, 0x0 );
		// Next
		CHECK_EQUAL(PTR_FIX(used->pUsedNext), 0x360);

		blkStart = hdrEnd;
		blkEnd   = blkStart + used->mBlockSize; 
		
		CHECK_EQUAL( PTR_FIX(blkStart), PTR_FIX(hdrEnd) );
		
		CHECK_EQUAL( PTR_FIX(blkEnd), 0x590 );
		CHECK_EQUAL( used->mBlockSize, 0x200 );
	
	// ---- HDR 5 -------------------------------------------

		// Check type
		free = (FreeHdr *)blkEnd;
		// Should be free
		CHECK_EQUAL((int)free->mBlockType, (int)BlockType::FREE);
		// Above FreeHdr
		CHECK_EQUAL(free->mAboveBlockFree, false);

		hdrStart = (Type::U32)free;
		hdrEnd = (Type::U32)free + sizeof(FreeHdr);

		// Hdr Start
		CHECK_EQUAL(PTR_FIX(hdrStart), 0x590);
		// Hdr End
		CHECK_EQUAL(PTR_FIX(hdrEnd), 0x5a0);
		// Prev
		CHECK_EQUAL(PTR_FIX(free->pFreePrev), 0x0150);
		// Next
		CHECK_EQUAL(free->pFreeNext, 0);

		blkStart = hdrEnd;
		blkEnd = blkStart + free->mBlockSize;

		CHECK_EQUAL(PTR_FIX(blkStart), PTR_FIX(hdrEnd));
		CHECK_EQUAL(PTR_FIX(blkEnd), 0xc800);
		CHECK_EQUAL(free->mBlockSize, 0xc260);

		secret = *((Type::U32 *)(blkEnd - 4));
		CHECK_EQUAL(PTR_FIX(secret), PTR_FIX(hdrStart));

		CHECK_EQUAL(PTR_FIX(h->pNextFit), PTR_FIX(hdrStart));
}

// ---  End of File ---------------
