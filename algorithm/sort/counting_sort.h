/*
 * counting_sort.h
 * Created on: Jul 25, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_COUNTING_SORT_H
#define ALGORITHM_COUNTING_SORT_H

#include <cassert>

namespace longan {

class CountingSort {
public:
    template <class T, class IntValConvertor>
    void operator() (T *array, int size, int limit, IntValConvertor cvt);
    template <class T, class IntValConvertor>
    void operator() (T *src, T *dst, int size, int limit, IntValConvertor cvt);
};

template <class T, class IntValConvertor>
void CountingSort::operator() (T *array, int size, int limit, IntValConvertor cvt) {
	assert(limit > 0);
	int *count = new int[limit];
	for(int i = 0; i < limit; ++i) {
		count[i] = 0;
	}
	T *array2 = new T[size];
	for(int i = 0; i < size; ++i) {
		array2[i] = array[i];
		++count[cvt(array[i])];
	}
	for(int i = 1; i < limit; ++i) {
		count[i] += count[i-1];
	}
	for(int i = size - 1; i >= 0; --i){
		array[--count[cvt(array2[i])]] = array2[i];
	}
	delete []array2;
	delete []count;
}

template <class T, class IntValConvertor>
void CountingSort::operator () (T *src, T *dst, int size, int limit, IntValConvertor cvt) {
    assert(limit > 0);
    int *count = new int[limit];
    for (int i = 0; i < limit; ++i) {
        count[i] = 0;
    }
    for (int i = 0; i < size; ++i) {
        ++count[cvt(src[i])];
    }
    for (int i = 1; i < limit; ++i) {
        count[i] += count[i-1];
    }
    for (int i = size - 1; i >= 0; --i) {
        dst[--count[cvt(src[i])]] = src[i];
    }
    delete []count;
}

} //~ namespace ycg

#endif // COUNTINGSORTER_H
