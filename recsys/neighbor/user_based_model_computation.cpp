/*
 * user_based_model_computation.cpp
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#include "user_based_model_computation.h"
#include "recsys/base/rating_adjust.h"
#include "common/math/math.h"
#include "common/lang/double.h"
#include "common/time/stopwatch.h"
#include "common/logging/logging.h"

namespace longan {

namespace UserBased {

void ModelComputation::AdjustRating() {
    if (mModel->GetParameter()->SimType() == Parameter::SimTypeAdjustedCosine) {
        AdjustRatingByMinusItemAverage(mTrainData);
    } else if (mModel->GetParameter()->SimType() == Parameter::SimTypeCorrelation) {
        AdjustRatingByMinusUserAverage(mTrainData);
    }
}

float ModelComputation::ComputeSimilarity(const UserVec& uv1, const UserVec& uv2) {
    int size1 = uv1.Size();
    if (size1 == 0) return 0.0f;
    int size2 = uv2.Size();
    if (size2 == 0) return 0.0f;
    const ItemRating* data1 = uv1.Data();
    const ItemRating* data2 = uv2.Data();
    double sum = 0.0;
    double norm1 = 0.0;
    double norm2 = 0.0;
    int i = 0, j = 0;
    int uid1 = data1[i].ItemId();
    int uid2 = data2[j].ItemId();
    double rating1 = data1[i].Rating();
    double rating2 = data2[j].Rating();
    while (true) {
        if (uid1 == uid2) {
            sum += rating1 * rating2;
            norm1 += rating1 * rating1;
            norm2 += rating2 * rating2;
            ++i; ++j;
            if (i == size1 || j == size2) break;
            uid1 = data1[i].ItemId();
            rating1 = data1[i].Rating();
            uid2 = data2[j].ItemId();
            rating2 = data2[j].Rating();
        } else if (uid1 < uid2) {
            ++i;
            if (i == size1) break;
            uid1 = data1[i].ItemId();
            rating1 = data1[i].Rating();
        } else if (uid1 > uid2) {
            ++j;
            if (j == size2) break;
            uid2 = data2[j].ItemId();
            rating2 = data2[j].Rating();
        }
    }
    double denominator = Math::Sqrt(norm1 * norm2);
    return (float)(sum / (denominator + Double::EPS));
}

void ModelComputationST::ComputeModel(const TrainOption *option, RatingMatUsers *trainData,
        ModelTrain *model) {
    mTrainOption = option;
    mTrainData = trainData;
    mModel = model;
    AdjustRating();
    for (int uid1 = 0; uid1 < mTrainData->NumUser(); ++uid1) {
        const UserVec& uv1 = trainData->GetUserVector(uid1);
        for (int uid2 = uid1 + 1; uid2 < mTrainData->NumUser(); ++uid2) {
            const UserVec& uv2 = trainData->GetUserVector(uid2);
            float sim = ComputeSimilarity(uv1, uv2);
            mModel->PutSimilarity(uid1, uid2, sim);
        }
    }
}

void ModelComputationMT::ComputeModel(const TrainOption *option, RatingMatUsers *trainData,
        ModelTrain *model) {
    mTrainOption = option;
    mTrainData = trainData;
    mModel = model;
    AdjustRating();
    mTotoalTask = (int64)(mTrainData->NumUser()-1) * mTrainData->NumUser() / 2;
    mProcessedTask = 0;
    mScheduler = new PipelinedScheduler<TaskBundle>(this, 1, mTrainOption->NumThread(), 1);
    mScheduler->Start();
    mScheduler->WaitFinish();
    delete mScheduler;
}

void ModelComputationMT::ProducerRun() {
    TaskBundle *currentBundle = new TaskBundle();
    currentBundle->reserve(TASK_BUNDLE_SIZE);
    for (int uid1 = 0; uid1 < mTrainData->NumUser(); ++uid1)  {
        for (int uid2 = uid1 + 1; uid2 < mTrainData->NumUser(); ++uid2) {
            if (currentBundle->size() == TASK_BUNDLE_SIZE) {
                mScheduler->ProducerPutTask(currentBundle);
                currentBundle = new TaskBundle();
                currentBundle->reserve(TASK_BUNDLE_SIZE);
            }
            Task task;
            task.uid1 = uid1;
            task.uid2 = uid2;
            currentBundle->push_back(task);
        }
    }
    mScheduler->ProducerPutTask(currentBundle);
    mScheduler->ProducerDone();
}

void ModelComputationMT::WorkerRun() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->WorkerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            const UserVec& uv1 = mTrainData->GetUserVector(task.uid1);
            const UserVec& uv2 = mTrainData->GetUserVector(task.uid2);
            task.sim = ComputeSimilarity(uv1, uv2);
        }
        mScheduler->WorkerPutTask(currentBundle);
    }
    mScheduler->WorkerDone();
}

void ModelComputationMT::ConsumerRun() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->ConsumerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            mModel->PutSimilarity(task.uid1, task.uid2, task.sim);
        }
        mProcessedTask += currentBundle->size();
        delete currentBundle;
    }
    mScheduler->ConsumerDone();
}

void ModelComputationMT::MonitorRun() {
    Stopwatch stopwatch;
    while (true) {
        int progress = static_cast<int>(100.0 * mProcessedTask / mTotoalTask);
        Log::Console("recsys", "computing user-user similarity...%d%%(%lld/%lld) done. total time=%.2lfs",
                progress, mProcessedTask, mTotoalTask, stopwatch.Toc());
        if (mProcessedTask >= mTotoalTask) break;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void ModelComputationMTStaticSchedule::ComputeModel(const TrainOption *option,
        RatingMatUsers *trainData, ModelTrain *model) {
    mTrainOption = option;
    mTrainData = trainData;
    mModel = model;
    AdjustRating();
    int numUser = mTrainData->NumUser();
    int numThread = mTrainOption->NumThread();
    int64 totalTask = (int64)numUser * (numUser - 1) / 2;
    int64 taskPerThread = totalTask / numThread;
    std::vector<std::thread> workers;
    for (int i = 0; i < numThread; ++i) {
        int64 begin = i * taskPerThread;
        int64 end = (i != numThread-1) ? (begin + taskPerThread) : totalTask;
        workers.push_back(std::thread(&ModelComputationMTStaticSchedule::ThreadRun, this, begin, end));
    }
    for (std::thread& t : workers) {
       t.join();
    }
}

void ModelComputationMTStaticSchedule::ThreadRun(int64 taskIdBegin, int64 taskIdEnd) {
    for (int64 taskId = taskIdBegin; taskId < taskIdEnd; ++taskId) {
        int uid1 = static_cast<int>((Math::Sqrt(8.0 * taskId + 1) + 1) / 2);
        int uid2 = taskId - (int64)uid1*(uid1-1)/2;
        const UserVec& uv1 = mTrainData->GetUserVector(uid1);
        const UserVec& uv2 = mTrainData->GetUserVector(uid2);
        float sim = ComputeSimilarity(uv1, uv2);
        mModel->PutSimilarity(uid1, uid2, sim);
    }
}

} //~ namespace UserBased

} //~ namespace longan
