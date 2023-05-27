// Using C++ because there is simple way to get an accurate Time Measuring tools in C

#include <stdio.h>
#include <chrono>

using namespace  std::chrono;

high_resolution_clock::time_point sCurrentFrameTime;
high_resolution_clock::time_point sPreviousFrameTime;

static bool				sTimerInitialized	= false;
static bool				sFpsLockedFrame		= false;
static long long		sDeltaTime			= 0;
static long long		sFrameTime			= 0;
static long long		sElapsedTime		= 0;
static unsigned long	sFrameCount			= 0;
static int				sLockedFPS			= 60;
static unsigned long	sLockedFPSFrameTime = 1000000 / sLockedFPS; // The timer is in MicroSeconds 

extern "C" void Clock_Reset()
{
	sPreviousFrameTime = high_resolution_clock::now();
	sCurrentFrameTime = high_resolution_clock::now();
	sDeltaTime = 0;
	sFrameTime = 0;
	sFrameCount = 0;
}

extern "C" void Clock_GameLoopStart()
{
	if (!sTimerInitialized)
		Clock_Reset();

	// calculating the time difference between 2 consecutive GameLoops
	sPreviousFrameTime = sCurrentFrameTime;
	sCurrentFrameTime = high_resolution_clock::now();
	const auto frameDuration = duration_cast<microseconds>(sCurrentFrameTime - sPreviousFrameTime);

	// The time difference is our DeltaTime dT
	sDeltaTime = frameDuration.count();

	// Total elapsed time since the first 
	sElapsedTime += sDeltaTime;

	// Calculating FrameTime for the FPS lock system
	sFrameTime += sDeltaTime;

	// Not using modulo, to be able to have a catchup mechanism in case of freeze
	sFpsLockedFrame = false;
	if (sFrameTime > sLockedFPSFrameTime)
	{
		sFrameTime -= sLockedFPSFrameTime;
		++sFrameCount;
		sFpsLockedFrame = true;
	}

	sTimerInitialized = true;
}

extern "C" void Clock_GetInfo(char* stringBuffer, int stringBufferSize)
{
	sprintf_s(stringBuffer, stringBufferSize, "dT:[%08lld], ElapsedTime:[%08lld], FrameCount:[%08lu]", sDeltaTime, sElapsedTime, sFrameCount);
}

extern "C" long long Clock_GetElapsedTimeUs()
{
	return sElapsedTime;
}

extern "C" unsigned long Clock_GetElapsedTimeMs()
{
	return static_cast<unsigned long>(sElapsedTime / 1000);
}

extern "C" float Clock_GetDeltaTime()
{
	// The narrow conversion is fine, dT should never spill out of the float limit
	return (sDeltaTime / 1000.f);
}

extern "C" bool Clock_IsAFrame()
{
	return sFpsLockedFrame;
}

extern "C" unsigned long Clock_GetFrameCount()
{
	return sFrameCount;
}

extern "C" void Clock_SetLockedFPS(int desiredFPS)
{
	sLockedFPS = desiredFPS;
}
