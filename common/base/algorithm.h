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

template <class T1, class T2>
int BSearch(const T1& key, const T2 *array, int size) {
    int left = 0;
    int right = size - 1;
    while(left <= right) {
        int mid = left + (right - left) / 2;
        if(array[mid] == key) {
            return mid;
        } else if(array[mid] < key) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
   }
   return -1;
}

template <class T1, class T2, class Comparator>
int BSearch(const T1& key, const T2 *array, int size, Comparator cmp) {
    int left = 0;
    int right = size - 1;
    while(left <= right) {
        int mid = left + (right - left) / 2;
        if(cmp(key, array[mid]) == 0) {
            return mid;
        } else if(cmp(key, array[mid]) > 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
   }
   return -1;
}

} //~ namespace longan

#endif /* COMMON_BASE_ALGORITHM_H */
