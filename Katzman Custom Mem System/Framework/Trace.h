//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
//----------------------------------------------------------------------------- 
// Trace v.2.1.0
//       v.2.5
//       v.2.6   - Baseline
//       v.2.6.1 - fixed warning TestRegistry
//       v.2.7   - Baseline
//       v.2.8   - warning 5039 windows.h
//       v.2.9   - fence
//----------------------------------------------------------------------------- 

#ifndef DEBUG_OUTPUT_H
#define DEBUG_OUTPUT_H

#include <assert.h>
#include <stdio.h>

// Windows.h include
// many warnings - need to wrap for Wall warnings
	#pragma warning( push )
	#pragma warning( disable : 4820 )
	#pragma warning( disable : 4668 )

#if _MSC_VER > 1911
	#pragma warning( disable : 5039 )
#endif

	#include <Windows.h>
	#pragma warning( pop ) 

// NOTE: you need to set your project settings
//       Character Set -> Use Multi-Byte Character Set

#define TraceBuffSize 256

// Singleton helper class
class Trace
{
public:
	// displays a printf to the output window
	static void out(char* fmt, ...)
	{
		Trace *pTrace = Trace::privGetInstance();
		assert(pTrace);

		va_list args;
		va_start(args, fmt);

		vsprintf_s(pTrace->privBuff, TraceBuffSize, fmt, args);
		OutputDebugString(pTrace->privBuff);

		va_end(args);
	}

	// Big four
	Trace() = default;
	Trace(const Trace &) = default;
	Trace & operator = (const Trace &) = default;
	~Trace() = default;

private:
	static Trace *privGetInstance()
	{
		// This is where its actually stored (BSS section)
		static Trace helper;
		return &helper;
	}
	char privBuff[TraceBuffSize];
};

#endif

// ---  End of File ---------------
