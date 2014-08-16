/*
 * count_inversion.h
 * Created on: Aug 15, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_SORT_COUNT_INVERSION_H
#define ALGORITHM_SORT_COUNT_INVERSION_H

#include "common/lang/types.h"

namespace longan {

// count number of inversions(i<j, but array[i] > array[j]) using mergesort
class CountInversion {
public:
	template <class T>
	uint64 operator()(T *array, int size);
private:
	template <class T>
	uint64 MergeSort(T *array, T *temp, int left, int right);
};

template <class T>
uint64 CountInversion::operator()(T *array, int size) {
	T *temp = new T[size];
	uint64 invs = MergeSort(array, temp, 0, size - 1);
	delete []temp;
	return invs;
}

template <class T>
uint64 CountInversion::MergeSort(T *array, T *temp, int left, int right) {
	if (left >= right) return 0;
	int mid = (left + right) / 2;
	uint64 invs = MergeSort(array, temp, left, mid) + MergeSort(array, temp, mid + 1, right);
    for (int i = left; i <= right; ++i) {
        temp[i] = array[i];
    }
    int i = left, j = mid+1, k = left;
    while (i <= mid && j <= right) {
        if (temp[i] < temp[j]) {
            array[k++] = temp[i++];
        } else {
            invs += mid - i + 1;
            array[k++] = temp[j++];
        }
    }
    while (i <= mid) array[k++] = temp[i++];
    while (j <= right) array[k++] = temp[j++];
	return invs;
}

}  // namespace longan

#endif /* ALGORITHM_SORT_COUNT_INVERSION_H */
