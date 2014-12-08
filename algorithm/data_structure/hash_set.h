/*
 * hash_set.h
 * Created on: Dec 7, 2014
 * Author: chenguangyu
 */

#ifndef ALGORITHM_DATA_STRUCTURE_HASH_SET_H
#define ALGORITHM_DATA_STRUCTURE_HASH_SET_H

#include <algorithm>
#include <memory>

namespace longan {

template <class Key>
class HashSet {
public:
    virtual ~HashSet() { }
    virtual Key* Find(const Key& key) const = 0;
    virtual bool Insert(const Key& key) = 0;
};

template <class Key, class Hash, class Pred, class Alloc = std::allocator<Key>, class AllocInt = std::allocator<int>>
class HashSetChained : public HashSet<Key> {
public:
    HashSetChained(int bucketSize, int capacity = DEFAULT_CAPACITY);
    virtual ~HashSetChained();
    virtual Key* Find(const Key& key) const override;
    virtual bool Insert(const Key &key) override;
private:
    bool Full() const {
        return mSize >= mCapacity;
    };
    void Expand();
private:
    static const int DEFAULT_CAPACITY = 4096;
    int* mBucket;
    int mBucketSize;
    Key* mData;
    int* mNext;
    int mCapacity;
    int mSize;
};

template <class Key, class Hash, class Pred, class Alloc, class AllocInt>
HashSetChained<Key, Hash, Pred, Alloc, AllocInt>::HashSetChained(int bucketSize, int capacity) {
    AllocInt allocInt;
    mBucket = allocInt.allocate(bucketSize);
    mBucketSize = bucketSize;
    std::fill(mBucket, mBucket + mBucketSize, -1);
    Alloc alloc;
    mData = alloc.allocate(capacity);
    mNext = allocInt.allocate(capacity);
    mCapacity = capacity;
    mSize = 0;
}

template <class Key, class Hash, class Pred, class Alloc,class AllocInt>
HashSetChained<Key, Hash, Pred, Alloc, AllocInt>::~HashSetChained() {
    AllocInt allocInt;
    allocInt.deallocate(mBucket, mBucketSize);
    Alloc alloc;
    for (int i = 0; i < mSize; ++i) {
        alloc.destroy(&mData[i]);
    }
    alloc.deallocate(mData, mSize);
    allocInt.deallocate(mNext, mSize);
}

template <class Key, class Hash, class Pred, class Alloc, class AllocInt>
Key* HashSetChained<Key, Hash, Pred, Alloc, AllocInt>::Find(const Key& key) const {
    Hash hash;
    Pred pred;
    int slot = hash(key) % mBucketSize;
    int p = mBucket[slot];
    while(p != -1) {
        if (pred(key, mData[p])) {
            return &mData[p];
        }
        p = mNext[p];
    }
    return nullptr;
}

template <class Key, class Hash, class Pred, class Alloc, class AllocInt>
bool HashSetChained<Key, Hash, Pred, Alloc, AllocInt>::Insert(const Key& key) {
    Hash hash;
    int slot = hash(key) % mBucketSize;
    if (Full()) {
        Expand();
    }
    mData[mSize] = key;
    mNext[mSize] = mBucket[slot];
    mBucket[slot] = mSize;
    ++mSize;
    return true;
}

template <class Key, class Hash, class Pred, class Alloc, class AllocInt>
void HashSetChained<Key, Hash, Pred, Alloc, AllocInt>::Expand() {
    int newCapacity = mCapacity * 2;
    Alloc alloc;
    AllocInt allocInt;
    Key* newData = alloc.allocate(newCapacity);
    int* newNext = allocInt.allocate(newCapacity);
    for(int i = 0; i < mCapacity; ++i) {
        alloc.construct(&newData[i], std::move(mData[i]));
        alloc.destroy(&mData[i]);
        newNext[i] = mNext[i];
    }
    alloc.deallocate(mData, mCapacity);
    allocInt.deallocate(mNext, mCapacity);
    mCapacity = newCapacity;
    mData = newData;
    mNext = newNext;
}

} //~ namespace longan

#endif // ALGORITHM_DATA_STRUCTURE_HASH_SET_H
