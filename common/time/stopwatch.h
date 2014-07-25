/*
 * stopwatch.h
 * Created on: Jul 24, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_TIME_STOPWATCH_H
#define COMMON_TIME_STOPWATCH_H

#include <chrono>
#include <string>

namespace longan {

class Stopwatch {
public:
	Stopwatch();
    void Reset();
    void Start();
    double ElapsedMilliseconds();
    void Tic() { Start(); }
    double Toc() { return ElapsedMilliseconds() / 1000; } // in seconds
private:
    std::chrono::high_resolution_clock::time_point begin;
    std::chrono::milliseconds duration;
};

} // namespace longan

#endif // COMMON_TIME_STOPWATCH_H
