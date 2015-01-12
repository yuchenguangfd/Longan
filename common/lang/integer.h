/*
 * integer.h
 * Created on: Aug 4, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_LANG_INTEGER_H
#define COMMON_LANG_INTEGER_H

#include <string>

namespace longan {

class Integer {
public:
    static const int MAX = 0x7FFFFFFF;
    static int ParseInt(const std::string& s);
    static std::string ToString(int num);
    static int ReverseByteOrder(int x);
public:
    Integer() : mValue(0) { }
    Integer(int i) : mValue(i) { }
private:
    int mValue;
};

} //~ namespace longan

#endif // COMMON_LANG_INTEGER_H
