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
    virtual ~PipelinedSchedulerClient() { };
    virtual std::thread* CreateProducerThread() = 0;
    virtual std::thread* CreateWorkerThread() = 0;
    virtual std::thread* CreateConsumerThread() = 0;
    virtual std::thread* CreateMonitorThread() = 0;
};

template <class Task>
class PipelinedScheduler {
public:
    PipelinedScheduler(PipelinedSchedulerClient *client,
        int numProducer, int numWorker, int numConsumer,
        int bufferSize1 = DEFAULT_BUFFER_SIZE, int bufferSize2 = DEFAULT_BUFFER_SIZE);
    virtual ~PipelinedScheduler();
    void Start();
    void WaitFinish();
    void ProducerPutTask(Task* task) {
        int rtn = mObjectBuffer1->Push(task);
        assert(rtn == LONGAN_SUCC);
    }
    Task* WorkerGetTask() {
        Task* task;
        int rtn = mObjectBuffer1->Pop(task);
        return (rtn == LONGAN_SUCC) ? task : nullptr;
    }
    void WorkerPutTask(Task* task) {
        int rtn = mObjectBuffer2->Push(task);
        assert(rtn == LONGAN_SUCC);
    }
    Task* ConsumerGetTask() {
        Task* task;
        int rtn = mObjectBuffer2->Pop(task);
        return (rtn == LONGAN_SUCC) ? task : nullptr;
    }
    void ProducerDone() {
        int rtn = mObjectBuffer1->ProducerQuit();
        assert(rtn == LONGAN_SUCC);
    }
    void WorkerDone() {
        int rtn;
        rtn = mObjectBuffer1->ConsumerQuit();
        assert(rtn == LONGAN_SUCC);
        rtn = mObjectBuffer2->ProducerQuit();
        assert(rtn == LONGAN_SUCC);
    }
    void ConsumerDone() {
        int rtn = mObjectBuffer2->ConsumerQuit();
        assert(rtn == LONGAN_SUCC);
    }
    int NumProducer() const { return mNumProducer; }
    int NumWorker() const { return mNumWorker; }
    int NumConsumer() const { return mNumConsumer; }
protected:
    static const int DEFAULT_BUFFER_SIZE = 128;
    PipelinedSchedulerClient *mClient;
    int mNumProducer;
    int mNumWorker;
    int mNumConsumer;
    int mBufferSize1;
    int mBufferSize2;
    ObjectBuffer<Task*> *mObjectBuffer1;
    ObjectBuffer<Task*> *mObjectBuffer2;
    std::vector<std::thread*> mThreads;
};

template <class Task>
PipelinedScheduler<Task>::PipelinedScheduler(PipelinedSchedulerClient *client,
    int numProducer, int numWorker, int numConsumer, int bufferSize1, int bufferSize2) :
    mClient(client),
    mNumProducer(numProducer),
    mNumWorker(numWorker),
    mNumConsumer(numConsumer),
    mBufferSize1(bufferSize1),
    mBufferSize2(bufferSize2) {
    mObjectBuffer1 = new ObjectBuffer<Task*>("Producer-Worker", mBufferSize1, mNumProducer, mNumWorker);
    mObjectBuffer2 = new ObjectBuffer<Task*>("Worker-Consumer", mBufferSize2, mNumWorker, mNumConsumer);
}

template <class Task>
PipelinedScheduler<Task>::~PipelinedScheduler() {
    delete mObjectBuffer1;
    delete mObjectBuffer2;
}

template <class Task>
void PipelinedScheduler<Task>::Start() {
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

template <class Task>
void PipelinedScheduler<Task>::WaitFinish() {
    for (std::thread* th : mThreads) {
        th->join();
        delete th;
    }
}

} //~ namespace longan

#endif /* COMMON_THREADING_PIPELINED_SCHEDULER_H */
