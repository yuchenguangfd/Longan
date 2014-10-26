#ifndef COMMON_MATH_VECTOR_H
#define COMMON_MATH_VECTOR_H

#include "math.h"
#include "common/util/random.h"
#include <iostream>
#include <cassert>

namespace longan {

template <class T, class Alloc = std::allocator<T> >
class Vector {
public:
    Vector();
	Vector(int size, bool init = false, T initValue = T());
    template <class InputIterator>
	Vector(InputIterator begin, InputIterator end);
	Vector(const Vector<T, Alloc>& orig);
    Vector(Vector<T, Alloc>&& orig);
	~Vector();
    Vector<T, Alloc>& operator= (const Vector<T, Alloc>& rhs);
    Vector<T, Alloc>& operator= (Vector<T, Alloc>&& rhs);

    template <class T1, class Alloc1>
    friend bool operator== (const Vector<T1, Alloc1>& lhs, const Vector<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend bool operator!= (const Vector<T1, Alloc1>& lhs, const Vector<T1, Alloc1>& rhs);

    template <class T1, class Alloc1>
    friend Vector<T1, Alloc1> operator+ (const Vector<T1, Alloc1>& lhs, const Vector<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend Vector<T1, Alloc1> operator- (const Vector<T1, Alloc1>& lhs, const Vector<T1, Alloc1>& rhs);
    template <class T1, class Alloc1>
    friend T1 InnerProd(const Vector<T1, Alloc1>& lhs, const Vector<T1, Alloc1>& rhs);

    int Size() const {
	    return mSize;
	};
	T* Data() {
	    return mData;
	}
	const T* Data() const {
	    return mData;
	}
    T& operator[] (int index) {
        return mData[index];
    }
    const T& operator[] (int index) const {
        return mData[index];
    }
public:
    static Vector<T, Alloc> Zeros(int size);
    static Vector<T, Alloc> Rand(int size);
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
        for (int i = 0; i < mSize; ++i) {
            mData[i] = initValue;
        }
    }
}

template <class T, class Alloc>
template <class InputIterator>
Vector<T, Alloc>::Vector(InputIterator begin, InputIterator end) {
    mSize = end - begin;
    Alloc alloc;
    mData = alloc.allocate(mSize);
    for (int i = 0; i < mSize; ++i) {
        mData[i] = *begin++;
    }
}

template <class T, class Alloc>
Vector<T, Alloc>::Vector(const Vector<T, Alloc>& orig) :
    mSize(orig.mSize) {
    Alloc alloc;
    mData = alloc.allocate(mSize);
    for (int i = 0; i < mSize; ++i) {
        mData[i] = orig.mData[i];
    }
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
    for (int i = 0; i < mSize; ++i) {
        mData[i] = rhs.mData[i];
    }
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
    return vec;
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
std::ostream& operator << (std::ostream& os, const Vector<T, Alloc>& vec) {
    if (vec.Size() == 0) return os;
    os << "(" << vec[0];
    for (int i = 1; i < vec.Size(); ++i) {
        os << "," << vec[i];
    }
    os << ")";
    return os;
}

typedef Vector<int>	Vector32I;
typedef Vector<float> Vector32F;
typedef Vector<double> Vector64F;

} //~ namespace longan

#endif // COMMON_MATH_VECTOR_H
