/*
 * evaluate_diversity_delegate.cpp
 * Created on: Feb 18, 2015
 * Author: chenguangyu
 */

#include "evaluate_diversity_delegate.h"
#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_list.h"
#include "common/common.h"


namespace longan {

void EvaluateDiversityDelegateST::Evaluate(const BasicPredict *predict, const RatingList *testData,
        const EvaluateOption *option) {
    mPredict = predict;
    mTestData = testData;
    mOption = option;
    RunningAverage<double> runningDiversity;
    for (int uid = 0; uid < mTestData->NumUser(); ++uid) {
        ItemIdList itemList = predict->PredictTopNItem(uid, mOption->CurrentRankingListSize());
        double sum = 0.0;
        for (int i = 0; i < itemList.size(); ++i) {
            for (int j = i + 1; j < itemList.size(); ++j) {
                sum += mPredict->ComputeItemSimilarity(itemList[i], itemList[j]);
            }
        }
        double userDiversity = 1.0 - sum / (itemList.size()*(itemList.size()-1)/2);
        runningDiversity.Add(userDiversity);
    }
    mDiversity = runningDiversity.CurrentAverage();
}

void EvaluateDiversityDelegateMT::Evaluate(const BasicPredict *predict, const RatingList *testData,
        const EvaluateOption *option) {
    mPredict = predict;
    mTestData = testData;
    mOption = option;
    mTotoalTask = mTestData->NumUser();
    mProcessedTask = 0;
    mScheduler = new PipelinedScheduler<TaskBundle>(this, 1, mOption->NumThread(), 1);
    mScheduler->Start();
    mScheduler->WaitFinish();
    delete mScheduler;
}

void EvaluateDiversityDelegateMT::ProducerRun() {
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

void EvaluateDiversityDelegateMT::WorkerRun() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->WorkerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            ItemIdList itemList = mPredict->PredictTopNItem(task.userId, mOption->CurrentRankingListSize());
            double sum = 0.0;
            for (int j = 0; j < itemList.size(); ++j) {
                for (int k = j + 1; k < itemList.size(); ++k) {
                    sum += mPredict->ComputeItemSimilarity(itemList[j], itemList[k]);
                }
            }
            task.userDiversity = 1.0 - sum / (itemList.size()*(itemList.size()-1)/2);
        }
        mScheduler->WorkerPutTask(currentBundle);
    }
    mScheduler->WorkerDone();
}

void EvaluateDiversityDelegateMT::ConsumerRun() {
    mRunningDiversity.Reset();
    while (true) {
        TaskBundle *currentBundle = mScheduler->ConsumerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            mRunningDiversity.Add(task.userDiversity);
            ++mProcessedTask;
        }
        delete currentBundle;
    }
    mDiversity = mRunningDiversity.CurrentAverage();
    mScheduler->ConsumerDone();
}

void EvaluateDiversityDelegateMT::MonitorRun() {
    Stopwatch stopwatch;
    while (true) {
        int progress = static_cast<int>(100.0 * mProcessedTask / mTotoalTask);
        Log::Console("recsys", "Evaluate Diversity...%d%%(%d/%d)done. current diversity=%lf, total time=%.2lf",
                progress, mProcessedTask, mTotoalTask,
                mRunningDiversity.CurrentAverage(),
                stopwatch.Toc());
        if (mProcessedTask >= mTotoalTask) break;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

} //~ namespace longan
