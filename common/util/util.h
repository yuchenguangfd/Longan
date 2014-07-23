/*
 * util.h
 * Created on: Jul 22, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_UTIL_UTIL_H
#define COMMON_UTIL_UTIL_H

namespace longan {

template <class T>
inline void Swap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

} //~ namespace longan

#endif /* COMMON_UTIL_UTIL_H */
