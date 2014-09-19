/*
 * running_statistic.h
 * Created on: Jul 21, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_MATH_RUNNING_STATISTIC_H
#define COMMON_MATH_RUNNING_STATISTIC_H

#include "math.h"
#include "algorithm/data_structure/binary_heap.h"
#include <vector>
#include <algorithm>
#include <limits>

namespace longan {

template <class T>
class RunningMin {
public:
    RunningMin() {
        mCurrentMin = std::numeric_limits<T>::max();
    }
    void Add(const T& val) {
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
    void Add(const T& val) {
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
    RunningMinK(int k) : mK(k), mCurrentMinHeap(k) { }
    void Add(const T& val) {
        if (mCurrentMinHeap.Size() < mK) {
            mCurrentMinHeap.Add(val);
        } else if (val < mCurrentMinHeap.Top()){
            mCurrentMinHeap.Modify(0, val);
        }
    }
    std::vector<T> CurrentMinK() const {
        std::vector<T> minSet(mCurrentMinHeap.Begin(), mCurrentMinHeap.End());
        std::sort(minSet.begin(), minSet.end());
        return minSet;
    }
private:
    int mK;
    MaxBinaryHeap<T> mCurrentMinHeap;
};

template <class T>
class RunningMaxK {
public:
    RunningMaxK(int k) : mK(k), mCurrentMaxHeap(k) { }
    void Add(const T& val) {
        if (mCurrentMaxHeap.Size() < mK) {
            mCurrentMaxHeap.Add(val);
        } else if (mCurrentMaxHeap.Top() < val) {
            mCurrentMaxHeap.Modify(0, val);
        }
    }
    std::vector<T> CurrentMaxK() const {
        std::vector<T> maxSet(mCurrentMaxHeap.Begin(), mCurrentMaxHeap.End());
        std::sort(maxSet.begin(), maxSet.end(),
                [](const T& lhs, const T& rhs)->bool { return rhs < lhs; });
        return maxSet;
    }
private:
    int mK;
    MinBinaryHeap<T> mCurrentMaxHeap;
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
