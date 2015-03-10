/*
 * evaluate_ranking_delegate.cpp
 * Created on: Jan 14, 2015
 * Author: chenguangyu
 */

#include "evaluate_ranking_delegate.h"
#include "recsys/base/rating_list.h"
#include "recsys/base/basic_predict.h"
#include "common/common.h"

namespace longan {

void EvaluateRankingDelegate::EvaluateOneUser(int userId, const UserVec& userVec, int *hitCount, int* nPrecision,
        int *nRecall) {
    ItemIdList itemList = mPredict->PredictTopNItem(userId, mOption->CurrentRankingListSize());
    int hit = 0;
    for (int i = 0; i < itemList.size(); ++i) {
        int pos = BSearch(itemList[i], userVec.Begin(), userVec.Size(),
            [](int lhs, const ItemRating& rhs)->int {
                return lhs - rhs.ItemId();
        });
        if (pos >= 0) {
            ++hit;
        }
    }
    *hitCount += hit;
    *nPrecision += itemList.size();
    *nRecall += userVec.Size();
}

void EvaluateRankingDelegateST::Evaluate(const BasicPredict *predict, RatingList *testData, const EvaluateOption *option) {
    mPredict = predict;
    mTestData = new RatingMatUsers();
    mTestData->Init(*testData);
    mOption = option;
    int hitCount = 0, nPrecision = 0, nRecall = 0;
    for (int uid = 0; uid < mTestData->NumUser(); ++uid) {
        EvaluateOneUser(uid, mTestData->GetUserVector(uid), &hitCount, &nPrecision, &nRecall);
    }
    mPrecision = static_cast<double>(hitCount) / nPrecision;
    mRecall = static_cast<double>(hitCount) / nRecall;
    mF1Score = 2.0 * (mPrecision * mRecall) / (mPrecision + mRecall);
    delete mTestData;
}

void EvaluateRankingDelegateMT::Evaluate(const BasicPredict *predict, RatingList *testData, const EvaluateOption *option) {
    mPredict = predict;
    mTestData = new RatingMatUsers();
    mTestData->Init(*testData);
    mOption = option;
    mTotoalTask = mTestData->NumUser();
    mProcessedTask = 0;
    mScheduler = new PipelinedScheduler<TaskBundle>(this, 1, mOption->NumThread(), 1);
    mScheduler->Start();
    mScheduler->WaitFinish();
    delete mScheduler;
    delete mTestData;
}

void EvaluateRankingDelegateMT::ProducerRun() {
    TaskBundle *currentBundle = new TaskBundle();
    currentBundle->reserve(TASK_BUNDLE_SIZE);
    for (int uid = 0; uid < mTestData->NumUser(); ++uid) {
        if (currentBundle->size() == TASK_BUNDLE_SIZE) {
            mScheduler->ProducerPutTask(currentBundle);
            currentBundle = new TaskBundle();
            currentBundle->reserve(TASK_BUNDLE_SIZE);
        }
        currentBundle->push_back(Task(uid));
    }
    mScheduler->ProducerPutTask(currentBundle);
    mScheduler->ProducerDone();
}

void EvaluateRankingDelegateMT::WorkerRun() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->WorkerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            EvaluateOneUser(task.userId, mTestData->GetUserVector(task.userId),
                    &task.hitCount, &task.nPrecision, &task.nRecall);
        }
        mScheduler->WorkerPutTask(currentBundle);
    }
    mScheduler->WorkerDone();
}

void EvaluateRankingDelegateMT::ConsumerRun() {
    mRunningHitCount = 0;
    mRunningNPrecision = 0;
    mRunningNRecall = 0;
    while (true) {
        TaskBundle *currentBundle = mScheduler->ConsumerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            mRunningHitCount += task.hitCount;
            mRunningNPrecision += task.nPrecision;
            mRunningNRecall += task.nRecall;
            ++mProcessedTask;
        }
        delete currentBundle;
    }
    mPrecision = static_cast<double>(mRunningHitCount) / mRunningNPrecision;
    mRecall = static_cast<double>(mRunningHitCount) / mRunningNRecall;
    mF1Score = 2.0 * (mPrecision * mRecall)/(mPrecision + mRecall);
    mScheduler->ConsumerDone();
}

void EvaluateRankingDelegateMT::MonitorRun() {
    Stopwatch stopwatch;
    while (true) {
        int progress = static_cast<int>(100.0 * mProcessedTask / mTotoalTask);
        Log::Console("recsys", "Evaluate Ranking...%d%% (%d/%d)done. current precision=%lf, recall=%lf, total time=%.2lf",
                progress, mProcessedTask, mTotoalTask,
                static_cast<double>(mRunningHitCount) / mRunningNPrecision,
                static_cast<double>(mRunningHitCount) / mRunningNRecall,
                stopwatch.Toc());
        if (mProcessedTask >= mTotoalTask) break;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

} //~ namespace longan
