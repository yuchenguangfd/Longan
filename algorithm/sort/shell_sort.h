/*
 * shell_sort.h
 * Created on: Jul 30, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_SORT_SHELLSORT_H
#define ALGORITHM_SORT_SHELLSORT_H

namespace longan {

class ShellSort {
public:
    template <class T>
    void operator() (T *array, int size);
    template <class T, class Comparator>
    void operator() (T *array, int size, Comparator cmp);
protected:
    int NextGap(int n)  { return n / 3 + 1; }
};

template <class T>
void ShellSort::operator () (T *array, int size) {
    int gap = size;
    do {
        gap = NextGap(gap);
        for(int i = gap; i < size; ++i) {
            T key = array[i];
            int j = i - gap;
            while(j >= 0 && key < array[j]) {
                array[j + gap] = array[j];
                j -= gap;
            }
            array[j + gap] = key;
        }
    } while(gap > 1);
}

template <class T, class Comparator>
void ShellSort::operator() (T *array, int size, Comparator cmp) {
    int gap = size;
    do {
        gap = NextGap(gap);
        for(int i = gap; i < size; ++i) {
            T key = array[i];
            int j = i - gap;
            while(j >= 0 && cmp(key, array[j]) < 0) {
                array[j + gap] = array[j];
                j -= gap;
            }
            array[j + gap] = key;
        }
    } while(gap > 1);
}

} //~ namespace longan

#endif // ALGORITHM_SORT_SHELLSORT_H
