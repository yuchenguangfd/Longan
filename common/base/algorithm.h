/*
 * algorithm.h
 * Created on: Jul 21, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_BASE_ALGORITHM_H
#define COMMON_BASE_ALGORITHM_H

namespace longan {

template <class T>
inline void Swap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

template<class T>
void Reverse(T *array, int size) {
    if (size == 0) return;
    int mid = size / 2;
    for (int i = 0; i < mid; ++i) {
        Swap(array[i], array[size - 1 - i]);
    }
}

//template<class _InIt,   class _Fn1> inline
//void _For_each(_InIt _First, _InIt _Last, _Fn1& _Func) {
//    for (; _First != _Last; ++_First)
//        _Func(*_First);
//}


} //~ namespace longan

#endif /* COMMON_BASE_ALGORITHM_H */
