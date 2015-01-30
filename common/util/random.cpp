/*
 * random.cpp
 * Created on: Jul 21, 2014
 * Author: chenguangyu
 */

#include "random.h"
#include <cmath>
#include <ctime>

namespace longan {

Random* Random::mInstance = nullptr;

Random& Random::Instance() {
    if (mInstance == nullptr) {
        static Random rand;
        mInstance = &rand;
    }
    return *mInstance;
}

Random::Random() {
    mState = time(nullptr);
}

double Random::Gauss() {
    static double v1, v2, s;
    static int phase = 0;
    double x;
    if (phase == 0) {
    	do {
            double U1 = NextDouble();
            double U2 = NextDouble();
            v1 = 2 * U1 - 1;
            v2 = 2 * U2 - 1;
            s = v1 * v1 + v2 * v2;
        } while(s >= 1 || s == 0);
        x = v1 * sqrt(-2 * log(s) / s);
    } else {
        x = v2 * sqrt(-2 * log(s) / s);
    }
    phase = 1 - phase;
    return x;
}

double Random::Gauss(double mean, double std) {
	return mean + Gauss() * std;
}

} //~ namespace longan
