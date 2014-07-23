/*
 * merge_sort.h
 * Created on: Jul 23, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_SORT_MERGE_SORT_H
#define ALGORITHM_SORT_MERGE_SORT_H

namespace longan {

class MergeSort {
public:
	template <class T>
    void operator() (T *array, int size);
	template <class T, class Comparator>
	void operator() (T *array, int size, Comparator cmp);
private:
	template <class T>
    void DoMergeSort(T *array, T *temp, int left, int right);
	template <class T, class Comparator>
	void DoMergeSort(T *array, T *temp, int left, int right, Comparator cmp);
	template <class T>
	void Merge(T *array, T *temp, int left, int mid, int right);
	template <class T, class Comparator>
	void Merge(T *array, T *temp, int left, int mid, int right, Comparator cmp);
	template <class T>
	void InsertionSort(T *array, int left, int right);
	template <class T, class Comparator>
	void InsertionSort(T *array, int left, int right, Comparator cmp);
private:
	static const int sThreshold = 16;
};

template <class T>
void MergeSort::operator () (T *array, int size) {
    T *temp = new T[size];
    DoMergeSort(array, temp, 0, size - 1);
    delete []temp;
}

template <class T, class Comparator>
void MergeSort::operator () (T *array, int size, Comparator cmp) {
    T *temp = new T[size];
    DoMergeSort(array, temp, 0, size - 1, cmp);
    delete []temp;
}

template <class T>
void MergeSort::DoMergeSort(T *array, T *temp, int left, int right) {
    if (right - left < sThreshold) {
    	InsertionSort(array, left, right);
    } else {
    	int mid = (left + right) / 2;
    	DoMergeSort(array, temp, left, mid);
    	DoMergeSort(array, temp, mid + 1, right);
    	Merge(array, temp, left, mid, right);
    }
}

template <class T, class Comparator>
void MergeSort::DoMergeSort(T *array, T *temp, int left, int right, Comparator cmp) {
	if (right - left < sThreshold) {
		InsertionSort(array, left, right, cmp);
	} else {
		int mid = (left + right) / 2;
		DoMergeSort(array, temp, left, mid, cmp);
		DoMergeSort(array, temp, mid + 1, right, cmp);
		Merge(array, temp, left, mid, right, cmp);
	}
}

template <class T>
void MergeSort::Merge(T *array, T *temp, int left, int mid, int right) {
    for (int i = left; i <= right; ++i) {
    	temp[i] = array[i];
    }
    int i = left, j = mid+1, k = left;
    while (i <= mid && j <= right) {
        if (temp[i] < temp[j]) {
            array[k++] = temp[i++];
        } else {
            array[k++] = temp[j++];
        }
    }
    while (i <= mid) {
        array[k++] = temp[i++];
    }
    while (j <= right) {
        array[k++] = temp[j++];
    }
}

template <class T, class Comparator>
void MergeSort::Merge(T *array, T *temp, int left, int mid, int right, Comparator cmp) {
	for (int i = left; i <= right; ++i) {
		temp[i] = array[i];
	}
	int i = left, j = mid + 1, k = left;
	while (i <= mid && j <= right) {
		if (cmp(temp[i], temp[j]) < 0) {
			array[k++] = temp[i++];
		} else {
			array[k++] = temp[j++];
		}
	}
	while (i <= mid) {
		array[k++] = temp[i++];
	}
	while (j <= right) {
		array[k++] = temp[j++];
	}
}

template <class T>
void MergeSort::InsertionSort(T *array, int left, int right) {
    for(int j = left + 1; j <= right; ++j) {
        int i = j - 1;
        T key = array[j];
        while(i >= left && key < array[i]) {
            array[i+1] = array[i];
            --i;
        }
        array[i+1] = key;
    }
}

template <class T, class Comparator>
void MergeSort::InsertionSort(T *array, int left, int right, Comparator cmp) {
    for(int j = left + 1; j <= right; ++j) {
        int i = j - 1;
        T key = array[j];
        while(i >= left && cmp(key, array[i]) < 0) {
            array[i+1] = array[i];
            --i;
        }
        array[i+1] = key;
    }
}

} //~ namespace longan

#endif // ALGORITHM_SORT_MERGE_SORT_H
