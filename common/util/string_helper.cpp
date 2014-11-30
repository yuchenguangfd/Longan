/*
 * string_helper.cpp
 *      Author: chenguangyu
 */

#include "string_helper.h"
#include "common/lang/character.h"
#include "common/lang/integer.h"

namespace longan {

std::vector<std::string> StringHelper::Split(const std::string& src, const std::string& delimit, bool ignoreEmptyString) {
    if (src.empty()) {
        return std::vector<std::string>();
    }
    if (delimit.empty()) {
        return std::vector<std::string>(1, src);
    }
    std::vector<std::string> subStrings;
    long index = std::string::npos, last_search_position = 0;
    while ((index = src.find(delimit, last_search_position)) != std::string::npos) {
        if (index != last_search_position || !ignoreEmptyString) {
            subStrings.push_back(src.substr(last_search_position, index - last_search_position));
        }
        last_search_position = index + delimit.size();
    }
    std::string last_one = src.substr(last_search_position);
    if (!last_one.empty() || !ignoreEmptyString) {
        subStrings.push_back(last_one);
    }
    return subStrings;
}

std::string StringHelper::ToString(int i) {
    return Integer::ToString(i);
}

int StringHelper::CountWhitespace(const std::string& str) {
	int cnt = 0;
	for (char ch : str) {
		if (Character::IsWhitespace(ch)) {
			++cnt;
		}
	}
	return cnt;
}

} //~ namespace longan
