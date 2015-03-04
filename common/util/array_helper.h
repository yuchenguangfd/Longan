/*
 * array_helper.h
 * Created on: Jul 22, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_UTIL_ARRAY_HELPER_H
#define COMMON_UTIL_ARRAY_HELPER_H

#include "common/util/random.h"
#include "common/base/algorithm.h"
#include <cassert>

namespace longan {

namespace ArrayHelper {

template <class T>
void RandomShuffle(T *array, int size) {
    Random& rnd = Random::Instance();
    for(int i = 0; i < size; ++i) {
        int p = rnd.Uniform(i, size);
        Swap(array[i], array[p]);
    }
}

template <class T>
void CreateArray1D(T **pArr1D, int size) {
    assert(pArr1D != nullptr);
    assert(size > 0);
    *pArr1D = new T[size];
}

template <class T>
void ReleaseArray1D(T **pArr1D, int size) {
    assert(pArr1D != nullptr);
    assert(size > 0);
    delete [](*pArr1D);
    *pArr1D = nullptr;
}

template <class T>
void InitArray1D(T *arr1D, int size, T initVal = T()) {
    for (int i = 0; i < size; ++i) {
        arr1D[i] = initVal;
    }
}

template <class T>
void CopyArray1D(const T *src, T *dst, int size) {
    for (int i = 0; i < size; ++i) {
        dst[i] = src[i];
    }
}

template <class T>
void CreateArray2D(T ***pArr2D, int rows, int cols) {
	T **mat = new T*[rows];
	for (int i = 0; i < rows; ++i) {
		mat[i] = new T[cols];
	}
	*pArr2D = mat;
}

template <class T>
void ReleaseArray2D(T ***pArr2D, int rows, int cols) {
    T **mat = *pArr2D;
    if (mat == nullptr) return;
    for (int i = 0; i < rows; ++i) {
        delete []mat[i];
    }
    delete []mat;
    *pArr2D = nullptr;
}

template <class T>
void InitArray2D(T **arr2D, int rows, int cols, T initVal = T()) {
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			arr2D[i][j] = initVal;
		}
	}
}

template <class T>
void CopyArray2D(T **src, T **dst, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            dst[i][j] = src[i][j];
        }
    }
}

void FillRange(int *array, int size);

void FillRandom(double *array, int size);
void FillRandom(double *array, int size, double from, double to);

} //~ namespace ArrayHelper

} //~ namespace longan

#endif /* COMMON_UTIL_ARRAY_HELPER_H */
