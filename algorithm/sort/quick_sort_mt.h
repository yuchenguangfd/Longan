/*
 * quick_sort_mt.h
 * Created on: Jul 25, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_SORT_QUICK_SORT_MT_H
#define ALGORITHM_SORT_QUICK_SORT_MT_H

#include "quick_sort.h"
#include <thread>

namespace longan {

class QuickSortMT : public QuickSort {
public:
    void SetThreadNum(int threadNum) {
        mThreadNum = threadNum;
    }
	template <class T>
    void operator() (T *array, int size);
    template <class T, class Comparator>
    void operator() (T *array, int size, Comparator cmp);
protected:
    template <class T>
    void MTQuickSort(T *array, int left, int right);
    template <class T, class Comparator>
    void MTQuickSort(T *array, int left, int right, Comparator cmp);
private:
    int mThreadNum = 16;
    int mMTThreshold;
};

template <class T>
void QuickSortMT::operator () (T *array, int size) {
	mMTThreshold = size / mThreadNum;
	MTQuickSort(array, 0, size - 1);
}

template <class T, class Comparator>
void QuickSortMT::operator () (T *array, int size, Comparator cmp) {
	mMTThreshold = size / mThreadNum;
	MTQuickSort(array, 0, size - 1, cmp);
}

template <class T>
void QuickSortMT::MTQuickSort(T *array, int left, int right) {
    if (right - left < sThreshold) {
    	mInsertionSort(array + left, right - left + 1);
    	return;
    }
    Exchange(array[(left+right)/2], array[left+1]);
    CompareExchange(array[left], array[left+1]);
    CompareExchange(array[left], array[right]);
    CompareExchange(array[left+1], array[right]);
    int mid = Partition(array, left+1, right-1);
    if (right - left > mMTThreshold) {
    	std::thread th1([=](T *array, int left, int right) {
    	        MTQuickSort(array, left, right);
    	    }, array, left, mid-1);
    	std::thread th2([=](T *array, int left, int right) {
    	        MTQuickSort(array, left, right);
    	    }, array, mid + 1, right);
    	th1.join();
    	th2.join();
    } else {
    	MTQuickSort(array, left, mid-1);
    	MTQuickSort(array, mid+1, right);
    }
}

template <class T, class Comparator>
void QuickSortMT::MTQuickSort(T *array, int left, int right, Comparator cmp) {
    if (right - left < sThreshold) {
        mInsertionSort(array + left, right - left + 1, cmp);
    	return;
    }
    Exchange(array[(left+right)/2], array[left+1]);
    CompareExchange(array[left], array[left+1], cmp);
    CompareExchange(array[left], array[right], cmp);
    CompareExchange(array[left+1], array[right], cmp);
    int mid = Partition(array, left+1, right-1, cmp);
    if (right - left > mMTThreshold) {
    	std::thread th1([=](T *array, int left, int right, Comparator cmp){
    		MTQuickSort(array, left, right, cmp);}, array, left, mid-1, cmp);
    	std::thread th2([=](T *array, int left, int right, Comparator cmp){
    		MTQuickSort(array, left, right, cmp);}, array, mid + 1, right, cmp);
    	th1.join();
    	th2.join();
    } else {
        MTQuickSort(array, left, mid-1, cmp);
        MTQuickSort(array, mid+1, right, cmp);
    }
}

} //~ namespace longan

#endif // ALGORITHM_SORT_QUICK_SORT_MT_H
