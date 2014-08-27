/*
 * binary_heap.h
 * Created on: Aug 27, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_DATA_STRUCTURE_BINARY_HEAP_H
#define ALGORITHM_DATA_STRUCTURE_BINARY_HEAP_H

namespace longan {

template <class T>
class BinaryHeap {
public:
    BinaryHeap(int capacity);
    BinaryHeap(int capacity, const T *array, int size);
    virtual ~BinaryHeap();
    void Add(const T& elem);
    T Extract();
    const T& Top() const { return mHeap[0]; }
    int Size() const { return mSize; }
    int Capacity() const { return mCapacity; }
    void Clear() { mSize = 0; }
    void Modify(int i, const T &elem);
    T* Begin() { return mHeap; }
    T* End() { return mHeap + mSize; }
protected:
    virtual void ShiftUp(int i) = 0;
    virtual void ShiftDown(int i) = 0;
protected:
    int mCapacity;
    T* mHeap;
    int mSize;
};

template <class T>
BinaryHeap<T>::BinaryHeap(int capacity) :
    mCapacity(capacity),
    mSize(0),
    mHeap(new T[capacity]) {
}

template <class T>
BinaryHeap<T>::BinaryHeap(int capacity, const T *array, int size) :
    mCapacity(capacity),
    mSize(size),
    mHeap(new T[capacity]) {
    for (int i = 0; i < size; ++i) {
        mHeap[i] = array[i];
    }
}

template <class T>
BinaryHeap<T>::~BinaryHeap() {
    delete []mHeap;
}

template <class T>
void BinaryHeap<T>::Add(const T &elem) {
    mHeap[mSize] = elem;
    ShiftUp(mSize);
    ++mSize;
}

template <class T>
T BinaryHeap<T>::Extract() {
    T top = mHeap[0];
    mHeap[0] = mHeap[--mSize];
    ShiftDown(0);
    return top;
}

template <class T>
void BinaryHeap<T>::Modify(int i, const T &elem) {
    mHeap[i] = elem;
    ShiftUp(i);
    ShiftDown(i);
}

template <class T>
class MaxBinaryHeap : public BinaryHeap<T> {
public:
    MaxBinaryHeap(int capacity) : BinaryHeap<T>(capacity) { };
    MaxBinaryHeap(int capacity, const T *array, int size);
    const T& Max() const  { return this->mHeap[0]; }
protected:
    void ShiftUp(int i);
    void ShiftDown(int i);
};

template <class T>
MaxBinaryHeap<T>::MaxBinaryHeap(int capacity, const T *array, int size) :
    BinaryHeap<T>(capacity, array, size) {
    for (int i = size / 2 - 1; i >= 0; --i) {
        ShiftDown(i);
    }
}

template <class T>
void MaxBinaryHeap<T>::ShiftUp(int i) {
    T *heap = this->mHeap;
    int j = (i-1)>>1;
    T key = heap[i];
    while(i > 0 && heap[j] < key) {
        heap[i] = heap[j];
        i = j;
        j = (i - 1) >> 1;
    }
    heap[i] = key;
}

template <class T>
void MaxBinaryHeap<T>::ShiftDown(int i) {
    int size = this->mSize;
    T* heap = this->mHeap;
    int j = (i << 1) + 1;
    T key = heap[i];
    while (j < size) {
        if (j + 1 < size && heap[j] < heap[j+1])
            j++;
        if (key < heap[j]) {
            heap[i] = heap[j];
            i = j;
            j = (i << 1) + 1;
        } else {
            break;
        }
    }
    heap[i] = key;
}

template <class T>
class MinBinaryHeap : public BinaryHeap<T> {
public:
    MinBinaryHeap(int capacity) : BinaryHeap<T>(capacity) { }
    MinBinaryHeap(int capacity, const T *array, int size);
    const T& Min() const { return this->mHeap[0]; }
protected:
    void ShiftUp(int i);
    void ShiftDown(int i);
};

template <class T>
MinBinaryHeap<T>::MinBinaryHeap(int capacity, const T *array, int size) :
	BinaryHeap<T>(capacity, array, size) {
	for (int i = size / 2 - 1; i >= 0; --i) {
		ShiftDown(i);
	}
}

template <class T>
void MinBinaryHeap<T>::ShiftUp(int i) {
    T *heap = this->mHeap;
    int j = (i - 1) >> 1;
    T key = heap[i];
    while(i > 0 && key < heap[j]) {
        heap[i] = heap[j];
        i = j;
        j = (i - 1) >> 1;
    }
    heap[i] = key;
}

template <class T>
void MinBinaryHeap<T>::ShiftDown(int i) {
    int size = this->mSize;
    T *heap = this->mHeap;
    int j = (i << 1) + 1;
    T key = heap[i];
    while (j < size) {
        if (j + 1 < size && heap[j+1] < heap[j])
            j++;
        if (heap[j] < key) {
            heap[i] = heap[j];
            i = j;
            j = (i << 1) + 1;
        } else {
            break;
        }
    }
    heap[i] = key;
}

} //~ namespace longan


#endif /* ALGORITHM_DATA_STRUCTURE_BINARY_HEAP_H */
