//----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
//----------------------------------------------------------------------------
// UnitTest v.2.1.0
//          v.2.1.1  - fixed C5038 warning
//          v.2.5
//          v.2.5.1 - Teardown - rework
//          v.2.6   - Baseline
//          v.2.6.1 - fixed warning TestRegistry
//          v.2.7   - Baseline
//          v.2.8   - warning 5039 windows.h
//          v.2.9   - fence
//---------------------------------------------------------------------------- 

#include "UnitTest.h"

Test::Test(const char * pTestName)
	:UnitSLink()
{
	// initialized data
	this->pName = pTestName;
	this->testFunc = this;

	// register it
	TestRegistry::AddTest(this);

}

// ---  End of File ---------------
