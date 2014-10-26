/*
 * bubble_sort.h
 * Created on: Jul 23, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_SORT_BUBBLE_SORT_H
#define ALGORITHM_SORT_BUBBLE_SORT_H

#include "common/base/algorithm.h"

namespace longan {

class BubbleSort {
public:
	template <class T>
	void operator() (T *array, int size);
	template <class T, class Comparator>
    void operator() (T *array, int size, Comparator cmp);
};

template <class T>
void BubbleSort::operator ()(T *array, int size) {
	bool isSorted = false;
	for (int i = 0; !isSorted && i < size; ++i) {
		isSorted = true;
		for (int j = size - 1; j > i; --j) {
			if (array[j] < array[j-1]) {
				Swap(array[j], array[j-1]);
				isSorted = false;
			}
		}
	}
}

template <class T, class Comparator>
void BubbleSort::operator ()(T *array, int size, Comparator cmp) {
	bool isSorted = false;
    for (int i = 0; !isSorted && i < size; ++i) {
		isSorted = true;
        for (int j = size - 1; j > i; --j) {
            if (cmp(array[j], array[j - 1]) < 0) {
                Swap(array[j], array[j-1]);
				isSorted = false;
			}
		}
	}
}

} //~ namespace longan

#endif /* ALGORITHM_SORT_BUBBLE_SORT_H */
