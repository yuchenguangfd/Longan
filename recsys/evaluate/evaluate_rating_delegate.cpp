/*
 * evaluate_rating_delegate.cpp
 * Created on: Jan 14, 2015
 * Author: chenguangyu
 */

#include "evaluate_rating_delegate.h"
#include "recsys/base/rating_list.h"
#include "recsys/base/basic_predict.h"
#include "common/math/math.h"
#include "common/time/stopwatch.h"
#include "common/logging/logging.h"
#include <cassert>

namespace longan {

void EvaluateRatingDelegateST::Evaluate(const BasicPredict *predict, const RatingList *testData,
        const EvaluateOption *option) {
    assert(testData->NumRating() > 0);
    double sumAbs = 0.0;
    double sumSqr = 0.0;
    for (int i = 0; i < testData->NumRating(); ++i) {
        const RatingRecord& rr = testData->At(i);
        float predRating = predict->PredictRating(rr.UserId(), rr.ItemId());
        float trueRating = rr.Rating();
        float error = predRating - trueRating;
        sumAbs += Math::Abs(error);
        sumSqr += Math::Sqr(error);
    }
    mMAE = sumAbs / testData->NumRating();
    mRMSE = Math::Sqrt(sumSqr / testData->NumRating());
}

void EvaluateRatingDelegateMT::Evaluate(const BasicPredict *predict, const RatingList *testData,
        const EvaluateOption *option) {
    assert(testData->NumRating() > 0);
    mPredict = predict;
    mTestData = testData;
    mOption = option;
    mTotoalTask = mTestData->NumRating();
    mProcessedTask = 0;
    mScheduler = new PipelinedScheduler<TaskBundle>(this, 1, mOption->NumThread(), 1);
    mScheduler->Start();
    mScheduler->WaitFinish();
    delete mScheduler;
}

void EvaluateRatingDelegateMT::ProducerRun() {
    TaskBundle *currentBundle = new TaskBundle();
    currentBundle->reserve(TASK_BUNDLE_SIZE);
    for (int i = 0; i < mTestData->NumRating(); ++i) {
        const RatingRecord& rr = mTestData->At(i);
        if (currentBundle->size() == TASK_BUNDLE_SIZE) {
            mScheduler->ProducerPutTask(currentBundle);
            currentBundle = new TaskBundle();
            currentBundle->reserve(TASK_BUNDLE_SIZE);
        }
        currentBundle->push_back(Task(rr.UserId(), rr.ItemId(), rr.Rating()));
    }
    mScheduler->ProducerPutTask(currentBundle);
    mScheduler->ProducerDone();
}

void EvaluateRatingDelegateMT::WorkerRun() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->WorkerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            task.predRating = mPredict->PredictRating(task.userId, task.itemId);
        }
        mScheduler->WorkerPutTask(currentBundle);
    }
    mScheduler->WorkerDone();
}

void EvaluateRatingDelegateMT::ConsumerRun() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->ConsumerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            double error = task.predRating - task.trueRating;
            mRunningMAE.Add(Math::Abs(error));
            mRunningMSE.Add(Math::Sqr(error));
        }
        mProcessedTask += currentBundle->size();
        delete currentBundle;
    }
    mMAE = mRunningMAE.CurrentAverage();
    mRMSE = Math::Sqrt(mRunningMSE.CurrentAverage());
    mScheduler->ConsumerDone();
}

void EvaluateRatingDelegateMT::MonitorRun() {
    Stopwatch stopwatch;
    while (true) {
        int progress = static_cast<int>(100.0 * mProcessedTask / mTotoalTask);
        Log::Console("recsys", "Evaluate Rating...%d%%(%d/%d) done. current MAE=%lf, RMSE=%lf, total time=%.2lfs",
                progress, mProcessedTask, mTotoalTask,
                mRunningMAE.CurrentAverage(), Math::Sqrt(mRunningMSE.CurrentAverage()),
                stopwatch.Toc());
        if (mProcessedTask >= mTotoalTask) break;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

} //~ namespace longan
