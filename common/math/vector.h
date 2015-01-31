/*
 * vector.h
 * Created on: Sep 4, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_MATH_VECTOR_H
#define COMMON_MATH_VECTOR_H

#include "math.h"
#include "common/util/random.h"
#include <algorithm>
#include <memory>
#include <iostream>
#include <cassert>

namespace longan {

template <class T, class Alloc>
class Matrix;

template <class T, class Alloc = std::allocator<T>>
class Vector {
public:
    Vector();
	Vector(int size, bool init = false, T initValue = T());
    template <class Iterator>
	Vector(Iterator begin, Iterator end);
	Vector(const Vector<T, Alloc>& orig);
    Vector(Vector<T, Alloc>&& orig);
	~Vector();
    Vector<T, Alloc>& operator= (const Vector<T, Alloc>& rhs);
    Vector<T, Alloc>& operator= (Vector<T, Alloc>&& rhs);
    Vector<T, Alloc>& operator+= (const Vector<T, Alloc>& rhs);
    Vector<T, Alloc>& operator-= (const Vector<T, Alloc>& rhs);
    Vector<T, Alloc>& operator*= (T rhs);
    Vector<T, Alloc>& operator/= (T rhs);

    template <class T1, class Alloc1>
    friend bool operator== (const Vector<T1, Alloc1>& lhs, const Vector<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend bool operator!= (const Vector<T1, Alloc1>& lhs, const Vector<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend Vector<T1, Alloc1> operator+ (const Vector<T1, Alloc1>& lhs, const Vector<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend Vector<T1, Alloc1> operator- (const Vector<T1, Alloc1>& lhs, const Vector<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend Vector<T1, Alloc1> operator- (T1 lhs, const Vector<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend Vector<T1, Alloc1> operator* (T1 lhs, const Vector<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend Vector<T1, Alloc1> operator* (const Vector<T1, Alloc1>& lhs, T1 rhs);
    template <class T1, class Alloc1>
    friend Vector<T1, Alloc1> operator* (const Matrix<T1, Alloc1>& lhs, const Vector<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend Vector<T1, Alloc1> operator/ (const Vector<T1, Alloc1>& lhs, T1 rhs);
    template <class T1, class Alloc1>
    friend Vector<T1, Alloc1> MultiplyElementWise(const Vector<T1, Alloc1>& v1, const Vector<T1, Alloc1>& v2);
    template <class T1, class Alloc1>
    friend Vector<T1, Alloc1> MultiplyElementWise(const Vector<T1, Alloc1>& v1,
        const Vector<T1, Alloc1>& v2, const Vector<T1, Alloc1>& v3);
    template <class T1, class Alloc1>
    friend T1 InnerProd(const Vector<T1, Alloc1>& lhs, const Vector<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend Matrix<T1, Alloc1> OutterProd(const Vector<T1, Alloc1>& lhs, const Vector<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend T1 NormL1(const Vector<T1, Alloc1>& vec);
    template <class T1, class Alloc1>
    friend T1 NormL2Sqr(const Vector<T1, Alloc1>& vec);
    template <class T1, class Alloc1>
    friend T1 NormL2(const Vector<T1, Alloc1>& vec);

    int Size() const { return mSize; }
	T* Data() { return mData; }
	const T* Data() const { return mData; }
    T* Begin() { return mData; }
    const T* Begin() const { return mData; }
    T* End() { return mData + mSize; }
    const T* End() const { return mData + mSize; }
    T& operator[] (int index) { return mData[index]; }
    const T& operator[] (int index) const { return mData[index]; }
public:
    static Vector<T, Alloc> Zeros(int size);
    static Vector<T, Alloc> Rand(int size);
    static Vector<T, Alloc> Rand(int size, T low, T high);
    static Vector<T, Alloc> Randn(int size);
protected:
    T *mData;
    int mSize;
};

template <class T, class Alloc>
Vector<T, Alloc>::Vector() :
    mData(nullptr),
    mSize(0) { }

template <class T, class Alloc>
Vector<T, Alloc>::Vector(int size, bool init, T initValue) :
	mSize(size) {
    Alloc alloc;
    mData = alloc.allocate(mSize);
    if (init) {
        std::fill(mData, mData + mSize, initValue);
    }
}

template <class T, class Alloc>
template <class Iterator>
Vector<T, Alloc>::Vector(Iterator begin, Iterator end) :
    mSize(end - begin) {
    assert(mSize >= 0);
    Alloc alloc;
    mData = alloc.allocate(mSize);
    std::copy(begin, end, mData);
}

template <class T, class Alloc>
Vector<T, Alloc>::Vector(const Vector<T, Alloc>& orig) :
    mSize(orig.mSize) {
    Alloc alloc;
    mData = alloc.allocate(mSize);
    std::copy(orig.mData, orig.mData + orig.mSize, mData);
}

template <class T, class Alloc>
Vector<T, Alloc>::Vector(Vector<T, Alloc>&& orig) :
    mSize(orig.mSize),
    mData(orig.mData) {
    orig.mSize = 0;
    orig.mData = nullptr;
}

template <class T, class Alloc>
Vector<T, Alloc>::~Vector() {
    Alloc alloc;
    alloc.deallocate(mData, mSize);
}

template <class T, class Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator= (const Vector<T, Alloc>& rhs) {
    if (this == &rhs) return *this;
    if (mSize != rhs.mSize) {
        Alloc alloc;
        alloc.deallocate(mData, mSize);
        mSize = rhs.mSize;
        mData = alloc.allocate(mSize);
    }
    std::copy(rhs.mData, rhs.mData + rhs.mSize, mData);
    return *this;
}

template <class T, class Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator= (Vector<T, Alloc>&& rhs) {
    if (this == &rhs) return *this;
    Alloc alloc;
    alloc.deallocate(mData, mSize);
    mSize = rhs.mSize;
    mData = rhs.mData;
    rhs.mSize = 0;
    rhs.mData = nullptr;
    return *this;
}

template <class T, class Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator+= (const Vector<T, Alloc>& rhs) {
    assert(this->mSize == rhs.mSize);
    for (int i = 0; i < mSize; ++i) {
        mData[i] += rhs.mData[i];
    }
    return *this;
}

template <class T, class Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator-= (const Vector<T, Alloc>& rhs) {
    assert(this->mSize == rhs.mSize);
    for (int i = 0; i < mSize; ++i) {
        mData[i] -= rhs.mData[i];
    }
    return *this;
}

template <class T, class Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator*= (T rhs) {
    for (int i = 0; i < mSize; ++i) {
        mData[i] *= rhs;
    }
    return *this;
}

template <class T, class Alloc>
Vector<T, Alloc>& Vector<T, Alloc>::operator/= (T rhs) {
    for (int i = 0; i < mSize; ++i) {
        mData[i] /= rhs;
    }
    return *this;
}

template <class T, class Alloc>
bool operator== (const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs) {
    if (lhs.mSize != rhs.mSize) return false;
    for (int i = 0; i < lhs.mSize; ++i) {
        if (lhs.mData[i] != rhs.mData[i])
            return false;
    }
    return true;
}

template <class T, class Alloc>
inline bool operator != (const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs) {
    return !(lhs == rhs);
}

template <class T, class Alloc>
Vector<T, Alloc> operator+ (const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs) {
    assert(lhs.Size() == rhs.Size());
    Vector<T, Alloc> result(lhs.mSize);
    for (int i = 0; i < lhs.mSize; ++i) {
        result.mData[i] = lhs.mData[i] + rhs.mData[i];
    }
    return std::move(result);
}

template <class T, class Alloc>
Vector<T, Alloc> operator- (const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs) {
    assert(lhs.mSize == rhs.mSize);
    Vector<T, Alloc> result(lhs.mSize);
    for (int i = 0; i < lhs.mSize; ++i) {
        result.mData[i] = lhs.mData[i] - rhs.mData[i];
    }
    return std::move(result);
}

template <class T, class Alloc>
Vector<T, Alloc> operator- (T lhs, const Vector<T, Alloc>& rhs) {
    Vector<T, Alloc> result(rhs.mSize);
    for (int i = 0; i < rhs.mSize; ++i) {
        result.mData[i] = lhs - rhs.mData[i];
    }
    return std::move(result);
}

template <class T, class Alloc>
Vector<T, Alloc> operator* (T lhs, const Vector<T, Alloc>& rhs) {
    Vector<T, Alloc> result(rhs.mSize);
    for (int i = 0; i < rhs.mSize; ++i) {
        result.mData[i] = lhs * rhs.mData[i];
    }
    return std::move(result);
}

template <class T, class Alloc>
Vector<T, Alloc> operator* (const Vector<T, Alloc>& lhs, T rhs) {
    Vector<T, Alloc> result(lhs.mSize);
    for (int i = 0; i < lhs.mSize; ++i) {
        result.mData[i] = lhs * rhs.mData[i];
    }
    return std::move(result);
}

template <class T, class Alloc>
Vector<T, Alloc> operator/ (const Vector<T, Alloc>& lhs, T rhs) {
    Vector<T, Alloc> result(lhs.mSize);
    for (int i = 0; i < lhs.mSize; ++i) {
        result.mData[i] = lhs.mData[i] / rhs;
    }
    return std::move(result);
}

template <class T, class Alloc>
Vector<T, Alloc> MultiplyElementWise(const Vector<T, Alloc>& v1, const Vector<T, Alloc>& v2) {
    assert(v1.mSize == v2.mSize);
    Vector<T, Alloc> result(v1.mSize);
    for (int i = 0; i < result.mSize; ++i) {
        result.mData[i] = v1.mData[i] * v2.mData[i];
    }
    return std::move(result);
}

template <class T, class Alloc>
Vector<T, Alloc> MultiplyElementWise(const Vector<T, Alloc>& v1,
    const Vector<T, Alloc>& v2, const Vector<T, Alloc>& v3) {
    assert(v1.mSize == v2.mSize && v1.mSize == v3.mSize);
    Vector<T, Alloc> result(v1.mSize);
    for (int i = 0; i < result.mSize; ++i) {
        result.mData[i] = v1.mData[i] * v2.mData[i] * v3.mData[i];
    }
    return std::move(result);
}

template <class T, class Alloc>
T InnerProd(const Vector<T, Alloc>& lhs, const Vector<T, Alloc>& rhs) {
    assert(lhs.mSize == rhs.mSize);
    T dotProd = T();
    for (int i = 0; i < rhs.mSize; ++i) {
        dotProd += lhs.mData[i] * rhs.mData[i];
    }
    return dotProd;
}

template <class T, class Alloc>
Vector<T, Alloc> Vector<T, Alloc>::Zeros(int size) {
    Vector<T, Alloc> vec(size, true, T());
    return std::move(vec);
}

template <class T, class Alloc>
Vector<T, Alloc> Vector<T, Alloc>::Rand(int size) {
    Vector<T, Alloc> vec(size, false);
    Random& rnd = Random::Instance();
    for (int i = 0; i < size; ++i) {
        vec.mData[i] = rnd.NextDouble();
    }
    return std::move(vec);
}

template <class T, class Alloc>
Vector<T, Alloc> Vector<T, Alloc>::Rand(int size, T low, T high) {
    Vector<T, Alloc> vec(size, false);
    Random& rnd = Random::Instance();
    for (int i = 0; i < size; ++i) {
        vec.mData[i] = rnd.Uniform(low, high);
    }
    return std::move(vec);
}

template <class T, class Alloc>
Vector<T, Alloc> Vector<T, Alloc>::Randn(int size) {
    Vector<T, Alloc> vec(size, false);
    Random& rnd = Random::Instance();
    for (int i = 0; i < size; ++i) {
        vec.mData[i] = rnd.Gauss();
    }
    return vec;
}

template <class T, class Alloc>
T NormL1(const Vector<T, Alloc>& vec) {
    T sum = T();
    for (int i = 0; i < vec.mSize; ++i) {
        sum += Math::Abs(vec.mData[i]);
    }
    return sum;
}

template <class T, class Alloc>
T NormL2Sqr(const Vector<T, Alloc>& vec) {
    T sum = T();
    for (int i = 0; i < vec.mSize; ++i) {
        sum += Math::Sqr(vec.mData[i]);
    }
    return sum;
}

template <class T, class Alloc>
T NormL2(const Vector<T, Alloc>& vec) {
    return Math::Sqrt(NormL2Sqr<T, Alloc>(vec));
}

template <class T, class Alloc>
std::ostream& operator<< (std::ostream& os, const Vector<T, Alloc>& vec) {
    os << vec[0];
    for (int i = 1; i < vec.Size(); ++i) {
        os << "," << vec[i];
    }
    return os;
}

typedef Vector<int>	Vector32I;
typedef Vector<float> Vector32F;
typedef Vector<double> Vector64F;

} //~ namespace longan

#endif /* COMMON_MATH_VECTOR_H */
