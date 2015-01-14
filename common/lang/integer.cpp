/*
 * integer.cpp
 * Created on: Aug 5, 2014
 * Author: chenguangyu
 */

#include "integer.h"
#include "character.h"
#include "common/base/algorithm.h"

namespace longan {

int Integer::ParseInt(const std::string &s) {
    int i;
    sscanf(s.c_str(), "%d", &i);
    return i;
}

std::string Integer::ToString(int num) {
    char buff[16];
    sprintf(buff, "%d", num);
    return std::string(buff);
}

int Integer::ReverseByteOrder(int x) {
    return ((x & 0xFF) << 24)
         + ((x & 0xFF00) << 8)
         + ((x & 0xFF0000) >> 8)
         + ((x & 0xFF000000) >> 24);
}

} //~ namespace longan
