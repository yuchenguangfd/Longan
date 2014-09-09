/*
 * double.h
 * Created on: Sep 8, 2014
 * Author: chenguangyu
 */
#ifndef COMMON_LANG_DOUBLE_H
#define COMMON_LANG_DOUBLE_H

#include <string>
#include <cmath>

namespace longan {

class Double {
public:
    static const double INF;
public:
	static double ParseDouble(const std::string& s);

    inline static int DblCmp(double x, double eps = 1e-10) {
        if (fabs(x) < eps) {
            return 0;
        } else if (x > 0) {
            return 1;
        } else {
            return -1;
        }
    }

    inline static bool DblZero(double x, double eps = 1e-10) {
        return fabs(x) < eps;
    }

    inline static bool DblEqual(double a, double b, double eps = 1e-10) {
        return fabs(a - b) < eps;
    }

    inline static bool DblNotEqual(double a, double b, double eps = 1e-10) {
        return fabs(a - b) >= eps;
    }

};

} //~ namespace longan

#endif // COMMON_LANG_DOUBLE_H
