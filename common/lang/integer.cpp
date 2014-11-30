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
    int i = 0;
    while (Character::IsWhitespace(s[i])) ++i;
    int sign = (s[i] == '-') ? -1 : 1;
    if (s[i] == '+' || s[i] == '-') ++i;
    int num = 0;
    while (Character::IsDigit(s[i])) {
        num = 10 * num + (s[i] - '0');
        ++i;
    }
    int result = sign * num;
    return result;
}

std::string Integer::ToString(int num) {
    if (num == 0) return "0";
    int sign = num;
    if (sign < 0) num = -num;
    char buff[16];
    int len = 0;
    while (num > 0) {
        buff[len++] = num % 10 + '0';
        num /= 10;
    }
    if (sign < 0) {
        buff[len++] = '-';
    }
    Reverse(buff, len);
    return std::string(buff, buff + len);
}

int Integer::ReverseByteOrder(int x) {
    return ((x & 0xFF) << 24)
         + ((x & 0xFF00) << 8)
         + ((x & 0xFF0000) >> 8)
         + ((x & 0xFF000000) >> 24);
}

} //~ namespace longan
