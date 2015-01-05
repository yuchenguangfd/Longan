/*
 * prime.cpp
 * Created on: Jan 4, 2015
 * Author: chenguangyu
 */

#include "prime.h"
#include <cassert>

namespace longan {

void PrimeFactorization(int N, PrimeFactorizationResult *result) {
    assert(N > 0);
    result->Clear();
    int factor = 2;
    int power = 0;
    while (N % factor == 0) {
        N /= factor;
        ++power;
    }
    if (power > 0) result->Add(factor, power);
    for (factor = 3; factor <= N / factor; factor += 2) {
        power = 0;
        while (N % factor == 0) {
            N /= factor;
            ++power;
        }
        if (power > 0) result->Add(factor, power);
    }
    if (N > 1) {
        result->Add(N, 1);
    }
}

} //~ namespace longan
