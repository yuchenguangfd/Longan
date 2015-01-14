/*
 * defines.h
 * Created on: Nov 15, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_LANG_DEFINES_H
#define COMMON_LANG_DEFINES_H

#include <cstddef>
#include <cstring>

#define DISALLOW_COPY(ClassName) \
    ClassName(const ClassName&) = delete;

#define DISALLOW_ASSIGN(ClassName) \
    void operator= (const ClassName&) = delete;

#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
    ClassName(const ClassName&) = delete; \
    void operator= (const ClassName&) = delete;

#define DISALLOW_IMPLICIT_CONSTRUCTORS(ClassName) \
    ClassName() = delete; \
    ClassName(const ClassName&) = delete; \
    void operator= (const ClassName&) = delete;

template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

template<typename To, typename From>
inline To implicit_cast(From const &f) { return f; }

#define dprint(expr) printf(#expr " = %g\n", expr)

#endif // COMMON_LANG_DEFINES_H
