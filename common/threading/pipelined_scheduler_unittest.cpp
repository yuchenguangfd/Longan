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
        mScheduler = new PipelinedScheduler<Task>(this, 1, numWorkers, 1, 10, 5, 5);
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
    struct Task {
        int begin;
        int end;
        int sum;
    };
    void ProducerRun() {
        const int SEG_SIZE = 10;
        PipelinedScheduler<Task>::TaskBundle *currentBundle
            = new PipelinedScheduler<Task>::TaskBundle();
        currentBundle->reserve(mScheduler->TaskBundleSize());
        for (int i = 0; i < mSize; i += SEG_SIZE) {
           Task task;
           task.begin = i;
           task.end = (i + SEG_SIZE <= mSize)? i + SEG_SIZE : mSize;
           task.sum = 0;
           if (currentBundle->size() == mScheduler->TaskBundleSize()) {
               mScheduler->ProducerPutTaskBundle(currentBundle);
               currentBundle = new PipelinedScheduler<Task>::TaskBundle();
               currentBundle->reserve(mScheduler->TaskBundleSize());
           }
           currentBundle->push_back(task);
        }
        mScheduler->ProducerPutTaskBundle(currentBundle);
        mScheduler->ProducerDone();
    }
    void WorkerRun() {
        while (true) {
            PipelinedScheduler<Task>::TaskBundle *currentBundle = mScheduler->WorkerGetTaskBundle();
            if (currentBundle == nullptr) break;
            for (int i = 0; i < currentBundle->size(); ++i) {
                Task& task = currentBundle->at(i);
                for (int j = task.begin; j < task.end; ++j) {
                    task.sum += mData[j];
                }
            }
            mScheduler->WorkerPutTaskBundle(currentBundle);
        }
        mScheduler->WorkerDone();
    }
    void ConsumerRun() {
        mSum = 0;
        while (true) {
            PipelinedScheduler<Task>::TaskBundle *currentBundle = mScheduler->ConsumerGetTaskBundle();
            if (currentBundle == nullptr) break;
            for (int i = 0; i < currentBundle->size(); ++i) {
                Task& task = currentBundle->at(i);
                mSum += task.sum;
            }
            delete currentBundle;
        }
        mScheduler->ConsumerDone();
    }
private:
    PipelinedScheduler<Task> *mScheduler;
    int *mData;
    int mSize;
    int mSum;
};

TEST(PipelinedSchedulerTest, ParallelComputeSumOK) {
    int *array = nullptr;
    int size = 100000;
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
