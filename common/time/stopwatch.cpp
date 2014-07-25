/*
 * stopwatch.h
 * Created on: Jul 24, 2014
 * Author: chenguangyu
 */

#include "stopwatch.h"

namespace longan {

Stopwatch::Stopwatch() {
    Reset();
}

void Stopwatch::Reset() {
    begin = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>
               (begin - begin);
}

void Stopwatch::Start() {
    begin = std::chrono::high_resolution_clock::now();
}

double Stopwatch::ElapsedMilliseconds() {
    duration = std::chrono::duration_cast<std::chrono::milliseconds>
                    (std::chrono::high_resolution_clock::now() - begin);
    return (double)duration.count();
}

} //~ namespace longan
