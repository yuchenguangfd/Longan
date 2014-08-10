/*
 * quick_sort.h
 * Created on: Jul 24, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_SORT_QUICK_SORT_H
#define ALGORITHM_SORT_QUICK_SORT_H

#include "insertion_sort.h"

namespace longan {

class QuickSort {
public:
    template <class T>
    void operator() (T *array, int size);
    template <class T, class Comparator>
    void operator() (T *array, int size, Comparator cmp);
protected:
    template <class T>
    void DoQuickSort(T *array, int left, int right);
    template <class T, class Comparator>
    void DoQuickSort(T *array, int left, int right, Comparator cmp);
    template <class T>
    int Partition(T *array, int left, int right);
    template <class T, class Comparator>
    int Partition(T *array, int left, int right, Comparator cmp);
    template <class T>
    void CompareExchange(T &a, T &b) { if (b < a) Exchange(a, b); }
    template <class T, class Comparator>
    void CompareExchange(T &a, T &b, Comparator cmp) { if (cmp(b, a) < 0) Exchange(a, b); }
    template <class T>
    void Exchange(T &a, T &b) { T temp = a; a = b; b = temp; }
protected:
    static const int sThreshold = 16;
    InsertionSort mInsertionSort;
};

template <class T>
void QuickSort::operator () (T *array, int size) {
    DoQuickSort(array, 0, size - 1);
    mInsertionSort(array, size);
}

template <class T, class Comparator>
void QuickSort::operator () (T *array, int size, Comparator cmp) {
    DoQuickSort(array, 0, size - 1, cmp);
    mInsertionSort(array, size, cmp);
}

template <class T>
void QuickSort::DoQuickSort(T *array, int left, int right) {
    if (right - left < sThreshold) return;
    // median of 3 partition
    Exchange(array[(left+right)/2], array[left+1]);
    CompareExchange(array[left], array[left+1]);
    CompareExchange(array[left], array[right]);
    CompareExchange(array[left+1], array[right]);
    int mid = Partition(array, left+1, right-1);
    DoQuickSort(array, left, mid-1);
    DoQuickSort(array, mid+1, right);
}

template <class T, class Comparator>
void QuickSort::DoQuickSort(T *array, int left, int right, Comparator cmp) {
    if (right - left < sThreshold) return;
    // median of 3 partition
    Exchange(array[(left+right)/2], array[left+1]);
    CompareExchange(array[left], array[left+1], cmp);
    CompareExchange(array[left], array[right], cmp);
    CompareExchange(array[left+1], array[right], cmp);
    int mid = Partition(array, left+1, right-1, cmp);
    DoQuickSort(array, left, mid-1, cmp);
    DoQuickSort(array, mid+1, right, cmp);
}

template <class T>
int QuickSort::Partition(T *array, int left, int right) {
    T key = array[left];
    int i = left, j = right;
    while (i < j) {
        while (i < j && key < array[j]) --j;
        if (i < j) {
            array[i] = array[j];
            ++i;
        }
        while (i < j && array[i] < key) ++i;
        if (i < j) {
            array[j] = array[i];
            --j;
        }
    }
    array[i] = key;
    return i;
}

template <class T, class Comparator>
int QuickSort::Partition(T *array, int left, int right, Comparator cmp) {
    T key = array[left];
    int i = left, j = right;
    while (i < j) {
        while (i < j && cmp(key, array[j]) < 0) --j;
        if (i < j) {
            array[i] = array[j];
            ++i;
        }
        while (i < j && cmp(array[i], key) < 0) ++i;
        if (i < j) {
            array[j] = array[i];
            --j;
        }
    }
    array[i] = key;
    return i;
}

} //~ namespace longan

#endif // ALGORITHM_SORT_QUICK_SORT_H
