/*
 * evaluate_ranking_delegate.cpp
 * Created on: Jan 14, 2015
 * Author: chenguangyu
 */

#include "evaluate_ranking_delegate.h"
#include "common/base/algorithm.h"
#include "recsys/base/rating_list.h"
#include "recsys/base/basic_predict.h"
#include "common/math/math.h"
#include "common/lang/integer.h"
#include "common/logging/logging.h"
#include "common/system/system_info.h"
#include "common/util/running_statistic.h"
#include <cassert>

namespace longan {

void EvaluateRankingDelegate::EvaluateOneUser(int userId, const ItemRating* truthDataBegin, int truthDataSize,
        double* precision, double* recall) {
    ItemIdList recommendedItemList = mPredict->PredictTopNItem(userId, mRankingListSize);
    int hitCount = 0;
    for (int i = 0; i < recommendedItemList.size(); ++i) {
        int iid = recommendedItemList[i];
        int pos = BSearch(iid, truthDataBegin, truthDataSize,
                [](int lhs, const ItemRating& rhs)->int {
                    return lhs - rhs.ItemId();
        });
        if (pos >= 0) {
            ++hitCount;
        }
    }
    *precision = static_cast<double>(hitCount) / mRankingListSize;
    *recall = (truthDataSize == 0) ? 0.0 : static_cast<double>(hitCount) / truthDataSize;
}

void SimpleEvaluateRankingDelegate::Evaluate(const BasicPredict *predict, const RatingMatrixAsUsers<> *rmat, int listSize) {
    assert(listSize > 0);
    mPredict = predict;
    mTestRatingMatrix = rmat;
    mRankingListSize = listSize;
    RunningAverage<double> raPrecision;
    RunningAverage<double> raRecall;
    for (int userId = 0; userId < mTestRatingMatrix->NumUser(); ++userId) {
        const auto& uv = mTestRatingMatrix->GetUserVector(userId);
        double userPrecision, userRecall;
        EvaluateOneUser(userId, uv.Data(), uv.Size(), &userPrecision, &userRecall);
        raPrecision.Add(userPrecision);
        raRecall.Add(userRecall);
    }
    mPrecision = raPrecision.CurrentAverage();
    mRecall = raRecall.CurrentAverage();
    mF1Score = 2.0 * (mPrecision * mRecall)/(mPrecision + mRecall);
}

void DynamicScheduledEvaluateRankingDelegate::Evaluate(const BasicPredict *predict, const RatingMatrixAsUsers<> *rmat, int listSize) {
    assert(listSize > 0);
    mPredict = predict;
    mTestRatingMatrix = rmat;
    mRankingListSize = listSize;
    int numWorker = SystemInfo::GetNumCPUCore();
    mScheduler = new PipelinedScheduler<TaskBundle>(this, 1, numWorker, 1);
    mScheduler->Start();
    mScheduler->WaitFinish();
    delete mScheduler;
}

void DynamicScheduledEvaluateRankingDelegate::ProducerRun() {
    TaskBundle *currentBundle = new TaskBundle();
    currentBundle->reserve(TASK_BUNDLE_SIZE);
    for (int uid = 0; uid < mTestRatingMatrix->NumUser(); ++uid) {
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

void DynamicScheduledEvaluateRankingDelegate::WorkerRun() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->WorkerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            const auto& uv = mTestRatingMatrix->GetUserVector(task.userId);
            EvaluateOneUser(task.userId, uv.Data(), uv.Size(), &task.precision, &task.recall);
        }
        mScheduler->WorkerPutTask(currentBundle);
    }
    mScheduler->WorkerDone();
}

void DynamicScheduledEvaluateRankingDelegate::ConsumerRun() {
    int totoalTask = mTestRatingMatrix->NumUser();
    int processedTask = 0;
    RunningAverage<double> raPrecision;
    RunningAverage<double> raRecall;
    while (true) {
        TaskBundle *currentBundle = mScheduler->ConsumerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            raPrecision.Add(task.precision);
            raRecall.Add(task.recall);
        }
        processedTask += currentBundle->size();
        mProgress = static_cast<double>(processedTask)/totoalTask;
        delete currentBundle;
    }
    mScheduler->ConsumerDone();
    mPrecision = raPrecision.CurrentAverage();
    mRecall = raRecall.CurrentAverage();
    mF1Score = 2.0 * (mPrecision * mRecall)/(mPrecision + mRecall);
}

void DynamicScheduledEvaluateRankingDelegate::MonitorRun() {
    while (true) {
        Log::Console("recsys", "Evaluate Ranking...%d%% done.", (int)(mProgress*100));
        if (mProgress > 0.99) break;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

} //~ namespace longan
