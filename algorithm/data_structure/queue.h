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
    Queue(int capacity = DEFAULT_CAPACITY, Alloc* alloc = nullptr) {
        assert(capacity > 0);
        mFront = mRear = 0;
        mSize = 0;
        mCapacity= capacity;
        if (alloc == nullptr) {
            mAlloc = new Alloc();
            mIsOwnAlloc = true;
        } else {
            mAlloc = alloc;
            mIsOwnAlloc = false;
        }
        mData = mAlloc->allocate(mCapacity);
    }
    ~Queue() {
        for (int i = 0; i < mSize; ++i) {
            mAlloc->destroy(&mData[mFront]);
            ++mFront;
            if (mFront == mCapacity) mFront = 0;
        }
        mAlloc->deallocate(mData, mCapacity);
        if (mIsOwnAlloc) {
            delete mAlloc;
        }
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
    	mAlloc->construct(&mData[mRear], elem);
    	++mRear;
		if(mRear == mCapacity) mRear = 0;
		++mSize;
    }
    T Dequeue() {
    	if (Empty()) {
            throw RuntimeError("queue is empty.");
        }
        T ret = mData[mFront];
        mAlloc->destroy(&mData[mFront]);
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
    	T *newData = mAlloc->allocate(newCapacity);
    	for (int i = 0; i < mCapacity; ++i) {
    		mAlloc->construct(&newData[i], mData[mFront]);
    		mAlloc->destroy(&mData[mFront]);
    		++mFront;
            if(mFront == mCapacity) mFront = 0;
    	}
    	mAlloc->deallocate(mData, mCapacity);
    	mData = newData;
    	mFront = 0;
    	mRear = mCapacity;
    	mSize = mCapacity;
    	mCapacity = newCapacity;
    }
protected:
    static const int DEFAULT_CAPACITY = 16;
    Alloc* mAlloc;
    bool mIsOwnAlloc;
    T* mData;
    int mCapacity;
    int mFront;
    int mRear;
    int mSize;
};

} //~ namespace longan

#endif // ALGORITHM_DATA_STRUCTURE_QUEUE_H
