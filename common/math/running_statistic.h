/*
 * running_statistic.h
 * Created on: Jul 21, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_MATH_RUNNING_STATISTIC_H
#define COMMON_MATH_RUNNING_STATISTIC_H

#include "math.h"
#include "algorithm/data_structure/binary_heap.h"
#include <limits>

namespace longan {

template <class T>
class RunningMin {
public:
    RunningMin() {
        mCurrentMin = std::numeric_limits<T>::max();
    }
    void Add(T val) {
        if (val < mCurrentMin) {
            mCurrentMin = val;
        }
    }
    T CurrentMin() const {
        return mCurrentMin;
    }
private:
    T mCurrentMin;
};

template <class T>
class RunningMax {
public:
    RunningMax() {
        mCurrentMax = -std::numeric_limits<T>::max();
        if (std::numeric_limits<T>::min() < mCurrentMax) {
            mCurrentMax = std::numeric_limits<T>::min();
        }
    }
    void Add(T val) {
        if (mCurrentMax < val) {
            mCurrentMax = val;
        }
    }
    T CurrentMax() const {
        return mCurrentMax;
    }
private:
    T mCurrentMax;
};

template <class T>
class RunningMinK {
public:
private:
    MaxBinaryHeap<T> mCurrentMinSet;
};

template <class T>
class RunningAverage {
public:
    RunningAverage(T initSum = T()) {
        mCount = 0;
        mSum = initSum;
    }
    void Add(T val) {
        ++mCount;
        mSum += val;
    }
    T CurrentAverage() const {
        return (mCount == 0)? 0.0 : mSum / mCount;
    }
    T CurrentMean() const {
        return CurrentAverage();
    }
private:
    int mCount;
    T mSum;
};

template <class T>
class RunningStd {
public:
    RunningStd(T initSum = T()) {
        mCount = 0;
        mSum = initSum;
        mSqrSum = initSum;
    }
    void Add(T val) {
        ++mCount;
        mSum += val;
        mSqrSum += val * val;
    }
    T CurrentMean() const {
        return (mCount == 0) ? 0.0 : mSum / mCount;
    }
    T CurrentAverage() const {
        return CurrentMean();
    }
    T CurrentVar() const {
        if (mCount == 0) {
            return 0.0;
        } else {
            T m1 = mSqrSum / mCount;
            T m2 = mSum / mCount;
            return m1 - m2 * m2;
        }
    }
    T CurrentStd() const {
        return Math::Sqrt(CurrentVar());
    }
private:
    int mCount;
    T mSum;
    T mSqrSum;
};

} //~ namespace longan

#endif /* COMMON_MATH_RUNNING_STATISTIC_H */
