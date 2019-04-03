//----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
//----------------------------------------------------------------------------
// Timer v.2.1.0
//       v.2.5
//       v.2.6   - Baseline
//       v.2.6.1 - fixed warning TestRegistry
//       v.2.7   - Baseline
//       v.2.8   - warning 5039 windows.h
//       v.2.9   - fence
//---------------------------------------------------------------------------- 

#include <assert.h>


// locals
#include "Timer.h"

Timer::Timer()
{
	this->privInitTimer();
	this->Reset();
}

void Timer::privInitTimer() 
{
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	this->SecondsPerCycle = 1.0f / Frequency.QuadPart;
}

LARGE_INTEGER Timer::privGetTimer()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	return time;
}

void Timer::Tic()
{
	// Forces a Fence... 
	printf("tic()\n");

	this->ticTime = this->privGetTimer();
}

void Timer::Toc()
{
	this->tocTime = this->privGetTimer();
	assert(this->tocTime.QuadPart >= this->ticTime.QuadPart);
	this->deltaTime.QuadPart = this->tocTime.QuadPart - this->ticTime.QuadPart;

	// Forces a Fence... 
	printf("toc()\n");
}

void Timer::Reset()
{
	this->ticTime.QuadPart = 0;
	this->tocTime.QuadPart = 0;
	this->deltaTime.QuadPart = 0;
}

float Timer::TimeInSeconds()
{
	float floatTime;
	floatTime = static_cast<float>(this->deltaTime.QuadPart);
	floatTime *= this->SecondsPerCycle;
	return floatTime;
}

// ---  End of File ---------------
