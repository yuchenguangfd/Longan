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
    BinaryHeap(int capacity = DEFAULT_CAPACITY, Alloc *alloc = nullptr);
    template <class Iterator>
    BinaryHeap(Iterator begin, int size, int capacity = DEFAULT_CAPACITY, Alloc *alloc = nullptr);
    virtual ~BinaryHeap();
    bool Empty() const { return mSize == 0; }
    void Add(const T& val);
    T Extract();
    const T& Top() const { return mHeap[0]; }
    int Size() const { return mSize; }
    int Capacity() const { return mCapacity; }
    void Clear() { mSize = 0; }
    void Modify(int pos, const T &val);
    T* Begin() { return mHeap; }
    const T* Begin() const { return mHeap; }
    T* End() { return mHeap + mSize; }
    const T* End() const { return mHeap + mSize; }
private:
    bool Full() const { return mSize >= mCapacity; }
    void Expand();
protected:
    virtual void ShiftUp(int pos) = 0;
    virtual void ShiftDown(int pos) = 0;
protected:
    static const int DEFAULT_CAPACITY = 16;
    Alloc* mAlloc;
    bool mIsOwnAlloc;
    int mCapacity;
    int mSize;
    T* mHeap;
};

template <class T, class Alloc>
BinaryHeap<T, Alloc>::BinaryHeap(int capacity, Alloc *alloc) {
    assert(capacity > 0);
    mSize = 0;
    mCapacity = capacity;
    if (alloc == nullptr) {
        mAlloc = new Alloc();
        mIsOwnAlloc = true;
    } else {
        mAlloc = alloc;
        mIsOwnAlloc = false;
    }
    mHeap = mAlloc->allocate(mCapacity);
}

template <class T, class Alloc>
template <class Iterator>
BinaryHeap<T, Alloc>::BinaryHeap(Iterator begin, int size, int capacity, Alloc *alloc) {
    assert(capacity > 0);
    mCapacity = (size > capacity)? size : capacity;
    mSize = size;
    if (alloc == nullptr) {
        mAlloc = new Alloc();
        mIsOwnAlloc = true;
    } else {
        mAlloc = alloc;
        mIsOwnAlloc = false;
    }
    mHeap = mAlloc->allocate(mCapacity);
    for (int i = 0; i < size; ++i) {
      mAlloc->construct(&mHeap[i], *begin++);
    }
}

template <class T, class Alloc>
BinaryHeap<T, Alloc>::~BinaryHeap() {
  for (int i = 0; i < mSize; ++i) {
      mAlloc->destroy(&mHeap[i]);
  }
  mAlloc->deallocate(mHeap, mCapacity);
  if (mIsOwnAlloc) {
      delete mAlloc;
  }
}

template <class T, class Alloc>
void BinaryHeap<T, Alloc>::Add(const T &val) {
    if (Full()) {
        Expand();
    }
    mAlloc->construct(&mHeap[mSize], val);
    ShiftUp(mSize);
    ++mSize;
}

template <class T, class Alloc>
T BinaryHeap<T, Alloc>::Extract() {
    if (Empty()) {
        throw RuntimeError("binary heap is empty.");
    }
    T top = mHeap[0];
    mAlloc->destroy(&mHeap[0]);
    --mSize;
    mAlloc->construct(&mHeap[0], mHeap[mSize]);
    mAlloc->destroy(&mHeap[mSize]);
    ShiftDown(0);
    return top;
}

template <class T, class Alloc>
void BinaryHeap<T, Alloc>::Modify(int pos, const T &val) {
    mAlloc->destroy(&mHeap[pos]);
    mAlloc->construct(&mHeap[pos], val);
    ShiftUp(pos);
    ShiftDown(pos);
}

template <class T, class Alloc>
void BinaryHeap<T, Alloc>::Expand() {
    assert(mSize == mCapacity);
    int newCapacity = 2 * mCapacity;
    T *newHeap = mAlloc->allocate(newCapacity);
    for (int i = 0; i < mCapacity; ++i) {
        mAlloc->construct(&newHeap[i], mHeap[i]);
        mAlloc->destroy(&mHeap[i]);
    }
    mAlloc->deallocate(mHeap, mCapacity);
    mHeap = newHeap;
    mSize = mCapacity;
    mCapacity = newCapacity;
}

template <class T, class Alloc = std::allocator<T> >
class MaxBinaryHeap : public BinaryHeap<T, Alloc> {
public:
    MaxBinaryHeap(int capacity = BinaryHeap<T, Alloc>::DEFAULT_CAPACITY, Alloc *alloc = nullptr) :
        BinaryHeap<T, Alloc>(capacity, alloc) { }
    template <class Iterator>
    MaxBinaryHeap(Iterator begin, int size, int capacity = BinaryHeap<T, Alloc>::DEFAULT_CAPACITY, Alloc *alloc = nullptr);
    const T& Max() const  { return this->Top(); }
protected:
    void ShiftUp(int pos);
    void ShiftDown(int pos);
};

template <class T, class Alloc>
template <class Iterator>
MaxBinaryHeap<T, Alloc>::MaxBinaryHeap(Iterator begin, int size, int capacity, Alloc *alloc) :
    BinaryHeap<T, Alloc>(begin, size, capacity, alloc) {
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
    MinBinaryHeap(int capacity = BinaryHeap<T, Alloc>::DEFAULT_CAPACITY, Alloc *alloc = nullptr) :
        BinaryHeap<T, Alloc>(capacity, alloc) { }
    template <class Iterator>
    MinBinaryHeap(Iterator begin, int size, int capacity = BinaryHeap<T, Alloc>::DEFAULT_CAPACITY, Alloc *alloc = nullptr);
    const T& Min() const { return this->Top(); }
protected:
    void ShiftUp(int pos);
    void ShiftDown(int pos);
};

template <class T, class Alloc>
template <class Iterator>
MinBinaryHeap<T, Alloc>::MinBinaryHeap(Iterator begin, int size, int capacity, Alloc *alloc) :
	BinaryHeap<T, Alloc>(begin, size, capacity, alloc) {
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
