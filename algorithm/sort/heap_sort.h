/*
 * heap_sort.h
 * Created on: Jul 27, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_SORT_HEAP_SORT_H
#define ALGORITHM_SORT_HEAP_SORT_H

namespace longan {

class HeapSort {
public:
    template <class T>
    void operator() (T *array, int size);
    template <class T, class Comparator>
    void operator() (T *array, int size, Comparator cmp);
private:
    template <class T>
    void Build(T *array, int size);
    template <class T, class Comparator>
    void Build(T *array, int size, Comparator cmp);
    template <class T>
    void Heapify(T *array, int size, int i);
    template <class T, class Comparator>
    void Heapify(T *array, int size, int i, Comparator cmp);
    template <class T>
    void Exchange(T &a, T &b);
};

template <class T>
void HeapSort::operator () (T *array, int size) {
    Build(array, size);
	for (int i = size - 1; i > 0; --i) {
        Exchange(array[0], array[i]);
        Heapify(array, i, 0);
	}
}

template <class T, class Comparator>
void HeapSort::operator () (T *array, int size, Comparator cmp) {
    Build(array, size, cmp);
    for (int i = size - 1; i > 0; --i) {
        Exchange(array[0], array[i]);
        Heapify(array, i, 0, cmp);
    }
}

template <class T>
void HeapSort::Build(T *array, int size) {
    for (int i = ((size - 1) >> 1); i >= 0; --i) {
        Heapify(array, size, i);
    }
}

template <class T, class Comparator>
void HeapSort::Build(T *array, int size, Comparator cmp) {
    for (int i = ((size - 1) >> 1); i >= 0; --i) {
        Heapify(array, size, i, cmp);
    }
}

template <class T>
void HeapSort::Heapify(T *array, int size, int i) {
	int j = (i << 1) + 1;
    T key = array[i];
    while (j < size) {
        if (j + 1 < size && array[j] < array[j+1])
			j++;
        if (array[j] < key)
			break;
        array[i] = array[j];
		i = j;
		j = (i << 1) + 1;
	}
    array[i] = key;
}

template <class T, class Comparator>
void HeapSort::Heapify(T *array, int size, int i, Comparator cmp) {
    int j = (i << 1) + 1;
    T key = array[i];
    while (j < size) {
        if (j + 1 < size && cmp(array[j], array[j+1]) < 0)
            j++;
        if (cmp(array[j], key) < 0)
            break;
        array[i] = array[j];
        i = j;
        j = (i << 1) + 1;
    }
    array[i] = key;
}

template <class T>
inline void HeapSort::Exchange(T &a, T &b) {
    T temp = a; a = b; b = temp;
}

} //~ namespace longan

#endif /* ALGORITHM_SORT_HEAP_SORT_H */
