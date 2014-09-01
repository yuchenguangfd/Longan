/*
 * small_memory_allocator.h
 * Created on: Sep 1, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_MEMORY_SMALL_MEMORY_ALLOCATOR_H
#define COMMON_MEMORY_SMALL_MEMORY_ALLOCATOR_H

#include "common/lang/types.h"
#include <vector>
#include <cstdlib>

namespace longan {

/* This class is optimized for small memory allocation. */
template <class T>
class SmallMemoryAllocator {
public:
    SmallMemoryAllocator(int poolSize = 1024 * 1024 * 16 /* 16MB */,
            int sizeThreshold = 1024 * 16 /* 16KB */);
    virtual ~SmallMemoryAllocator();
    T* Allocate(int size);
    void Deallocate(T *p, int size);
    T* allocate(int size) { return Allocate(size); }
    void deallocate(T *p, int size) { Deallocate(p, size); }
private:
    const int mObjectSize;
    const int mPoolSize;
    const int mSizeThreshold;
    std::vector<T*> mSmallMemoryPools;
    int mSmallMemoryCurrentPos;
};

template <class T>
SmallMemoryAllocator<T>::SmallMemoryAllocator(int poolSize, int sizeThreshold) :
    mObjectSize(sizeof(T)),
    mPoolSize(poolSize / sizeof(T)),
    mSizeThreshold(sizeThreshold / sizeof(T)) {
    mSmallMemoryCurrentPos = mPoolSize;
}

template <class T>
SmallMemoryAllocator<T>::~SmallMemoryAllocator() {
    for (int i = 0; i < mSmallMemoryPools.size(); i++) {
        free(mSmallMemoryPools[i]);
    }
}

template <class T>
T* SmallMemoryAllocator<T>::Allocate(int size) {
    T* p = nullptr;
    if (size >= mSizeThreshold) {
        p = (T*)malloc(size * mObjectSize);
        return p;
    }
    if (mSmallMemoryCurrentPos + size > mPoolSize) {
        p = (T*)malloc(mPoolSize * mObjectSize);
        if (p == nullptr) return nullptr;
        mSmallMemoryPools.push_back(p);
        mSmallMemoryCurrentPos = 0;
    }
    p = mSmallMemoryPools.back() + mSmallMemoryCurrentPos;
    mSmallMemoryCurrentPos += size;
    return p;
}

template <class T>
void SmallMemoryAllocator<T>::Deallocate(T *p, int size) {
    if (size >= mSizeThreshold) {
        free(p);
    }
}

} //~ namespace longan

#endif /* COMMON_MEMORY_SMALL_MEMORY_ALLOCATOR_H */
