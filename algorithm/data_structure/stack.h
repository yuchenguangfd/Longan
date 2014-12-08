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
    explicit Stack(int capacity = DEFAULT_CAPACITY) {
        assert(capacity > 0);
        mSize = 0;
        mCapacity = capacity;
        Alloc alloc;
        mData = alloc.allocate(mCapacity);
    }
    ~Stack() {
        Alloc alloc;
        for (int i = 0; i < mSize; ++i) {
            alloc.destroy(&mData[i]);
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
    void Push(const T& elem) {
        if (Full()) {
            Expand();
        }
        Alloc alloc;
        alloc.construct(&mData[mSize], elem);
        ++mSize;
    }
    T Pop() {
        if (Empty()) {
            throw LonganRuntimeError("stack is empty.");
        }
        T ret = mData[--mSize];
        Alloc alloc;
        alloc.destroy(&mData[mSize]);
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
        Alloc alloc;
        T* newData = alloc.allocate(newCapacity);
        for(int i = 0; i < mCapacity; ++i) {
            alloc.construct(&newData[i], std::move(mData[i]));
            alloc.destroy(&mData[i]);
        }
        alloc.deallocate(mData, mCapacity);
        mCapacity = newCapacity;
        mData = newData;
    }
private:
    static const int DEFAULT_CAPACITY = 16;
    T* mData;
    int mSize;
    int mCapacity;
};

} //~ namespace longan

#endif // ALGORITHM_DATA_STRUCTURE_STACK_H
