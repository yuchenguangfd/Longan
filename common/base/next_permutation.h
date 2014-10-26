/*
 * next_permutation.h
 * Created on: Oct 23, 2014
 * Author: chenguangyu
 */

#include "algorithm.h"

namespace longan {

template <class T>
bool NextPermutation(T* array, int size) {
    int j;
    for (j = size-2; j >= 0 && array[j+1] <= array[j]; --j) { }
    if (j < 0) {
        Reverse(array, size);
        return false;
    }
    int k;
    for(k = size-1; array[k] <= array[j]; --k) { }
    Swap(array[j], array[k]);
    for(k = j+1; k <= (j+size)/2; ++k) {
        Swap(array[k], array[size+j-k]);
    }
    return true;
}

} //~ namespace longan
