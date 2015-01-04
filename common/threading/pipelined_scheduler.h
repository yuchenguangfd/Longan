/*
 * pipelined_scheduler.h
 * Created on: Jan 3, 2015
 * Author: chenguangyu
 */

#ifndef COMMON_THREADING_PIPELINED_SCHEDULER_H
#define COMMON_THREADING_PIPELINED_SCHEDULER_H

#include "object_buffer.h"
#include <vector>
#include <cassert>

namespace longan {

class PipelinedSchedulerClient {
public:
    virtual ~PipelinedSchedulerClient() {};
    virtual std::thread* CreateProducerThread() = 0;
    virtual std::thread* CreateWorkerThread() = 0;
    virtual std::thread* CreateConsumerThread() = 0;
    virtual std::thread* CreateMonitorThread() = 0;
};

template <class T>
class PipelinedScheduler {
public:
    typedef T Task;
    typedef std::vector<Task> TaskBundle;
public:
    PipelinedScheduler(PipelinedSchedulerClient *client, int numProducer, int numWorker, int numConsumer, int taskBundleSize,
        int bufferSize1 = DEFAULT_BUFFER_SIZE, int bufferSize2 = DEFAULT_BUFFER_SIZE);
    virtual ~PipelinedScheduler();
    void Start();
    void WaitFinish();
    void ProducerPutTaskBundle(TaskBundle* bundle);
    TaskBundle* WorkerGetTaskBundle();
    void WorkerPutTaskBundle(TaskBundle* bundle);
    TaskBundle* ConsumerGetTaskBundle();
    void ProducerDone();
    void WorkerDone();
    void ConsumerDone();
    int NumProducer() const {
        return mNumProducer;
    }
    int NumWorker() const {
        return mNumWorker;
    }
    int NumConsumer() const {
        return mNumConsumer;
    }
    int TaskBundleSize() const {
        return mTaskBundleSize;
    }
protected:
    static const int DEFAULT_BUFFER_SIZE = 128;
    PipelinedSchedulerClient *mClient;
    int mNumProducer;
    int mNumWorker;
    int mNumConsumer;
    int mTaskBundleSize;
    int mBufferSize1;
    int mBufferSize2;
    ObjectBuffer<TaskBundle*> *mObjectBuffer1;
    ObjectBuffer<TaskBundle*> *mObjectBuffer2;
    std::vector<std::thread*> mThreads;
};

template <class T>
PipelinedScheduler<T>::PipelinedScheduler(PipelinedSchedulerClient *client, int numProducer, int numWorker, int numConsumer,
    int taskBundleSize, int bufferSize1, int bufferSize2) :
    mClient(client),
    mNumProducer(numProducer),
    mNumWorker(numWorker),
    mNumConsumer(numConsumer),
    mTaskBundleSize(taskBundleSize),
    mBufferSize1(bufferSize1),
    mBufferSize2(bufferSize2) {
    mObjectBuffer1 = new ObjectBuffer<TaskBundle*>("ProducerToWorker", mBufferSize1,
        mNumProducer, mNumWorker);
    mObjectBuffer2 = new ObjectBuffer<TaskBundle*>("WorkerToConsumer", mBufferSize2,
        mNumWorker, mNumConsumer);
}

template <class T>
PipelinedScheduler<T>::~PipelinedScheduler() {
    delete mObjectBuffer1;
    delete mObjectBuffer2;
}

template<class T>
void PipelinedScheduler<T>::Start() {
    mThreads.clear();
    for (int i = 0; i < NumProducer(); ++i) {
        mThreads.push_back(mClient->CreateProducerThread());
    }
    for (int i = 0; i < NumWorker(); ++i) {
        mThreads.push_back(mClient->CreateWorkerThread());
    }
    for (int i = 0; i < NumConsumer(); ++i) {
        mThreads.push_back(mClient->CreateConsumerThread());
    }
    std::thread *monitorThread = mClient->CreateMonitorThread();
    if (monitorThread != nullptr) {
        mThreads.push_back(monitorThread);
    }
}

template<class T>
void PipelinedScheduler<T>::WaitFinish() {
    for (std::thread* th : mThreads) {
        th->join();
        delete th;
    }
}

template<class T>
inline void PipelinedScheduler<T>::ProducerPutTaskBundle(TaskBundle* bundle) {
    int rtn = mObjectBuffer1->Push(bundle);
    assert(rtn == LONGAN_SUCC);
}

template<class T>
inline typename PipelinedScheduler<T>::TaskBundle*
    PipelinedScheduler<T>::WorkerGetTaskBundle() {
    TaskBundle* bundle;
    int rtn = mObjectBuffer1->Pop(bundle);
    return (rtn == LONGAN_SUCC) ? bundle : nullptr;
}

template<class T>
inline void PipelinedScheduler<T>::WorkerPutTaskBundle(TaskBundle* bundle) {
    int rtn = mObjectBuffer2->Push(bundle);
    assert(rtn == LONGAN_SUCC);
}

template<class T>
inline typename PipelinedScheduler<T>::TaskBundle*
    PipelinedScheduler<T>::ConsumerGetTaskBundle() {
    TaskBundle* bundle;
    int rtn = mObjectBuffer2->Pop(bundle);
    return (rtn == LONGAN_SUCC) ? bundle : nullptr;
}

template<class T>
inline void PipelinedScheduler<T>::ProducerDone() {
    int rtn = mObjectBuffer1->ProducerQuit();
    assert(rtn == LONGAN_SUCC);
}

template<class T>
inline void PipelinedScheduler<T>::WorkerDone() {
    int rtn;
    rtn = mObjectBuffer1->ConsumerQuit();
    assert(rtn == LONGAN_SUCC);
    rtn = mObjectBuffer2->ProducerQuit();
    assert(rtn == LONGAN_SUCC);
}

template<class T>
inline void PipelinedScheduler<T>::ConsumerDone() {
    int rtn = mObjectBuffer2->ConsumerQuit();
    assert(rtn == LONGAN_SUCC);
}

} //~ namespace longan

#endif /* COMMON_THREADING_PIPELINED_SCHEDULER_H */
