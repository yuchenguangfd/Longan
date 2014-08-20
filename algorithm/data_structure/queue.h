/*
 * queue.h
 * Created on: Aug 20, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_DATA_STRUCTURE_QUEUE_H
#define ALGORITHM_DATA_STRUCTURE_QUEUE_H

#include "common/error.h"
#include <memory>
#include <cassert>

namespace longan {

template <class T, class Alloc = std::allocator<T> >
class Queue {
public:
    Queue(int capacity = DEFAULT_CAPACITY) :
    	mCapacity(capacity), mFront(0), mRear(0), mSize(0) {
        mData = alloc.allocate(capacity);
    }
    ~Queue() {
        alloc.deallocate(mData, mCapacity);
    }
    bool Empty() const { 
        return mSize == 0; 
    }
    int Size() const { 
        return mSize;
    }
    int Capacity() const { 
        return mCapacity; 
    }
    void Enqueue(const T &elem) {
    	if (Full()) {
    		Expand();
    	}
    	mData[mRear++] = elem;
		if(mRear == mCapacity) mRear = 0;
		++mSize;
    }
    T Dequeue() {
    	if (Empty()) {
            throw RuntimeError("queue is empty.");
        }
        const T& elem = mData[mFront++];
        if(mFront == mCapacity) mFront = 0;
        --mSize;
        return elem;
    }
protected:
    bool Full() const { 
        return mSize >= mCapacity; 
    }
    void Expand() {
    	assert(mSize == mCapacity);
    	int newCapacity = 2 * mCapacity;
    	T *newData = alloc.allocate(newCapacity);
    	for (int i = 0; i < mCapacity; ++i) {
    		newData[i] = mData[mFront++];
            if(mFront == mCapacity) mFront = 0;
    	}
    	alloc.deallocate(mData, mCapacity);
    	mData = newData;
    	mFront = 0;
    	mRear = mCapacity;
    	mSize = mCapacity;
    	mCapacity = newCapacity;
    }
protected:
    static const int DEFAULT_CAPACITY = 16;
    T* mData;
    int mCapacity;
    int mFront;
    int mRear;
    int mSize;
    Alloc alloc;
};

} //~ namespace longan

#endif // ALGORITHM_DATA_STRUCTURE_QUEUE_H
