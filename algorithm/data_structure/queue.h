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
    explicit Queue(int capacity = DEFAULT_CAPACITY) {
        assert(capacity > 0);
        mFront = mRear = 0;
        mSize = 0;
        mCapacity= capacity;
        Alloc alloc;
        mData = alloc.allocate(mCapacity);
    }
    ~Queue() {
        Alloc alloc;
        for (int i = 0; i < mSize; ++i) {
            alloc.destroy(&mData[mFront]);
            ++mFront;
            if (mFront == mCapacity) mFront = 0;
        }
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
    T Front() const {
        return mData[mFront];
    }
    T Rear() const {
        return mData[mRear - 1];
    }
    void Enqueue(const T &elem) {
    	if (Full()) {
    		Expand();
    	}
    	Alloc alloc;
    	alloc.construct(&mData[mRear], elem);
    	++mRear;
		if(mRear == mCapacity) mRear = 0;
		++mSize;
    }
    T Dequeue() {
    	if (Empty()) {
            throw LonganRuntimeError("queue is empty.");
        }
        T ret = mData[mFront];
        Alloc alloc;
        alloc.destroy(&mData[mFront]);
        ++mFront;
        if(mFront == mCapacity) mFront = 0;
        --mSize;
        return ret;
    }
protected:
    bool Full() const { 
        return mSize >= mCapacity; 
    }
    void Expand() {
    	assert(mSize == mCapacity);
    	int newCapacity = 2 * mCapacity;
    	Alloc alloc;
    	T *newData = alloc.allocate(newCapacity);
    	for (int i = 0; i < mCapacity; ++i) {
    		alloc.construct(&newData[i], mData[mFront]);
    		alloc.destroy(&mData[mFront]);
    		++mFront;
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
};

} //~ namespace longan

#endif // ALGORITHM_DATA_STRUCTURE_QUEUE_H
