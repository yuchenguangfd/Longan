/*
 * binary_heap.h
 * Created on: Aug 27, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_DATA_STRUCTURE_BINARY_HEAP_H
#define ALGORITHM_DATA_STRUCTURE_BINARY_HEAP_H

#include "common/error.h"
#include <memory>
#include <cassert>

namespace longan {

template <class T, class Alloc = std::allocator<T> >
class BinaryHeap {
public:
    BinaryHeap(int capacity = DEFAULT_CAPACITY);
    template <class Iterator>
    BinaryHeap(Iterator begin, int size, int capacity = DEFAULT_CAPACITY);
    BinaryHeap(const BinaryHeap<T, Alloc>& orig) = delete;
    BinaryHeap(BinaryHeap<T, Alloc>&& orig);
    virtual ~BinaryHeap();
    bool Empty() const {
        return mSize == 0;
    }
    void Add(const T& val);
    T Extract();
    const T& Top() const {
        return mHeap[0];
    }
    int Size() const {
        return mSize;
    }
    int Capacity() const {
        return mCapacity;
    }
    void Clear() {
        mSize = 0;
    }
    void Modify(int pos, const T &val);
    T* Begin() {
        return mHeap;
    }
    const T* Begin() const {
        return mHeap;
    }
    T* End() {
        return mHeap + mSize;
    }
    const T* End() const {
        return mHeap + mSize;
    }
private:
    bool Full() const {
        return mSize >= mCapacity;
    }
    void Expand();
protected:
    virtual void ShiftUp(int pos) = 0;
    virtual void ShiftDown(int pos) = 0;
protected:
    static const int DEFAULT_CAPACITY = 16;
    int mCapacity;
    int mSize;
    T* mHeap;
};

template <class T, class Alloc>
BinaryHeap<T, Alloc>::BinaryHeap(int capacity) :
    mSize(0),
    mCapacity(capacity) {
    Alloc alloc;
    mHeap = alloc.allocate(mCapacity);
}

template <class T, class Alloc>
template <class Iterator>
BinaryHeap<T, Alloc>::BinaryHeap(Iterator begin, int size, int capacity) {
    mCapacity = (size > capacity)? size : capacity;
    mSize = size;
    Alloc alloc;
    mHeap = alloc.allocate(mCapacity);
    for (int i = 0; i < size; ++i) {
      alloc.construct(&mHeap[i], *begin++);
    }
}

template <class T, class Alloc>
BinaryHeap<T, Alloc>::BinaryHeap(BinaryHeap<T, Alloc>&& orig) :
    mCapacity(orig.mCapacity),
    mSize(orig.mSize),
    mHeap(orig.mHeap) {
    orig.mCapacity = 0;
    orig.mSize = 0;
    orig.mHeap = 0;
}

template <class T, class Alloc>
BinaryHeap<T, Alloc>::~BinaryHeap() {
    Alloc alloc;
    for (int i = 0; i < mSize; ++i) {
        alloc.destroy(&mHeap[i]);
    }
    alloc.deallocate(mHeap, mCapacity);
}

template <class T, class Alloc>
void BinaryHeap<T, Alloc>::Add(const T &val) {
    if (Full()) {
        Expand();
    }
    Alloc alloc;
    alloc.construct(&mHeap[mSize], val);
    ShiftUp(mSize);
    ++mSize;
}

template <class T, class Alloc>
T BinaryHeap<T, Alloc>::Extract() {
    if (Empty()) {
        throw LonganRuntimeError("binary heap is empty.");
    }
    T top = mHeap[0];
    Alloc alloc;
    alloc.destroy(&mHeap[0]);
    --mSize;
    alloc.construct(&mHeap[0], mHeap[mSize]);
    alloc.destroy(&mHeap[mSize]);
    ShiftDown(0);
    return top;
}

template <class T, class Alloc>
void BinaryHeap<T, Alloc>::Modify(int pos, const T &val) {
    Alloc alloc;
    alloc.destroy(&mHeap[pos]);
    alloc.construct(&mHeap[pos], val);
    ShiftUp(pos);
    ShiftDown(pos);
}

template <class T, class Alloc>
void BinaryHeap<T, Alloc>::Expand() {
    assert(mSize == mCapacity);
    int newCapacity = 2 * mCapacity;
    Alloc alloc;
    T *newHeap = alloc.allocate(newCapacity);
    for (int i = 0; i < mCapacity; ++i) {
        alloc.construct(&newHeap[i], mHeap[i]);
        alloc.destroy(&mHeap[i]);
    }
    alloc.deallocate(mHeap, mCapacity);
    mHeap = newHeap;
    mSize = mCapacity;
    mCapacity = newCapacity;
}

template <class T, class Alloc = std::allocator<T> >
class MaxBinaryHeap : public BinaryHeap<T, Alloc> {
public:
    MaxBinaryHeap(int capacity = BinaryHeap<T, Alloc>::DEFAULT_CAPACITY) :
        BinaryHeap<T, Alloc>(capacity) { }
    template <class Iterator>
    MaxBinaryHeap(Iterator begin, int size, int capacity = BinaryHeap<T, Alloc>::DEFAULT_CAPACITY);
    const T& Max() const  {
        return this->Top();
    }
protected:
    void ShiftUp(int pos);
    void ShiftDown(int pos);
};

template <class T, class Alloc>
template <class Iterator>
MaxBinaryHeap<T, Alloc>::MaxBinaryHeap(Iterator begin, int size, int capacity) :
    BinaryHeap<T, Alloc>(begin, size, capacity) {
    for (int i = size / 2 - 1; i >= 0; --i) {
        ShiftDown(i);
    }
}

template <class T, class Alloc>
void MaxBinaryHeap<T, Alloc>::ShiftUp(int pos) {
    T *heap = this->mHeap;
    int i = pos, j = (pos - 1)>>1;
    T key = heap[i];
    while(i > 0 && heap[j] < key) {
        heap[i] = heap[j];
        i = j;
        j = (i - 1) >> 1;
    }
    heap[i] = key;
}

template <class T, class Alloc>
void MaxBinaryHeap<T, Alloc>::ShiftDown(int pos) {
    int size = this->mSize;
    T* heap = this->mHeap;
    int i = pos, j = (pos << 1) + 1;
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

template <class T, class Alloc = std::allocator<T> >
class MinBinaryHeap : public BinaryHeap<T, Alloc> {
public:
    MinBinaryHeap(int capacity = BinaryHeap<T, Alloc>::DEFAULT_CAPACITY) :
        BinaryHeap<T, Alloc>(capacity) { }
    template <class Iterator>
    MinBinaryHeap(Iterator begin, int size, int capacity = BinaryHeap<T, Alloc>::DEFAULT_CAPACITY);
    const T& Min() const { return this->Top(); }
protected:
    void ShiftUp(int pos);
    void ShiftDown(int pos);
};

template <class T, class Alloc>
template <class Iterator>
MinBinaryHeap<T, Alloc>::MinBinaryHeap(Iterator begin, int size, int capacity) :
	BinaryHeap<T, Alloc>(begin, size, capacity) {
	for (int i = size / 2 - 1; i >= 0; --i) {
		ShiftDown(i);
	}
}

template <class T, class Alloc>
void MinBinaryHeap<T, Alloc>::ShiftUp(int pos) {
    T *heap = this->mHeap;
    int i = pos, j = (pos - 1) >> 1;
    T key = heap[i];
    while(i > 0 && key < heap[j]) {
        heap[i] = heap[j];
        i = j;
        j = (i - 1) >> 1;
    }
    heap[i] = key;
}

template <class T, class Alloc>
void MinBinaryHeap<T, Alloc>::ShiftDown(int pos) {
    int size = this->mSize;
    T *heap = this->mHeap;
    int i = pos, j = (pos << 1) + 1;
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
