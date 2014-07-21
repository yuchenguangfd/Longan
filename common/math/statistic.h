/*
 * statistic.h
 * Created on: Jul 21, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_MATH_STATISTIC_H
#define COMMON_MATH_STATISTIC_H

#include <cmath>

namespace longan {

class RunningStatistic {
public:
    RunningStatistic() :
        mSum(0.0), mCount(0), mSqrSum(0) { }
    void Accumulate(double val) {
        ++mCount;
        mSum += val;
        mSqrSum += val * val;
    }
    double GetMean() const {
        return (mCount == 0) ? 0.0 : mSum / mCount;
    }
    double GetAverage() const {
        return GetMean();
    }
    double GetVariance() const {
        if (mCount == 0) {
            return 0.0;
        } else {
            double m1 = mSqrSum / mCount;
            double m2 = mSum / mCount;
            return m1 - m2 * m2;
        }
    }
    double GetStandardDeviation() {
        return sqrt(GetVariance());
    }
private:
	int mCount;
	double mSum;
    double mSqrSum;
};

} //~ namespace longan

#endif /* STATISTIC_H_ */
