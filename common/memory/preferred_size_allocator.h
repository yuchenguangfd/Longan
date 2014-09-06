/*
 * preferred_size_allocator.h
 * Created on: Sep 1, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_MEMORY_PREFERRED_SIZE_ALLOCATOR_H
#define COMMON_MEMORY_PREFERRED_SIZE_ALLOCATOR_H

#include <vector>
#include <map>
#include <cstdlib>
#include <cassert>
#include <iostream>

namespace longan {

/* This class is optimized for a fixed size memory allocation.*/
template <class T>
class PreferredSizeAllocator {
public:
    PreferredSizeAllocator(int preferredSize, int poolSize = 1024 * 1024 * 32 /* 32MB */);
    ~PreferredSizeAllocator();
    T* Allocate(int size);
    void Deallocate(T *p, int size);
    T* allocate(int size) { return Allocate(size); }
    void deallocate(T *p, int size) { Deallocate(p, size); }
protected:
    void NewMemoryPool();
protected:
    const int mObjectSize;
    const int mPreferredSize;
    int mPoolSize;
    std::vector<T*> mMemoryPools;
    std::vector<T*> mAvailablePtrStack;
};

template <class T>
PreferredSizeAllocator<T>::PreferredSizeAllocator(int preferredSize, int poolSize) :
    mObjectSize(sizeof(T)),
    mPreferredSize(preferredSize) {
    mPoolSize = ((poolSize/mObjectSize)/mPreferredSize + 1) * mPreferredSize;
    NewMemoryPool();
}

template <class T>
PreferredSizeAllocator<T>::~PreferredSizeAllocator() {
    for (int i = 0; i < mMemoryPools.size(); ++i) {
        free((void*)mMemoryPools[i]);
    }
}

template <class T>
inline T* PreferredSizeAllocator<T>::Allocate(int size) {
    T *p = nullptr;
    if (size != mPreferredSize) {
        p = (T*)malloc(size * mObjectSize);
        return p;
    }
    if (mAvailablePtrStack.empty()) {
        NewMemoryPool();
    }
    p = mAvailablePtrStack.back();
    mAvailablePtrStack.pop_back();
    return p;
}

template <class T>
inline void PreferredSizeAllocator<T>::Deallocate(T *p, int size) {
    if (size != mPreferredSize) {
        free((void*)p);
    }
    mAvailablePtrStack.push_back(p);
}

template <class T>
inline void PreferredSizeAllocator<T>::NewMemoryPool() {
    T *pool = (T*)malloc(mPoolSize * mObjectSize);
    mMemoryPools.push_back(pool);
    for (int i = 0; i < mPoolSize; i += mPreferredSize) {
        mAvailablePtrStack.push_back(pool + i);
    }
}

} //~ namespace longan

#endif /* COMMON_MEMORY_PREFERRED_SIZE_ALLOCATOR_H */
