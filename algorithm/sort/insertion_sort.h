/*
 * insertion_sort.h
 * Created on: Jul 23, 2014
 * Author: chenguangyu
 */
#ifndef ALGORITHM_SORT_INSERTION_SORT_H
#define ALGORITHM_SORT_INSERTION_SORT_H

namespace longan {

class InsertionSort {
public:
    template <class T>
    void operator() (T *array, int size);
    template <class T, class Comparator>
    void operator() (T *array, int size, Comparator cmp);
};

template <class T>
void InsertionSort::operator() (T *array, int size) {
    for (int j = 1; j < size; ++j) {
        int i = j - 1;
        T key = array[j];
        while(i >= 0 && key < array[i]) {
            array[i + 1] = array[i];
            --i;
        }
        array[i+1] = key;
    }
}

template <class T, class Comparator>
void InsertionSort::operator() (T *array, int size, Comparator cmp) {
	for (int j = 1; j < size; ++j) {
		int i = j - 1;
		T key = array[j];
		while (i >= 0 && cmp(key, array[i]) < 0) {
			array[i + 1] = array[i];
			--i;
		}
		array[i+1] = key;
	}
}

} //~ namespace longan

#endif // ALGORITHM_SORT_INSERTIONSORT_H
