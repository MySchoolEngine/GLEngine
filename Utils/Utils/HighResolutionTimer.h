#pragma once

#include <Utils/UtilsApi.h>

#include <chrono>

/*
 * High Resolution Timer
 */
namespace Utils {
class UTILS_API_EXPORT HighResolutionTimer {
private:
	std::chrono::high_resolution_clock::time_point _start, _stop, _last;
	std::chrono::duration<double, std::nano>	   _duration;

public:
	HighResolutionTimer();
	void   reset();
	double getElapsedTimeSeconds();
	double getElapsedTimeMilliseconds();
	double getElapsedTimeFromLastQuerySeconds();
	double getElapsedTimeFromLastQueryMilliseconds();
	double getTimerPrecisionSeconds();
};
} // namespace Utils