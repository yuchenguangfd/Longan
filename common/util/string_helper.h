/*
 * string_helper.h
 * Created on: Jul 29, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_UTIL_STRING_HELPER_H
#define COMMON_UTIL_STRING_HELPER_H

#include <string>
#include <vector>

namespace longan {

namespace StringHelper {

std::vector<std::string> Split(const std::string& src, const std::string& delimit, bool ignoreEmptyString = true);

int CountWhitespace(const std::string& str);

}

} //~ namespace longan

#endif /* COMMON_UTIL_STRING_HELPER_H */
