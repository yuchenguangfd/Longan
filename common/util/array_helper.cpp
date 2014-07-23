/*
 * array_helper.cpp
 * Created on: Jul 22, 2014
 * Author: chenguangyu
 */

#include "array_helper.h"
#include <cassert>

namespace longan {

namespace ArrayHelper {

// fill int array using 0, 1, 2, ..., n-1
void FillRange(int *array, int n) {
    assert(n >= 0);
    for (int i = 0; i < n; ++i) {
        array[i] = i;
    }
}

} //~ namespace ArrayHelper

}  //~ namespace longan
