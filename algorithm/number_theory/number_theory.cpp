/*
 * number_theory.cpp
 * Created on: Jul 23, 2014
 * Author: chenguangyu
 */

#include "number_theory.h"
#include "common/base/algorithm.h"
#include "common/math/math.h"

namespace longan {

namespace NumberTheory {

int Gcd(int a, int b) {
    if (a < b) {
        Swap(a, b);
    }
    while (b != 0) {
        int r = a % b;
        a = b;
        b = r;
    }
    return a;
}

bool IsPrime(int n) {
    if(n <= 1) return false;
    if(n == 2) return true;
    if((n & 1) == 0) return false;
    int limit = (int)Math::Sqrt(static_cast<double>(n));
    for(int t = 3; t <= limit; t += 2)
        if(n % t == 0) {
            return false;
        }
    return true;
}

}  // namespace NumberTheory

}  // namespace longan
