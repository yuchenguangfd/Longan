/*
 * math.h
 * Created on: Jul 20, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_MATH_MATH_H
#define COMMON_MATH_MATH_H

#include <cmath>

namespace longan {

namespace Math {

template <class T>
inline T Abs(T x) {
    return (x >= 0) ? x : -x;
}

template <class T>
inline const T& Max(const T &a, const T &b) {
    return (a < b)? b : a;
}

template <class T>
inline const T& Min(const T &a, const T &b) {
    return (a < b)? a : b;
}

inline double RelativeError(double expect, double actual) {
    return Abs(actual - expect) / expect;
}

template <class T>
inline T Sqr(T x) {
    return x * x;
}

inline double Sqrt(double x) {
    return ::sqrt(x);
}

int Factorial(int n);

} //~ namespace Math

} //~ namespace longan

#endif // COMMON_MATH_MATH_H
