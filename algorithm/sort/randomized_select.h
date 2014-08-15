/*
 * randomized_select.h
 * Created on: Aug 10, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_SORT_RANDOMIZED_SELECT_H
#define ALGORITHM_SORT_RANDOMIZED_SELECT_H

#include "common/util/random.h"

namespace longan {

class RandomizedSelect {
public:
    template <class T>
	T operator() (T *array, int size, int k);
	template <class T, class Comparator>
	T operator() (T *array, int size, int k, Comparator cmp);
private:
	template <class T>
	T Select(T *array, int left, int right, int k);
	template <class T, class Comparator>
	T Select(T *array, int left, int right, int k, Comparator cmp);
};

template <class T>
T RandomizedSelect::operator() (T *array, int size, int k) {
	return Select(array, 0, size - 1, k);
}

template <class T, class Comparator>
T RandomizedSelect::operator() (T *array, int size, int k, Comparator cmp) {
	return Select(array, 0, size - 1, k, cmp);
}

template <class T>
T RandomizedSelect::Select(T *array, int left, int right, int k) {
    int rpos = Random::Instance().Uniform(left, right + 1);
    T key = array[rpos];
    array[rpos] = array[left];
    array[left] = key;
    int i = left, j = right;
    while(i < j) {
        while(i < j && key < array[j]) --j;
        if (i < j) {
            array[i] = array[j];
            ++i;
        }
        while(i < j && array[i] < key) ++i;
        if(i<j) {
            array[j] = array[i];
            --j;
        }
    }
    array[i] = key;
    if(k == i) {
        return array[i];
    } else if(k < i) {
        return Select(array, left, i-1, k);
    } else {
        return Select(array, i+1, right, k);
    }
}

template <class T, class Comparator>
T RandomizedSelect::Select(T *array, int left, int right, int k, Comparator cmp) {
	int rpos = Random::Instance().Uniform(left, right+1);
	T key = array[rpos];
	array[rpos] = array[left];
	array[left] = key;
	int i = left, j = right;
	while(i < j) {
		while(i<j && cmp(key, array[j]) < 0) --j;
		if(i<j) {
			array[i] = array[j];
			++i;
		}
		while(i<j && cmp(array[i], key) < 0) ++i;
		if(i<j)	{
			array[j] = array[i];
			--j;
		}
	}
	array[i] = key;
    if(k == i) {
		return array[i];
	} else if(k < i) {
		return select(array, left, i-1, k, cmp);
	} else {
		return select(array, i+1, right, k, cmp);
	}
}

} //~ namespace longan

#endif /* ALGORITHM_SORT_RANDOMIZED_SELECT_H */
