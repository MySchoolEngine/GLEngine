#include <Utils/HighResolutionTimer.h>

namespace Utils {

#define TIMER_NS_PER_SECOND 1000000000.0
#define TIMER_NS_PER_MILLISECOND 1000000.0

HighResolutionTimer::HighResolutionTimer() { reset(); }

// Reset timer
void HighResolutionTimer::reset() {
  _start = _last = std::chrono::high_resolution_clock::now();
}

double HighResolutionTimer::getElapsedTimeSeconds() {
  _stop = std::chrono::high_resolution_clock::now();
  _duration = _stop - _start;
  _last = _stop;

  return _duration.count() / TIMER_NS_PER_SECOND;
}

double HighResolutionTimer::getElapsedTimeMilliseconds() {
  _stop = std::chrono::high_resolution_clock::now();
  _duration = _stop - _start;
  _last = _stop;

  return _duration.count() / TIMER_NS_PER_MILLISECOND;
}

double HighResolutionTimer::getElapsedTimeFromLastQuerySeconds() {
  _stop = std::chrono::high_resolution_clock::now();
  _duration = _stop - _last;
  _last = _stop;

  return _duration.count() / TIMER_NS_PER_SECOND;
}

double HighResolutionTimer::getElapsedTimeFromLastQueryMilliseconds() {
  _stop = std::chrono::high_resolution_clock::now();
  _duration = _stop - _last;
  _last = _stop;

  return _duration.count() / TIMER_NS_PER_MILLISECOND;
}

double HighResolutionTimer::getTimerPrecisionSeconds() {
  return double(std::chrono::high_resolution_clock::period::num) /
         std::chrono::high_resolution_clock::period::den;
}
} // namespace Utils