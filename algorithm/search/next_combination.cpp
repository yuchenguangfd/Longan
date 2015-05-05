/*
 * next_combination.cpp
 * Created on: May 5, 2015
 * Author: chenguangyu
 */

#include "next_combination.h"

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

} //~ namespace longan

