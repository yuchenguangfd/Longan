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
inline const T& Max(const T &a, const T &b) {
    return (a < b)? b : a;
}

template <class T>
inline const T& Min(const T &a, const T &b) {
    return (a < b)? a : b;
}

int Factorial(int n);

} //~ namespace math

#endif // COMMON_MATH_MATH_H
