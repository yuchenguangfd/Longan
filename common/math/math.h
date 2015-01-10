/*
 * math.h
 * Created on: Jul 20, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_MATH_MATH_H
#define COMMON_MATH_MATH_H

#include <iterator>
#include <cmath>

namespace longan {

namespace Math {

const double PI = 3.14159265358979323846;
const double E = 2.7182818284590452354;

template <class T>
inline T Abs(T x) {
    return (x < T()) ? -x : x;
}

template <class T>
inline const T& Max(const T& a, const T& b) {
    return (a < b)? b : a;
}

template <class T>
inline const T& Min(const T& a, const T& b) {
    return (a < b)? a : b;
}

template <class T>
const T& Max(const T& a, const T& b, const T& c) {
    if (b < a && c < a) {
        return a;
    } else if (c < b) {
        return b;
    } else {
        return c;
    }
}

template <class T>
inline const T& Min(const T& a, const T& b, const T& c) {
    if (a < b && a < c) {
        return a;
    } else if ( b < c) {
        return b;
    } else {
        return c;
    }
}

template <class Iterator>
const typename std::iterator_traits<Iterator>::value_type&
    MaxInRange(Iterator begin, Iterator end, Iterator& maxPos) {
    maxPos = begin;
    for (++begin; begin != end; ++begin) {
        if (*maxPos < *begin) {
            maxPos = begin;
        }
    }
    return *maxPos;
}

template <class Iterator>
const typename std::iterator_traits<Iterator>::value_type&
    MaxInRange(Iterator begin, Iterator end) {
    Iterator maxPos;
    return MaxInRange(begin, end, maxPos);
}

inline double RelativeError(double expect, double actual) {
    return Abs(actual - expect) / expect;
}

template <class T>
inline T Sqr(const T& x) {
    return x * x;
}

inline double Sqr(double x) {
    return x * x;
}

inline int Sqr(int x) {
    return x * x;
}

inline double Sqrt(double x) {
    return ::sqrt(x);
}

inline double Sqrt(float x) {
    return ::sqrtf(x);
}

inline int Round(double x) {
    return static_cast<int>(::round(x));
}

inline int Ceil(double x) {
    return static_cast<int>(::ceil(x));
}

inline int Floor(double x) {
    return static_cast<int>(::floor(x));
}

int Factorial(int n);

} //~ namespace Math

} //~ namespace longan

#endif // COMMON_MATH_MATH_H
