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

// fill int array using 0, 1, 2, ..., size-1
void FillRange(int *array, int size) {
    assert(array != nullptr && size >= 0);
    for (int i = 0; i < size; ++i) {
        array[i] = i;
    }
}

// fill double array using random number in range [0, 1)
void FillRandom(double *array, int size) {
    assert(array != nullptr && size >= 0);
    Random& rnd = Random::Instance();
    for (int i = 0; i < size; ++i) {
        array[i] = rnd.NextDouble();
    }
}

void FillRandom(double *array, int size, double rangeLow, double rangeHigh) {
    assert(array != nullptr && size >= 0);
    Random& rnd = Random::Instance();
    for (int i = 0; i < size; ++i) {
        array[i] = rnd.Uniform(rangeLow, rangeHigh);
    }
}

} //~ namespace ArrayHelper

}  //~ namespace longan
