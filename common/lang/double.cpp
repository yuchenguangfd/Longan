/*
 * double.cpp
 * Created on: Sep 8, 2014
 * Author: chenguangyu
 */

#include "double.h"
#include "character.h"
#include <cstdio>

namespace longan {

const double Double::INF = 1e300;

double Double::ParseDouble(const std::string& s) {
    double d;
    sscanf(s.c_str(), "%lf", &d);
    return d;
}

} //~ namespace longan
