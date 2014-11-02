/*
 * object_buffer.h
 * Created on: Oct 30, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_THREADING_OBJECT_BUFFER_H
#define COMMON_THREADING_OBJECT_BUFFER_H

#include "common/lang/types.h"
#include "common/error.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <string>

namespace longan {

// class for producer-consumer threading model
template <class T>
class ObjectBuffer {
public:
    typedef T ObjectType;
    ObjectBuffer(const std::string& name, int maxSize, int numProducer = 0, int numConsumer = 0);
    // Producer push object t, will block producer when buffer is full
    int Push(const T& t) {
        int64 id;
        return Push(t, id);
    }
    int Push(const T& t, int64& id);
    // Producer push object t, will NOT block producer when buffer is full
    int TryPush(const T& t)  {
        int64 id;
        return TryPush(t, id);
    }
    int TryPush(const T& t, int64& id);
    int ProducerQuit();
    // Consumer pop object t, will block consumer when buffer is empty
    int Pop(T& t) {
        int64 id;
        return Pop(t, id);
    }
    int Pop(T& t, int64& id);
    // Consumer try pop object t, will NOT block consumer when buffer is empty
    int TryPop(T& t) {
        int64 id;
        return TryPop(t, id);
    }
    int TryPop(T& t, int64& id);
    int ConsumerQuit();
    int Clear();
    int ForceFail();
    int Size() const {
        return mQueue.size();
    }
    int MaxSize() const {
        return mMaxSize;
    }
    int64 CurrentId() const {
        return mObjectId;
    }
    int NumProducer() const {
        return mNumProducer;
    }
    int NumConsumer() const {
        return mNumConsumer;;
    }
    int NumAliveProducer() const {
        return mNumAliveProducer;
    }
    int NumAliveConsumer() const {
        return mNumAliveConsumer;
    }
    bool IsValid() const {
        return !(mNoAliveProducer || mNoAliveConsumer || mFailed);
    }
private:
    std::string mName;
    int mMaxSize;
    int mNumProducer;
    int mNumConsumer;
    int mNumAliveProducer;
    int mNumAliveConsumer;

    volatile int64 mObjectId;
    std::deque<T> mQueue;
    std::mutex mMutex;
    std::condition_variable mNotFull;
    std::condition_variable mNotEmpty;

    bool mNoAliveProducer;
    bool mNoAliveConsumer;
    bool mFailed;
};

template <class T>
ObjectBuffer<T>::ObjectBuffer(const std::string& name, int maxSize, int numProducer, int numConsumer) :
    mName(name),
    mMaxSize(maxSize),
    mNumProducer(numProducer),
    mNumConsumer(numConsumer),
    mNumAliveProducer(numProducer),
    mNumAliveConsumer(numConsumer),
    mObjectId(0),
    mNoAliveProducer(false),
    mNoAliveConsumer(false),
    mFailed(false) { }

template <class T>
int ObjectBuffer<T>::Push(const T& t, int64& id) {
    std::unique_lock<std::mutex> lock(mMutex);
    while (!mFailed && !mNoAliveConsumer && mQueue.size() >= mMaxSize) {
        mNotFull.wait(lock);
    }
    if (mFailed) {
        return LONGAN_OBJECT_BUFFER_FAILED;
    }
    if (mNoAliveConsumer) {
        return LONGAN_OBJECT_BUFFER_NO_ALIVE_CONSUMER;
    }
    id = mObjectId++;
    mQueue.push_front(t);
    mNotEmpty.notify_one();
    return LONGAN_SUCC;
}

template <class T>
int ObjectBuffer<T>::TryPush(const T& t, int64& id) {
    std::unique_lock<std::mutex> lock(mMutex);
    if (mFailed) {
        return LONGAN_OBJECT_BUFFER_FAILED;
    }
    if (mNoAliveConsumer) {
        return LONGAN_OBJECT_BUFFER_NO_ALIVE_CONSUMER;
    }
    if (mQueue.size() >= mMaxSize) {
        return LONGAN_OBJECT_BUFFER_TRY_PUSH_FAILED;
    }
    id = mObjectId++;
    mQueue.push_front(t);
    mNotEmpty.notify_one();
    return LONGAN_SUCC;
}

template <class T>
int ObjectBuffer<T>::ProducerQuit() {
    std::unique_lock<std::mutex> lock(mMutex);
    if (mFailed) {
        return LONGAN_OBJECT_BUFFER_FAILED;
    }
    if (mNumProducer == 0) { //0 means infinite
        return LONGAN_OBJECT_BUFFER_UNEXCEPTED_PRODUCER_QUIT;
    }
    if (mNumAliveProducer == 0) { // no alive producer left
        return LONGAN_OBJECT_BUFFER_UNEXCEPTED_PRODUCER_QUIT;
    }
    --mNumAliveProducer;
    if (mNumAliveProducer == 0) {
        mNoAliveProducer = true;
        mNotEmpty.notify_all(); //wake up all waiting consumer threads
    }
    return LONGAN_SUCC;
}

template <class T>
int ObjectBuffer<T>::Pop(T& t, int64& id) {
    std::unique_lock<std::mutex> lock(mMutex);
    while (!mFailed && !mNoAliveProducer && mQueue.empty()) {
        mNotEmpty.wait(lock);
    }
    if (mFailed) {
        return LONGAN_OBJECT_BUFFER_FAILED;
    }
    if (mQueue.empty()) {
        return LONGAN_OBJECT_BUFFER_NO_MORE_OBJECT;
    }
    t = mQueue.back();
    id = mObjectId - mQueue.size();
    mQueue.pop_back();
    mNotFull.notify_one();
    return LONGAN_SUCC;
}

template <class T>
int ObjectBuffer<T>::TryPop(T& t, int64& id) {
    std::unique_lock<std::mutex> lock(mMutex);
    if (mFailed) {
        return LONGAN_OBJECT_BUFFER_FAILED;
    }
    if (mQueue.empty()){
        if (mNoAliveProducer) {
            return LONGAN_OBJECT_BUFFER_NO_MORE_OBJECT;
        } else {
            return LONGAN_OBJECT_BUFFER_TRY_POP_FAILED;
        }
    }
    t = mQueue.back();
    id = mObjectId - mQueue.size();
    mQueue.pop_back();
    mNotFull.notify_one();
    return LONGAN_SUCC;
}

template <class T>
int ObjectBuffer<T>::ConsumerQuit() {
    std::unique_lock<std::mutex> lock(mMutex);
    if (mFailed) {
        return LONGAN_OBJECT_BUFFER_FAILED;
    }
    if (mNumConsumer == 0) { //0 means infinite
        return LONGAN_OBJECT_BUFFER_UNEXCEPTED_CONSUMER_QUIT;
    }
    if (mNumAliveConsumer == 0) { // no alive consumer left
        return LONGAN_OBJECT_BUFFER_UNEXCEPTED_CONSUMER_QUIT;
    }
    --mNumAliveConsumer;
    if (mNumAliveConsumer == 0) {
        mNoAliveConsumer = true;
        mQueue.clear();
        mNotFull.notify_all(); //wake up all waiting producer threads
    }
    return LONGAN_SUCC;
}

template <class T>
int ObjectBuffer<T>::Clear() {
    std::unique_lock<std::mutex> lock(mMutex);
    mQueue.clear();
    return LONGAN_SUCC;
}

template <class T>
int ObjectBuffer<T>::ForceFail() {
    std::unique_lock<std::mutex> lock(mMutex);
    mFailed = true;
    mQueue.clear();
    // if threads are blocked pushing or popping, we need to notify them
    mNotFull.notify_all();
    mNotEmpty.notify_all();
    return LONGAN_SUCC;
}

} //~ namespace longan

#endif /* COMMON_THREADING_OBJECT_BUFFER_H */
