/*
 * next.cpp
 * Created on: May 5, 2015
 * Author: chenguangyu
 */

#include "next.h"

namespace longan {

bool NextCombination(int *array, int n, int r) {
    int i = r - 1;
    while (i >= 0 && array[i] == n-r+i) {
    	--i;
    }
    if (i < 0) return false;
    ++array[i];
    for (int j = i + 1; j < r; ++j) {
    	array[j] = array[j-1]+1;
    }
    return true;
}

bool NextPermutation(int *array, int n) {
    if (n <= 1) return false;
    int j;
    for (j = n - 2; j >= 0 && array[j+1] <= array[j]; --j) { }
    if (j < 0) {
        int mid = n / 2;
        for (int i = 0; i < mid; ++i) {
            int temp = array[i];
            array[i] = array[n - 1 - i];
            array[n - 1 - i] = temp;
        }
        return false;
    }
    int k;
    for(k = n-1; array[k] <= array[j]; --k) { }
    int temp = array[j];
    array[j] = array[k];
    array[k] = temp;
    for(k = j+1; k <= (j + n) / 2; ++k) {
        int temp = array[k];
        array[k] = array[n + j - k];
        array[n+j-k] = temp;
    }
    return true;
}

} //~ namespace longan
