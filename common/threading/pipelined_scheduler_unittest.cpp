/*
 * pipelined_scheduler_unittest.cpp
 * Created on: Jan 4, 2014
 * Author: chenguangyu
 */

#include "pipelined_scheduler.h"
#include "common/system/system_info.h"
#include "common/util/array_helper.h"
#include <gtest/gtest.h>

using namespace std;
using namespace longan;

class ParallelComputeSum : public PipelinedSchedulerClient {
public:
    int ComputeSum(int *data, int size) {
        mData = data;
        mSize = size;
        int numWorkers = SystemInfo::GetNumCPUCore();
        mScheduler = new PipelinedScheduler<SumTask>(this, 1, numWorkers, 1);
        mScheduler->Start();
        mScheduler->WaitFinish();
        delete mScheduler;
        return mSum;
    }
    virtual std::thread* CreateProducerThread() override {
        return new std::thread(&ParallelComputeSum::ProducerRun, this);
    }
    virtual std::thread* CreateWorkerThread() override {
        return new std::thread(&ParallelComputeSum::WorkerRun, this);
    }
    virtual std::thread* CreateConsumerThread() override {
        return new std::thread(&ParallelComputeSum::ConsumerRun, this);
    }
    virtual std::thread* CreateMonitorThread() override {
        return nullptr;
    }
private:
    struct SumTask {
        int begin;
        int end;
        int sum;
    };
    void ProducerRun() {
        const int SEG_SIZE = 128;
        for (int i = 0; i < mSize; i += SEG_SIZE) {
           SumTask *task = new SumTask();
           task->begin = i;
           task->end = (i + SEG_SIZE <= mSize)? i + SEG_SIZE : mSize;
           task->sum = 0;
           mScheduler->ProducerPutTask(task);
        }
        mScheduler->ProducerDone();
    }
    void WorkerRun() {
        while (true) {
            SumTask *task = mScheduler->WorkerGetTask();
            if (task == nullptr) break;
            for (int i = task->begin; i < task->end; ++i) {
                task->sum += mData[i];
            }
            mScheduler->WorkerPutTask(task);
        }
        mScheduler->WorkerDone();
    }
    void ConsumerRun() {
        mSum = 0;
        while (true) {
            SumTask *task = mScheduler->ConsumerGetTask();
            if (task == nullptr) break;
            mSum += task->sum;
            delete task;
        }
        mScheduler->ConsumerDone();
    }
private:
    PipelinedScheduler<SumTask> *mScheduler;
    int *mData;
    int mSize;
    int mSum;
};

TEST(PipelinedSchedulerTest, ParallelComputeSumOK) {
    int *array = nullptr;
    int size = 10000000;
    ArrayHelper::CreateArray1D(&array, size);
    ArrayHelper::InitArray1D(array, size, 1);
    ParallelComputeSum pcs;
    EXPECT_EQ(size, pcs.ComputeSum(array, size));
    ArrayHelper::ReleaseArray1D(&array, size);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
