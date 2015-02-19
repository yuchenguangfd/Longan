/*
 * evaluate_rating_delegate.cpp
 * Created on: Jan 14, 2015
 * Author: chenguangyu
 */

#include "evaluate_coverage_delegate.h"
#include "recsys/base/rating_list.h"
#include "recsys/base/basic_predict.h"
#include "common/math/math.h"
#include "common/time/stopwatch.h"
#include "common/logging/logging.h"
#include <algorithm>
#include <cassert>

namespace longan {

void EvaluateCoverageDelegateST::Evaluate(const BasicPredict *predict, const RatingList *testData,
        const EvaluateOption *option) {
    mPredict = predict;
    mTestData = testData;
    mOption = option;

    int N = mTestData->NumItem();
    std::vector<int> itemFreqs(N);
    int sumFreq = 0;
    for (int uid = 0; uid < mTestData->NumUser(); ++uid) {
        ItemIdList itemList = predict->PredictTopNItem(uid, option->RankingListSize());
        for (int iid : itemList) {
            ++itemFreqs[iid];
            ++sumFreq;
        }
    }

    int count = 0;
    for (int i = 0; i < N; ++i) {
        if (itemFreqs[i] > 0) ++count;
    }
    mCoverage = static_cast<double>(count) / N;

    double sum = 0.0;
    std::vector<double> pItems(N);
    for (int i = 0; i < N; ++i) {
        pItems[i] = static_cast<double>(itemFreqs[i]) / sumFreq;;
        if (pItems[i] > 0.0) {
            sum += pItems[i] * Math::Log(pItems[i]);
        }
    }
    mEntropy = -sum;

    std::sort(pItems.begin(), pItems.end());
    sum = 0.0;
    for (int i = 0; i < N; ++i) {
        sum += (2*i+1-N) * pItems[i];
    }
    mGiniIndex = sum / (N-1);
}

void EvaluateCoverageDelegateMT::Evaluate(const BasicPredict *predict, const RatingList *testData,
        const EvaluateOption *option) {
    mPredict = predict;
    mTestData = testData;
    mOption = option;
    mScheduler = new PipelinedScheduler<Task>(this, 1, mOption->NumThread(), 1);
    mScheduler->Start();
    mScheduler->WaitFinish();
    delete mScheduler;
}

void EvaluateCoverageDelegateMT::ProducerRun() {
    for (int uidBegin = 0; uidBegin < mTestData->NumUser(); uidBegin += NUM_USER_PER_TASK) {
        int uidEnd = (uidBegin + NUM_USER_PER_TASK < mTestData->NumUser()) ?
                uidBegin + NUM_USER_PER_TASK : mTestData->NumUser();
        Task* task = new Task();
        task->userIdBegin = uidBegin;
        task->userIdEnd = uidEnd;
        task->itemFreqs.resize(mTestData->NumItem());
        mScheduler->ProducerPutTask(task);
    }
    mScheduler->ProducerDone();
}

void EvaluateCoverageDelegateMT::WorkerRun() {
    while (true) {
        Task *task = mScheduler->WorkerGetTask();
        if (task == nullptr) break;
        for (int uid = task->userIdBegin; uid < task->userIdEnd; ++uid) {
            ItemIdList itemList = mPredict->PredictTopNItem(uid, mOption->RankingListSize());
            for (int iid : itemList) {
                ++task->itemFreqs[iid];
            }
        }
        mScheduler->WorkerPutTask(task);
    }
    mScheduler->WorkerDone();
}

void EvaluateCoverageDelegateMT::ConsumerRun() {
    mTotoalTask = mTestData->NumUser();
    mProcessedTask = 0;
    std::vector<int> itemFreqs(mTestData->NumItem());
    int sumFreq = 0;
    while (true) {
        Task *task = mScheduler->ConsumerGetTask();
        if (task == nullptr) break;
        for (int i = 0; i < task->itemFreqs.size(); ++i) {
            itemFreqs[i] += task->itemFreqs[i];
            sumFreq += task->itemFreqs[i];
        }
        mProcessedTask += task->userIdEnd - task->userIdBegin;
        delete task;
    }

    int N = itemFreqs.size();
    int count = 0;
    for (int i = 0; i < N; ++i) {
        if (itemFreqs[i] > 0) ++count;
    }
    mCoverage = static_cast<double>(count) / N;

    double sum = 0.0;
    std::vector<double> pItems(N);
    for (int i = 0; i < N; ++i) {
        pItems[i] = static_cast<double>(itemFreqs[i]) / sumFreq;
        if (pItems[i] > 0.0) {
            sum += pItems[i] * Math::Log(pItems[i]);
        }
    }
    mEntropy = -sum;

    std::sort(pItems.begin(), pItems.end());
    sum = 0.0;
    for (int i = 0; i < N; ++i) {
        sum += (2*i+1-N) * pItems[i];
    }
    mGiniIndex = sum / (N-1);

    mScheduler->ConsumerDone();
}

void EvaluateCoverageDelegateMT::MonitorRun() {
    Stopwatch stopwatch;
    while (true) {
        int progress = static_cast<int>(100.0 * mProcessedTask / mTotoalTask);
        Log::Console("recsys", "Evaluate Coverage...%d%%(%d/%d) done. total time=%.2lfs",
                progress, mProcessedTask, mTotoalTask,
                stopwatch.Toc());
        if (mProcessedTask >= mTotoalTask) break;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

} //~ namespace longan
