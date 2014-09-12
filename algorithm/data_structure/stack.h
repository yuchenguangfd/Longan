/*
 * stack.h
 * Created on: Sep 10, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_DATA_STRUCTURE_STACK_H
#define ALGORITHM_DATA_STRUCTURE_STACK_H

#include "common/error.h"
#include <memory>
#include <cassert>

namespace longan {

template <class T, class Alloc = std::allocator<T> >
class Stack {
public:
    explicit Stack(int capacity = DEFAULT_CAPACITY, Alloc* alloc = nullptr) {
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
        mData = mAlloc->allocate(mCapacity);
    }
    ~Stack() {
        for (int i = 0; i < mSize; ++i) {
            mAlloc->destroy(&mData[i]);
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
    void Push(const T& elem) {
        if (Full()) {
            Expand();
        }
        mAlloc->construct(&mData[mSize], elem);
        ++mSize;
    }
    T Pop() {
        if (Empty()) {
            throw RuntimeError("stack is empty.");
        }
        T ret = mData[--mSize];
        mAlloc->destroy(&mData[mSize]);
        return ret;
    }
    const T& Top() const {
        return mData[mSize - 1];
    }
    T& Top() {
        return mData[mSize - 1];
    }
private:
    bool Full() const {
        return mSize >= mCapacity;
    }
    void Expand() {
        assert(mSize == mCapacity);
        int newCapacity = mCapacity * 2;
        T* newData = mAlloc->allocate(newCapacity);
        for(int i = 0; i < mCapacity; ++i) {
            mAlloc->construct(&newData[i], mData[i]);
            mAlloc->destroy(&mData[i]);
        }
        mAlloc->deallocate(mData, mCapacity);
        mCapacity = newCapacity;
        mData = newData;
    }
private:
    static const int DEFAULT_CAPACITY = 16;
    Alloc* mAlloc;
    bool mIsOwnAlloc;
    T* mData;
    int mSize;
    int mCapacity;
};

} //~ namespace longan

#endif // ALGORITHM_DATA_STRUCTURE_STACK_H
