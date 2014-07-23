/*
 * array_helper.cpp
 * Created on: Jul 22, 2014
 * Author: chenguangyu
 */

#include "array_helper.h"
#include "random.h"
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

// fill double array using random number in range [0, 1)
void FillRandom(double *array, int size) {
    Random& rnd = Random::Instance();
    for (int i = 0; i < size; ++i) {
        array[i] = rnd.NextDouble();
    }
}

} //~ namespace ArrayHelper

}  //~ namespace longan
