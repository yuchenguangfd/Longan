/*
 * evaluate_rating_delegate.cpp
 * Created on: Jan 14, 2015
 * Author: chenguangyu
 */

#include "evaluate_rating_delegate.h"
#include "recsys/base/rating_list.h"
#include "recsys/base/basic_predict.h"
#include "common/math/math.h"
#include "common/lang/integer.h"
#include "common/logging/logging.h"
#include "common/system/system_info.h"
#include "common/util/running_statistic.h"

namespace longan {

void SimpleEvaluateRatingDelegate::Evaluate(const BasicPredict *predict, const RatingList *testRatingList) {
    double sumAbs = 0.0;
    double sumSqr = 0.0;
    for (int i = 0; i < testRatingList->NumRating(); ++i) {
        const RatingRecord& rr = testRatingList->At(i);
        float predictedRating = predict->PredictRating(rr.UserId(), rr.ItemId());
        float trueRating = rr.Rating();
        float error = predictedRating - trueRating;
        sumAbs += Math::Abs(error);
        sumSqr += Math::Sqr(error);
    }
    mMAE = sumAbs / testRatingList->NumRating();
    mRMSE = Math::Sqrt(sumSqr / testRatingList->NumRating());
}

void DynamicScheduledEvaluateRatingDelegate::Evaluate(const BasicPredict *predict, const RatingList *testRatingList) {
    mPredict = predict;
    mTestRatingList = testRatingList;
    int numWorker = SystemInfo::GetNumCPUCore();
    mScheduler = new PipelinedScheduler<TaskBundle>(this, 1, numWorker, 1);
    mScheduler->Start();
    mScheduler->WaitFinish();
    delete mScheduler;
}

void DynamicScheduledEvaluateRatingDelegate::ProducerRun() {
    TaskBundle *currentBundle = new TaskBundle();
    currentBundle->reserve(TASK_BUNDLE_SIZE);
    for (int i = 0; i < mTestRatingList->NumRating(); ++i) {
        const RatingRecord& rr = mTestRatingList->At(i);
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

void DynamicScheduledEvaluateRatingDelegate::WorkerRun() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->WorkerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            task.predictedRating = mPredict->PredictRating(task.userId, task.itemId);
        }
        mScheduler->WorkerPutTask(currentBundle);
    }
    mScheduler->WorkerDone();
}

void DynamicScheduledEvaluateRatingDelegate::ConsumerRun() {
    int totoalTask = mTestRatingList->NumRating();
    int processedTask = 0;
    double sumAbs = 0.0;
    double sumSqr = 0.0;
    while (true) {
        TaskBundle *currentBundle = mScheduler->ConsumerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            float error = task.predictedRating - task.trueRating;
            sumAbs += Math::Abs(error);
            sumSqr += Math::Sqr(error);
        }
        processedTask += currentBundle->size();
        mProgress = static_cast<double>(processedTask)/totoalTask;
        delete currentBundle;
    }
    mMAE = sumAbs / mTestRatingList->NumRating();
    mRMSE = Math::Sqrt(sumSqr / mTestRatingList->NumRating());
    mScheduler->ConsumerDone();
}

void DynamicScheduledEvaluateRatingDelegate::MonitorRun() {
    while (true) {
        ConsoleLog::I("recsys", "Evaluate Rating..." + Integer::ToString((int)(mProgress*100)) + "% done.");
        if (mProgress > 0.99) break;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

} //~ namespace longan
