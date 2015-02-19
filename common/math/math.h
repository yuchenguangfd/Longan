/*
 * math.h
 * Created on: Jul 20, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_MATH_MATH_H
#define COMMON_MATH_MATH_H

#include <iterator>
#include <cmath>
#include <cassert>

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

template <class T>
int MaxIndex(const T *array, int size) {
    assert(size > 0);
    T max = array[0];
    int idx = 0;
    for (int i = 1; i < size; ++i) {
        if (max < array[i]) {
            max = array[i];
            idx = i;
        }
    }
    return idx;
}

template <class T>
int MinIndex(const T *array, int size) {
    assert(size > 0);
    T min = array[0];
    int idx = 0;
    for (int i = 1; i < size; ++i) {
        if (array[i] < min) {
            min = array[i];
            idx = i;
        }
    }
    return idx;
}

template <class Iterator>
const typename std::iterator_traits<Iterator>::value_type&
    MaxInRange(Iterator begin, Iterator end, Iterator& maxPos) {
    assert(begin != end);
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

template <class Iterator>
const typename std::iterator_traits<Iterator>::value_type&
    MinInRange(Iterator begin, Iterator end, Iterator& minPos) {
    assert(begin != end);
    minPos = begin;
    for (++begin; begin != end; ++begin) {
        if (*begin < *minPos) {
            minPos = begin;
        }
    }
    return *minPos;
}

template <class Iterator>
const typename std::iterator_traits<Iterator>::value_type&
    MinInRange(Iterator begin, Iterator end) {
    Iterator minPos;
    return MinInRange(begin, end, minPos);
}

inline double RelativeError(double expect, double actual) {
    return Abs(actual - expect) / expect;
}

template <class T>
inline T Sqr(const T& x) {
    return x * x;
}

inline double Sqrt(double x) {
    return ::sqrt(x);
}

inline double Sqrt(float x) {
    return ::sqrtf(x);
}

inline double Log(double x) {
    return ::log(x);
}

inline int Round(double x) {
    return static_cast<int>(::round(x));
}

inline int Round(float x) {
    return static_cast<int>(::roundf(x));
}

inline int Ceil(double x) {
    return static_cast<int>(::ceil(x));
}

inline int Ceil(float x) {
    return static_cast<int>(::ceilf(x));
}

inline int Floor(double x) {
    return static_cast<int>(::floor(x));
}

inline int Floor(float x) {
    return static_cast<int>(::floorf(x));
}

inline double Sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

int Factorial(int n);

} //~ namespace Math

} //~ namespace longan

#endif // COMMON_MATH_MATH_H
