/*
 * integer.h
 * Created on: Aug 4, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_LANG_INTEGER_H
#define COMMON_LANG_INTEGER_H

#include "types.h"
#include <string>
#include <limits>

namespace longan {

class Integer {
public:
    static const int MAX = std::numeric_limits<int>::max();
    static std::string ToString(int num);
    static std::string ToString(int num, int fieldWidth);
    static int ParseInt(const std::string& s);
    static int64 ParseInt64(const std::string& s);
    static int ReverseByteOrder(int x);
public:
    Integer() : mValue(0) { }
    Integer(int i) : mValue(i) { }
private:
    int mValue;
};

} //~ namespace longan

#endif // COMMON_LANG_INTEGER_H
