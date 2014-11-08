/*
 * running_statistic.h
 * Created on: Jul 21, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_MATH_RUNNING_STATISTIC_H
#define COMMON_MATH_RUNNING_STATISTIC_H

#include "algorithm/data_structure/binary_heap.h"
#include "common/math/math.h"
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
    RunningMinK(int k) : mK(k), mCurrentMinK(k) { }
    void Add(const T& val) {
        if (mCurrentMinK.Size() < mK) {
            mCurrentMinK.Add(val);
        } else if (val < mCurrentMinK.Top()){
            mCurrentMinK.Modify(0, val);
        }
    }
    std::vector<T> CurrentMinK() const {
        std::vector<T> minSet(mCurrentMinK.Begin(), mCurrentMinK.End());
        std::sort(minSet.begin(), minSet.end());
        return minSet;
    }
    const T* CurrentMinKBegin() const {
        return mCurrentMinK.Begin();
    }
    const T* CurrentMinKEnd() const {
        return mCurrentMinK.End();
    }
private:
    int mK;
    MaxBinaryHeap<T> mCurrentMinK;
};

template <class T>
class RunningMaxK {
public:
    RunningMaxK(int k) : mK(k), mCurrentMaxK(k) { }
    void Add(const T& val) {
        if (mCurrentMaxK.Size() < mK) {
            mCurrentMaxK.Add(val);
        } else if (mCurrentMaxK.Top() < val) {
            mCurrentMaxK.Modify(0, val);
        }
    }
    std::vector<T> CurrentMaxK() const {
        std::vector<T> maxSet(mCurrentMaxK.Begin(), mCurrentMaxK.End());
        std::sort(maxSet.begin(), maxSet.end(),
                [](const T& lhs, const T& rhs)->bool { return rhs < lhs; });
        return maxSet;
    }
    const T* CurrentMaxKBegin() const {
        return mCurrentMaxK.Begin();
    }
    const T* CurrentMaxKEnd() const {
        return mCurrentMaxK.End();
    }
private:
    int mK;
    MinBinaryHeap<T> mCurrentMaxK;
};

template <class T>
class RunningAverage {
public:
    RunningAverage(T initSum = T()) {
        mCount = 0;
        mSum = initSum;
    }
    void Add(const T& val) {
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
    void Add(const T& val) {
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