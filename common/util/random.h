/*
 * random.h
 * Created on: Jul 21, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_UTIL_RANDOM_H
#define COMMON_UTIL_RANDOM_H

#include "common/lang/types.h"

namespace longan {

class Random {
public:
    static Random& Instance();
public:
    void ChangeSeed(uint64 seed) {
    	mState = seed;
    }
    uint32 NextInt() {
        mState = (uint64)(unsigned)mState * 4164903690U + (mState >> 32);
        return (uint32)mState;
    }
    double NextDouble() {
    	return (double)NextInt() / 0xffffffff;
    }
    int Uniform(int start, int end) {
    	return start + NextInt() % (end - start);
    }
	double Uniform(double begin, double end) {
		return begin + NextDouble() * (end - begin);
	}
	double Gauss();
	double Gauss(double mean, double std);
protected:
    Random();
    Random(const Random&) = delete;
    Random(Random&&) = delete;
    Random& operator = (const Random&) = delete;
    Random& operator = (Random&&) = delete;
    ~Random() { };
private:
    static Random *mInstance;
    uint64 mState;
};

} //~ namespace longan

#endif
