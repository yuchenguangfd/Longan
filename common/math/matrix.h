/*
 * matrix.h
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_MATH_MATRIX_H
#define COMMON_MATH_MATRIX_H

#include "vector.h"
#include <algorithm>
#include <memory>
#include <cassert>

namespace longan {

template <class T, class Alloc = std::allocator<T>>
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
    Matrix<T, Alloc>& operator+= (const Matrix<T, Alloc>& rhs);
    Matrix<T, Alloc>& operator-= (const Matrix<T, Alloc>& rhs);
    template <class T1, class Alloc1>
    friend bool operator== (const Matrix<T1, Alloc1>& lhs, const Matrix<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend Matrix<T1, Alloc1> operator+ (const Matrix<T1, Alloc1>& lhs, const Matrix<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend Matrix<T1, Alloc1> operator* (T1 lhs, const Matrix<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend Matrix<T1, Alloc1> operator* (const Matrix<T1, Alloc1>& lhs, const Matrix<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend Vector<T1, Alloc1> operator* (const Matrix<T1, Alloc1>& lhs, const Vector<T1, Alloc1>& rhs);

    template <class T1, class Alloc1>
    friend Matrix<T1, Alloc1> MultiplyElementWise(const Matrix<T1, Alloc1>& lhs, const Matrix<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend Matrix<T1, Alloc1> MultiplyElementWise(const Matrix<T1, Alloc1>& m1,
            const Matrix<T1, Alloc1>& m2, const Matrix<T1, Alloc1>& m3);
    template <class T1, class Alloc1>
    friend Matrix<T1, Alloc1> OutterProd(const Vector<T1, Alloc1>& lhs, const Vector<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend T1 NormF(const Matrix<T1, Alloc1>& mat);
    template <class T1, class Alloc1>
    friend T1 NormFSqr(const Matrix<T1, Alloc1>& mat);

    Matrix<T, Alloc> Transpose() const;
    int Rows() const { return mRows; }
    int Cols() const { return mCols; }
    int Size() const { return mSize; }
    T* Data() { return mData; }
    const T* Data() const { return mData; }
    T* operator[] (int row) { return mData + row * mCols; }
    const T* operator[] (int row) const { return mData + row * mCols; }
    bool IsSquare() const { return mRows == mCols; }
    bool IsEmpty() const { return mSize == 0; }
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
Matrix<T, Alloc>& Matrix<T, Alloc>::operator+= (const Matrix& rhs) {
    assert(this->mRows == rhs.mRows && this->mCols == rhs.mCols);
    for (int i = 0; i < mSize; ++i) {
        mData[i] += rhs.mData[i];
    }
    return *this;
}

template <class T, class Alloc>
Matrix<T, Alloc>& Matrix<T, Alloc>::operator-= (const Matrix<T, Alloc>& rhs) {
    assert(this->mRows == rhs.mRows && this->mCols == rhs.mCols);
    for (int i = 0; i < mSize; ++i) {
        mData[i] -= rhs.mData[i];
    }
    return *this;
}

template <class T, class Alloc>
bool operator== (const Matrix<T, Alloc>& lhs, const Matrix<T, Alloc>& rhs) {
    if (lhs.mRows != rhs.mRows || lhs.mCols != rhs.mCols) return false;
    for (int i = 0; i < lhs.mSize; ++i) {
        if (lhs.mData[i] != rhs.mData[i]) return false;
    }
    return true;
}

template <class T, class Alloc>
Matrix<T, Alloc> operator+ (const Matrix<T, Alloc>& lhs, const Matrix<T, Alloc>& rhs) {
    assert(lhs.mRows == rhs.mRows && lhs.mCols == rhs.mCols);
    Matrix<T, Alloc> result(lhs.mRows, lhs.mCols);
    for (int i = 0; i < result.mSize; ++i) {
        result.mData[i] = lhs.mData[i] + rhs.mData[i];
    }
    return std::move(result);
}

template <class T, class Alloc>
Matrix<T, Alloc> operator* (T lhs, const Matrix<T, Alloc>& rhs) {
    Matrix<T, Alloc> result(rhs.mRows, rhs.mCols);
    for (int i = 0; i < result.mSize; ++i) {
        result.mData[i] = lhs * rhs.mData[i];
    }
    return std::move(result);
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

template <class T, class Alloc>
Vector<T, Alloc> operator* (const Matrix<T, Alloc>& lhs, const Vector<T, Alloc>& rhs){
    assert(lhs.mCols == rhs.mSize);
    Vector<T, Alloc> result(lhs.mRows);
    for (int i = 0; i < lhs.mRows; ++i) {
        T sum = T();
        const T *lhsRow = lhs.mData + i * lhs.mCols;
        for (int k = 0; k < rhs.mSize; ++k) {
            sum += lhsRow[k] * rhs[k];
        }
        result[i] = sum;
    }
    return std::move(result);
}

template <class T, class Alloc>
Matrix<T, Alloc> MultiplyElementWise(const Matrix<T, Alloc>& lhs, const Matrix<T, Alloc>& rhs) {
    assert(lhs.mRows == rhs.mRows && lhs.mCols == rhs.mCols);
    Matrix<T, Alloc> result(lhs.mRows, lhs.mCols);
    for (int i = 0; i < result.mSize; ++i) {
        result.mData[i] = lhs.mData[i] * rhs.mData[i];
    }
    return std::move(result);
}

template <class T, class Alloc>
Matrix<T, Alloc> MultiplyElementWise(const Matrix<T, Alloc>& m1, const Matrix<T, Alloc>& m2,
        const Matrix<T, Alloc>& m3) {
    assert(m1.mRows == m2.mRows && m1.mCols == m2.mCols);
    assert(m1.mRows == m3.mRows && m1.mCols == m3.mCols);
    Matrix<T, Alloc> result(m1.mRows, m1.mCols);
    for (int i = 0; i < result.mSize; ++i) {
        result.mData[i] = m1.mData[i] * m2.mData[i] * m3.mData[i];
    }
    return std::move(result);
}

template <class T, class Alloc>
Matrix<T, Alloc> OutterProd(const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs) {
    Matrix<T, Alloc> result(lhs.mSize, rhs.mSize);
    for (int i = 0; i < result.mRows; ++i) {
        for (int j = 0; j < result.mCols; ++j) {
            result.mData[i * result.mCols + j] = lhs.mData[i]*rhs.mData[j];
        }
    }
    return std::move(result);
}

template <class T, class Alloc>
T NormFSqr(const Matrix<T, Alloc>& mat) {
    T sum = T();
    for (int i = 0; i < mat.mSize; ++i) {
        sum += Math::Sqr(mat.mData[i]);
    }
    return sum;
}

template <class T, class Alloc>
T NormF(const Matrix<T, Alloc>& mat) {
    return Math::Sqrt(NormFSqr<T, Alloc>(mat));
}

template <class T, class Alloc>
Matrix<T, Alloc> Matrix<T, Alloc>::Transpose() const {
    Matrix<T, Alloc> result(mCols, mRows);
    for(int i = 0; i < mCols; ++i) {
        for(int j = 0; j < mRows; ++j) {
            result.mData[i * mRows + j] = mData[j * mCols + i];
        }
    }
    return std::move(result);
}

template <class T, class Alloc>
std::ostream& operator<< (std::ostream& os, const Matrix<T, Alloc>& mat) {
    for (int i = 0; i < mat.Rows(); ++i) {
        os << mat[i][0];
        for (int j = 1; j < mat.Cols(); ++j) {
            os << "," << mat[i][j];
        }
        os << "\n";
    }
    return os;
}

typedef Matrix<int> Matrix32I;
typedef Matrix<float> Matrix32F;
typedef Matrix<double> Matrix64F;

} //~ namespace longan

#endif /* COMMON_MATH_MATRIX_H */
