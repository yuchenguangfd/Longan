/*
 * double.cpp
 * Created on: Sep 8, 2014
 * Author: chenguangyu
 */

#include "double.h"
#include "character.h"
#include <limits>
#include <cstdio>

namespace longan {

const double Double::INF = std::numeric_limits<double>::max();
const double Double::EPS = std::numeric_limits<double>::min();

double Double::ParseDouble(const std::string& s) {
    double d;
    sscanf(s.c_str(), "%lf", &d);
    return d;
}

} //~ namespace longan
