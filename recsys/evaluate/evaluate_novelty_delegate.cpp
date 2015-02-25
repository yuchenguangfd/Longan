/*
 * evaluate_novelty_delegate.cpp
 * Created on: Feb 19, 2015
 * Author: chenguangyu
 */

#include "evaluate_novelty_delegate.h"
#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_list.h"
#include "recsys/base/rating_trait.h"
#include "common/logging/logging.h"
#include "common/time/stopwatch.h"

namespace longan {

void EvaluateNoveltyDelegateST::Evaluate(const BasicPredict *predict, const RatingList *trainData,
        const EvaluateOption *option) {
    mPredict = predict;
    mTrainData = trainData;
    mOption = option;
    mRatingTrait = new RatingTrait();
    mRatingTrait->Init(*trainData);
    RunningAverage<double> runningNovelty;
    for (int uid = 0; uid < mTrainData->NumUser(); ++uid) {
        ItemIdList itemList = mPredict->PredictTopNItem(uid, mOption->CurrentRankingListSize());
        double sum = 0;
        for (int iid : itemList) {
            sum += Math::Log(1.0 + mRatingTrait->ItemPopularity(iid));
        }
        sum /= itemList.size();
        runningNovelty.Add(sum);
    }
    mNovelty = runningNovelty.CurrentAverage();
    delete mRatingTrait;
}

void EvaluateNoveltyDelegateMT::Evaluate(const BasicPredict *predict, const RatingList *trainData,
        const EvaluateOption *option) {
    mPredict = predict;
    mTrainData = trainData;
    mOption = option;
    mRatingTrait = new RatingTrait();
    mRatingTrait->Init(*trainData);
    mTotoalTask = mTrainData->NumUser();
    mProcessedTask = 0;
    mScheduler = new PipelinedScheduler<TaskBundle>(this, 1, mOption->NumThread(), 1);
    mScheduler->Start();
    mScheduler->WaitFinish();
    delete mScheduler;
    delete mRatingTrait;
}

void EvaluateNoveltyDelegateMT::ProducerRun() {
    TaskBundle *currentBundle = new TaskBundle();
    currentBundle->reserve(TASK_BUNDLE_SIZE);
    for (int uid = 0; uid < mTrainData->NumUser(); ++uid) {
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

void EvaluateNoveltyDelegateMT::WorkerRun() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->WorkerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            ItemIdList itemList = mPredict->PredictTopNItem(task.userId, mOption->CurrentRankingListSize());
            double sum = 0;
            for (int iid : itemList) {
                sum += Math::Log(1.0 + mRatingTrait->ItemPopularity(iid));
            }
            task.userNovelty = sum / itemList.size();
        }
        mScheduler->WorkerPutTask(currentBundle);
    }
    mScheduler->WorkerDone();
}

void EvaluateNoveltyDelegateMT::ConsumerRun() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->ConsumerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            mRunningNovelty.Add(task.userNovelty);
            ++mProcessedTask;
        }
        delete currentBundle;
    }
    mNovelty = mRunningNovelty.CurrentAverage();
    mScheduler->ConsumerDone();
}

void EvaluateNoveltyDelegateMT::MonitorRun() {
    Stopwatch stopwatch;
    while (true) {
        int progress = static_cast<int>(100.0 * mProcessedTask / mTotoalTask);
        Log::Console("recsys", "Evaluate Novelty...%d%%(%d/%d) done. current novelty=%lf, total time=%.2lfs",
                progress, mProcessedTask, mTotoalTask,
                mRunningNovelty.CurrentAverage(),
                stopwatch.Toc());
        if (mProcessedTask >= mTotoalTask) break;
        std::this_thread::sleep_for(std::chrono::seconds(10));
   }
}

} //~ namespace longan
