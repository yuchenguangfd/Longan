/*
 * array.h
 * Created on: May 12, 2015
 * Author: chenguangyu
 */

#ifndef COMMON_LANG_ARRAY_H
#define COMMON_LANG_ARRAY_H

#include "defines.h"
#include <memory>

namespace longan {

template <class T, class Alloc = std::allocator<T>>
class Array1D {
public:
    Array1D(int size, bool init = false, T initValue = T()) :
        mSize(size) {
        Alloc alloc;
        mData = alloc.allocate(mSize);
        if (init) {
            for (int i = 0; i < mSize; ++i) {
                alloc.construct(&mData[i], initValue);
            }
        }
    }
    ~Array1D()  {
        Alloc alloc;
        for (int i = 0; i < mSize; ++i) {
            alloc.destroy(&mData[i]);
        }
        alloc.deallocate(mData, mSize);
    }
    int Size() const { return mSize; }
    const T& operator[] (int index) const { return mData[index]; }
    T& operator[] (int index) { return mData[index]; }
    operator T* ()  { return mData; }
    T* begin() { return mData; }
    const T* begin() const { return mData; }
    T* end() { return mData + mSize; }
    const T* end() const { return mData + mSize; }
private:
    int mSize;
    T *mData;
    DISALLOW_COPY_AND_ASSIGN(Array1D);
};

template <class T, class Alloc = std::allocator<T>>
class Array2D {
public:
    Array2D(int r, int c, bool init = false, T initValue = T()) :
        mRows(r), mCols(c), mSize(r * c) {
        Alloc alloc;
        mData = alloc.allocate(mSize);
        if (init) {
            for (int i = 0; i < mSize; ++i) {
                alloc.construct(&mData[i], initValue);
            }
        }
    }
    ~Array2D() {
        Alloc alloc;
        for (int i = 0; i < mSize; ++i) {
            alloc.destroy(&mData[i]);
        }
        alloc.deallocate(mData, mSize);
    }
    int Rows() const { return mRows; }
    int Cols() const { return mCols; }
    const T* operator[] (int i) const {
        return mData + i * mCols;
    }
    T* operator[] (int i) {
        return mData + i * mCols;
    }
private:
    int mRows, mCols;
    int mSize;
    T* mData;
    DISALLOW_COPY_AND_ASSIGN(Array2D);
};

typedef Array1D<char> CharArray;
typedef Array1D<int> IntArray;
typedef Array1D<double> DoubleArray;
typedef Array1D<bool> BoolArray;

} //~ namespace longan

#endif // COMMON_LANG_ARRAY_H
