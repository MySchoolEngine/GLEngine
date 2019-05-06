#pragma once

#include <Core/CoreMacros.h>

#include <chrono>

/*
 * High Resolution Timer
 */

#define TIMER_NS_PER_SECOND 1000000000.0
#define TIMER_NS_PER_MILLISECOND 1000000.0
namespace Utils {
class API_EXPORT HighResolutionTimer
{
private:
	std::chrono::high_resolution_clock::time_point _start, _stop, _last;
	std::chrono::duration<double, std::nano> _duration;

public:
	HighResolutionTimer();
	void reset();
	double getElapsedTimeSeconds();
	double getElapsedTimeMilliseconds();
	double getElapsedTimeFromLastQuerySeconds();
	double getElapsedTimeFromLastQueryMilliseconds();
	double getTimerPrecisionSeconds();
};
}