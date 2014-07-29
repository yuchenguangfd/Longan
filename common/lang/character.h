/*
 * character.h
 * Created on: Jul 29, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_LANG_CHARACTER_H
#define COMMON_LANG_CHARACTER_H

namespace longan {

class Character {
public:
    static bool IsDigit(char ch) {
        return (ch >= '0') && (ch <= '9');
    }

    static bool IsLetter(char ch) {
        return  (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
    }

    static bool IsLowerCase(char ch) {
        return (ch >= 'a' && ch <= 'z');
    }

    static bool IsUpperCase(char ch) {
        return (ch >= 'A' && ch <= 'Z');
    }

    static bool IsWhitespace(char ch) {
        return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
    }

    static char ToLower(char ch) {
        if (IsUpperCase(ch)) {
            char lower = ch - 'A' + 'a';
            return lower;
        } else {
            return ch;
        }
    }

    static char ToUpper(char ch) {
        if (IsUpperCase(ch)) {
            char lower = ch - 'A' + 'a';
            return lower;
        } else {
            return ch;
        }
    }
};

}  // namespace longan

#endif // COMMON_LANG_CHARACTER_H
