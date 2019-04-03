//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 

#include "Framework.h"

#include "Mem.h"

#include "Trace.h"

#define PTR_FIX(x)  (unsigned int)(x == 0) ? 0: ((unsigned int)x - (unsigned int)h)  
#define UNUSED_VAR(v) ((void *)v)

TEST( Secret_pointers , MemoryShakeOut )
{
	FileIO::Open("Student_", "Test_16");

	fprintf(FileIO::GetHandle(),"Test16: \n\n");

	// Part A:
	fprintf(FileIO::GetHandle(),"      Mem mem;\n");
	fprintf(FileIO::GetHandle(),"      mem.Initialize();\n");

	fprintf(FileIO::GetHandle(),"      void *a1 = mem.Malloc(0x100);\n");
	fprintf(FileIO::GetHandle(),"      void *a2 = mem.Malloc(0x20);\n");
	fprintf(FileIO::GetHandle(),"      void *a3 = mem.Malloc(0x20);\n");
	fprintf(FileIO::GetHandle(),"      void *a4 = mem.Malloc(0x20);\n");
	fprintf(FileIO::GetHandle(),"      void *a5 = mem.Malloc(0x20);\n");
	fprintf(FileIO::GetHandle(),"      void *b1 = mem.Malloc(0x200);\n");
	fprintf(FileIO::GetHandle(),"      void *c1 = mem.Malloc(0x300);\n");
	fprintf(FileIO::GetHandle(),"      void *c2 = mem.Malloc(0x30);\n");
	fprintf(FileIO::GetHandle(),"      void *c3 = mem.Malloc(0x30);\n");

	fprintf(FileIO::GetHandle(),"      void *c4 = mem.Malloc(0x30);\n");
	fprintf(FileIO::GetHandle(),"      void *c5 = mem.Malloc(0x30);\n");
	fprintf(FileIO::GetHandle(),"      void *c6 = mem.Malloc(0x30);\n");
	fprintf(FileIO::GetHandle(),"      void *c7 = mem.Malloc(0x30);\n");
	fprintf(FileIO::GetHandle(),"      void *d1 = mem.Malloc(0x200);\n");

	fprintf(FileIO::GetHandle(),"      void *d2 = mem.Malloc(0x40);\n");
	fprintf(FileIO::GetHandle(),"      void *d3 = mem.Malloc(0x60);\n");
	fprintf(FileIO::GetHandle(),"      void *e1 = mem.Malloc(0x100);\n");
	fprintf(FileIO::GetHandle(),"      void *f1 = mem.Malloc(0x500);\n");
	fprintf(FileIO::GetHandle(),"      void *g1 = mem.Malloc(0x100);\n\n");

	fprintf(FileIO::GetHandle(),"      mem.Dump();\n\n");

	fprintf(FileIO::GetHandle(),"      mem.Free(d1);\n");
	fprintf(FileIO::GetHandle(),"      mem.Free(b1);\n");
	fprintf(FileIO::GetHandle(),"      mem.Free(f1);\n");
	fprintf(FileIO::GetHandle(),"      mem.Free(a1);\n");
	fprintf(FileIO::GetHandle(),"      mem.Free(a4);\n");
	fprintf(FileIO::GetHandle(),"      mem.Free(c7);\n");
	fprintf(FileIO::GetHandle(),"      mem.Free(c5);\n");
	fprintf(FileIO::GetHandle(),"      mem.Free(c2);\n\n");

	fprintf(FileIO::GetHandle(),"      mem.Dump();\n\n");

	fprintf(FileIO::GetHandle(),"      mem1.Free(d2);\n");
	fprintf(FileIO::GetHandle(),"      mem1.Free(c4);\n");
	fprintf(FileIO::GetHandle(),"      mem1.Free(g1);\n\n");

	fprintf(FileIO::GetHandle(),"      mem1.Dump();\n\n");


	//---------------------------------------------------------
	// Part A:
	//---------------------------------------------------------
	Mem mem1;
	mem1.Initialize();

   	void *a1 = mem1.Malloc( 0x100 );
	void *a2 = mem1.Malloc(0x20);
	void *a3 = mem1.Malloc(0x20);
	void *a4 = mem1.Malloc(0x20);
	void *a5 = mem1.Malloc(0x20);
   	void *b1 = mem1.Malloc( 0x200 );
	void *c1 = mem1.Malloc( 0x300 );
	void *c2 = mem1.Malloc(0x30);
	void *c3 = mem1.Malloc(0x30);

	void *c4 = mem1.Malloc(0x30);
	void *c5 = mem1.Malloc(0x30);
	void *c6 = mem1.Malloc(0x30);
	void *c7 = mem1.Malloc(0x30);
	void *d1 = mem1.Malloc( 0x200 );

	void *d2 = mem1.Malloc(0x40);
	void *d3 = mem1.Malloc(0x60);
	void *e1 = mem1.Malloc( 0x100);
	void *f1 = mem1.Malloc(0x500);
	void *g1 = mem1.Malloc(0x100);

	UNUSED_VAR(c1);
	UNUSED_VAR(e1);
	UNUSED_VAR(g1);
	UNUSED_VAR(a1);
	UNUSED_VAR(a2);
	UNUSED_VAR(a3);
	UNUSED_VAR(a5);
	UNUSED_VAR(c3);
	UNUSED_VAR(c6);
	UNUSED_VAR(c4);
	UNUSED_VAR(d2);	
	UNUSED_VAR(d3);

	// ---- Verify A: -----------------------------------------------------------

	Heap *h = mem1.GetHeap();

	CHECK_EQUAL(PTR_FIX(a1), 0x50); 
	CHECK_EQUAL(PTR_FIX(a2), 0x160); 
	CHECK_EQUAL(PTR_FIX(a3), 0x190); 
	CHECK_EQUAL(PTR_FIX(a4), 0x1C0); 
	CHECK_EQUAL(PTR_FIX(a5), 0x1F0); 
	CHECK_EQUAL(PTR_FIX(b1), 0x220); 
	CHECK_EQUAL(PTR_FIX(c1), 0x430); 
	CHECK_EQUAL(PTR_FIX(c2), 0x740); 
	CHECK_EQUAL(PTR_FIX(c3), 0x780); 

	CHECK_EQUAL(PTR_FIX(c4), 0x7c0); 
	CHECK_EQUAL(PTR_FIX(c5), 0x800);
	CHECK_EQUAL(PTR_FIX(c6), 0x840);
	CHECK_EQUAL(PTR_FIX(c7), 0x880); 
	CHECK_EQUAL(PTR_FIX(d1), 0x8c0);

	CHECK_EQUAL(PTR_FIX(d2), 0xad0); 
	CHECK_EQUAL(PTR_FIX(d3), 0xb20); 
	CHECK_EQUAL(PTR_FIX(e1), 0xb90); 
	CHECK_EQUAL(PTR_FIX(f1), 0xca0);
	CHECK_EQUAL(PTR_FIX(g1), 0x11b0); 

	mem1.Dump();

	//---------------------------------------------------------
	// Part B:
	//---------------------------------------------------------

	mem1.Free( d1 );
	mem1.Free( b1 );
	mem1.Free(f1);
	mem1.Free(a1);
	mem1.Free(a4);
	mem1.Free(c7);
	mem1.Free(c5);
	mem1.Free(c2);

	UsedHdr *used;
	Type::U32  *secret;
	FreeHdr *free;

	// Check for Secret Pointers, given pointer
	used = (UsedHdr *)a2 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x150);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (Type::U32 *)((Type::U32)used - 4);
	free =  (FreeHdr *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x40);

	// Check for Secret Pointers, given pointer
	used = (UsedHdr *)a5 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x1E0);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (Type::U32 *)((Type::U32)used - 4);
	free = (FreeHdr *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x1B0);

	// Check for Secret Pointers, given pointer
	used = (UsedHdr *)c1 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x420);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (Type::U32 *)((Type::U32)used - 4);
	free = (FreeHdr *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x210);

	// Check for Secret Pointers, given pointer
	used = (UsedHdr *)c3 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x770);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (Type::U32 *)((Type::U32)used - 4);
	free = (FreeHdr *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x730);

	// Check for Secret Pointers, given pointer
	used = (UsedHdr *)c6 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x830);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (Type::U32 *)((Type::U32)used - 4);
	free = (FreeHdr *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x7f0);

	// Check for Secret Pointers, given pointer
	used = (UsedHdr *)d2 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0xac0);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (Type::U32 *)((Type::U32)used - 4);
	free = (FreeHdr *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x870);

	// Check for Secret Pointers, given pointer
	used = (UsedHdr *)g1 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x11a0);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (Type::U32 *)((Type::U32)used - 4);
	free = (FreeHdr *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0xc90);

	mem1.Dump();

	//---------------------------------------------------------
	// Part C:
	//---------------------------------------------------------
	//
	//  READ the next 3 Frees... here is the real test
	//  If it doesn't pass inspection - you lose points 
	//
	//---------------------------------------------------------

	// This should not use While loop on the FreeHdr list
	// I will walk into you function and verify - Keenan
	mem1.Free(d2);

	// This should not use While loop on the FreeHdr list
	// I will walk into you function and verify - Keenan
	mem1.Free(c4);

	// This should not use While loop on the FreeHdr list
	// I will walk into you function and verify - Keenan
	mem1.Free(g1);

	// Check for Secret Pointers, given pointer
	used = (UsedHdr *)a2 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x150);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (Type::U32 *)((Type::U32)used - 4);
	free = (FreeHdr *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x40);

	// Check for Secret Pointers, given pointer
	used = (UsedHdr *)a5 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x1e0);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (Type::U32 *)((Type::U32)used - 4);
	free = (FreeHdr *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x1b0);

	// Check for Secret Pointers, given pointer
	used = (UsedHdr *)c1 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x420);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (Type::U32 *)((Type::U32)used - 4);
	free = (FreeHdr *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x210);

	// Check for Secret Pointers, given pointer
	used = (UsedHdr *)c3 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x770);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (Type::U32 *)((Type::U32)used - 4);
	free = (FreeHdr *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x730);

	// Check for Secret Pointers, given pointer
	used = (UsedHdr *)c6 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0x830);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (Type::U32 *)((Type::U32)used - 4);
	free = (FreeHdr *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x7b0);

	// Check for Secret Pointers, given pointer
	used = (UsedHdr *)d3 - 1;
	CHECK_EQUAL((PTR_FIX(used)), 0xb10);
	CHECK_EQUAL(used->mAboveBlockFree, true);
	secret = (Type::U32 *)((Type::U32)used - 4);
	free = (FreeHdr *)*secret;
	CHECK_EQUAL((PTR_FIX(free)), 0x870);

	mem1.Dump();
	FileIO::Close();

}

// ---  End of File ---------------
