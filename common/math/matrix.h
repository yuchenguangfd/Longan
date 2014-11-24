/*
 * matrix.h
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_MATH_MATRIX_H
#define COMMON_MATH_MATRIX_H

#include <algorithm>
#include <memory>
#include <cassert>

namespace longan {

template <class T, class Alloc = std::allocator<T> >
class Matrix {
public:
    Matrix();
    Matrix(int rows, int cols, bool init = false, T initValue = T());
    template <class Iterator>
    Matrix(int rows, int cols, Iterator begin, Iterator end);
    Matrix(const Matrix<T, Alloc>& orig);
    Matrix(Matrix<T, Alloc>&& orig);
    ~Matrix();
    Matrix<T, Alloc>& operator= (const Matrix<T, Alloc>& rhs);
    Matrix<T, Alloc>& operator= (Matrix<T, Alloc>&& rhs);

    template <class T1, class Alloc1>
    friend bool operator== (const Matrix<T1, Alloc1>& lhs, const Matrix<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend Matrix<T1, Alloc1> operator* (const Matrix<T1, Alloc1>& lhs, const Matrix<T1, Alloc1>& rhs);

    int Rows() const {
        return mRows;
    }
    int Cols() const {
        return mCols;
    }
    T* Data() {
        return mData;
    }
    const T* Data() const {
        return mData;
    }
    T* operator[] (int row) {
        return mData + row * mCols;
    }
    const T* operator[] (int row) const {
        return mData + row * mCols;
    }
private:
    T *mData;
    int mRows;
    int mCols;
    int mSize;
};

template <class T, class Alloc>
Matrix<T, Alloc>::Matrix() :
    mData(nullptr),
    mRows(0),
    mCols(0),
    mSize(0) { }

template <class T, class Alloc>
Matrix<T, Alloc>::Matrix(int rows, int cols, bool init, T initValue) :
    mRows(rows),
    mCols(cols),
    mSize(rows * cols) {
    Alloc alloc;
    mData = alloc.allocate(mSize);
    if (init) {
        std::fill(mData, mData + mSize, initValue);
    }
}

template <class T, class Alloc>
template <class Iterator>
Matrix<T, Alloc>::Matrix(int rows, int cols, Iterator begin, Iterator end) :
    mRows(rows),
    mCols(cols),
    mSize(rows * cols) {
    assert(mSize == end - begin);
    Alloc alloc;
    mData = alloc.allocate(mSize);
    std::copy(begin, end, mData);
}

template <class T, class Alloc>
Matrix<T, Alloc>::Matrix(const Matrix<T, Alloc>& orig) :
    mRows(orig.mRows),
    mCols(orig.mCols),
    mSize(orig.mSize) {
    Alloc alloc;
    mData = alloc.allocate(mSize);
    std::copy(orig.mData, orig.mData + orig.mSize, mData);
}

template <class T, class Alloc>
Matrix<T, Alloc>::Matrix(Matrix<T, Alloc>&& orig):
    mRows(orig.mRows),
    mCols(orig.mCols),
    mSize(orig.mSize),
    mData(orig.mData) {
    orig.mRows = 0;
    orig.mCols = 0;
    orig.mSize = 0;
    orig.mData = nullptr;
}

template <class T, class Alloc>
Matrix<T, Alloc>::~Matrix() {
    Alloc alloc;
    alloc.deallocate(mData, mSize);
}

template <class T, class Alloc>
Matrix<T, Alloc>& Matrix<T, Alloc>::operator = (const Matrix<T, Alloc>& rhs) {
    if (this == &rhs) return *this;
    if (mSize != rhs.mSize) {
        Alloc alloc;
        alloc.deallocate(mData, mSize);
        mRows = rhs.mRows;
        mCols = rhs.mCols;
        mSize = rhs.mSize;
        mData = alloc.allocate(mSize);
    }
    std::copy(rhs.mData, rhs.mData + rhs.mSize, mData);
    return *this;
}

template <class T, class Alloc>
Matrix<T, Alloc>& Matrix<T, Alloc>::operator= (Matrix<T, Alloc>&& rhs) {
    if (this == &rhs) return *this;
    Alloc alloc;
    alloc.deallocate(mData, mSize);
    mRows = rhs.mRows;
    mCols = rhs.mCols;
    mSize = rhs.mSize;
    mData = rhs.mData;
    rhs.mRows = 0;
    rhs.mCols = 0;
    rhs.mSize = 0;
    rhs.mData = nullptr;
    return *this;
}

template <class T, class Alloc>
bool operator == (const Matrix<T, Alloc>& lhs, const Matrix<T, Alloc>& rhs) {
    if (lhs.mRows != rhs.mRows || lhs.mCols != rhs.mCols) return false;
    for (int i = 0; i < lhs.mSize; ++i) {
        if (lhs.mData[i] != rhs.mData[i]) return false;
    }
    return true;;
}

template <class T, class Alloc>
Matrix<T, Alloc> operator* (const Matrix<T, Alloc>& lhs, const Matrix<T, Alloc>& rhs) {
    assert(lhs.mCols == rhs.mRows);
    Matrix<T, Alloc> result(lhs.mRows, rhs.mCols);
    for(int i = 0; i < result.mRows; ++i) {
        for(int j = 0; j < result.mCols; ++j) {
            T sum = T();
            for(int k = 0; k < lhs.mCols; ++k) {
                sum += lhs[i][k] * rhs[k][j];
            }
            result[i][j] = sum;
        }
    }
    return std::move(result);
}

typedef Matrix<int> Matrix32I;
typedef Matrix<float> Matrix32F;
typedef Matrix<double> Matrix64F;

} //~ namespace longan

#endif /* COMMON_MATH_MATRIX_H */
