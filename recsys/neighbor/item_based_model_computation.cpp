/*
 * item_based_model_computation.cpp
 * Created on: Nov 9, 2014
 * Author: chenguangyu
 */

#include "item_based_model_computation.h"
#include "recsys/base/rating_adjust.h"
#include "common/math/math.h"
#include "common/lang/double.h"
#include "common/time/stopwatch.h"
#include "common/logging/logging.h"

namespace longan {

namespace ItemBased {

void ModelComputation::AdjustRating() {
    if (mModel->mParameter->SimType() == Parameter::SimTypeAdjustedCosine) {
        AdjustRatingByMinusUserAverage(mTrainData);
    } else if (mModel->mParameter->SimType() == Parameter::SimTypeCorrelation) {
        AdjustRatingByMinusItemAverage(mTrainData);
    }
}

float ModelComputation::ComputeSimilarity(const ItemVec& iv1, const ItemVec& iv2) {
    int size1 = iv1.Size();
    if (size1 == 0) return 0.0f;
    int size2 = iv2.Size();
    if (size2 == 0) return 0.0f;
    const UserRating* data1 = iv1.Data();
    const UserRating* data2 = iv2.Data();
    double sum = 0.0;
    double norm1 = 0.0;
    double norm2 = 0.0;
    int i = 0, j = 0;
    int uid1 = data1[i].UserId();
    int uid2 = data2[j].UserId();
    double rating1 = data1[i].Rating();
    double rating2 = data2[j].Rating();
    while (true) {
        if (uid1 == uid2) {
            sum += rating1 * rating2;
            norm1 += rating1 * rating1;
            norm2 += rating2 * rating2;
            ++i; ++j;
            if (i == size1 || j == size2) break;
            uid1 = data1[i].UserId();
            rating1 = data1[i].Rating();
            uid2 = data2[j].UserId();
            rating2 = data2[j].Rating();
        } else if (uid1 < uid2) {
            ++i;
            if (i == size1) break;
            uid1 = data1[i].UserId();
            rating1 = data1[i].Rating();
        } else if (uid1 > uid2) {
            ++j;
            if (j == size2) break;
            uid2 = data2[j].UserId();
            rating2 = data2[j].Rating();
        }
    }
    double denominator = Math::Sqrt(norm1 * norm2);
    return (float)(sum / (denominator + Double::EPS));
}

void ModelComputationST::ComputeModel(const TrainOption *option, RatingMatItems *trainData,
        ModelTrain *model) {
    mTrainOption = option;
    mTrainData = trainData;
    mModel = model;
    AdjustRating();
    for (int iid1 = 0; iid1 < mTrainData->NumItem(); ++iid1) {
        const ItemVec& iv1 = mTrainData->GetItemVector(iid1);
        for (int iid2 = iid1 + 1; iid2 < mTrainData->NumItem(); ++iid2) {
            const ItemVec& iv2 = mTrainData->GetItemVector(iid2);
            float sim = ComputeSimilarity(iv1, iv2);
            mModel->mSimMat[iid2][iid1] = sim;
        }
    }
}

void ModelComputationMT::ComputeModel(const TrainOption *option, RatingMatItems *trainData,
        ModelTrain *model) {
    mTrainOption = option;
    mTrainData = trainData;
    mModel = model;
    AdjustRating();
    mTotoalTask = (int64)(mTrainData->NumItem()) * mTrainData->NumItem() / 2;
    mProcessedTask = 0;
    mScheduler = new PipelinedScheduler<TaskBundle>(this, 1, mTrainOption->NumThread(), 1);
    mScheduler->Start();
    mScheduler->WaitFinish();
    delete mScheduler;
}

void ModelComputationMT::ProducerRun() {
    TaskBundle *currentBundle = new TaskBundle();
    currentBundle->reserve(TASK_BUNDLE_SIZE);
    for (int iid1 = 0; iid1 < mTrainData->NumItem(); ++iid1)  {
        for (int iid2 = iid1 + 1; iid2 < mTrainData->NumItem(); ++iid2) {
            if (currentBundle->size() == TASK_BUNDLE_SIZE) {
                mScheduler->ProducerPutTask(currentBundle);
                currentBundle = new TaskBundle();
                currentBundle->reserve(TASK_BUNDLE_SIZE);
            }
            Task task;
            task.iid1 = iid1;
            task.iid2 = iid2;
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
            const ItemVec& iv1 = mTrainData->GetItemVector(task.iid1);
            const ItemVec& iv2 = mTrainData->GetItemVector(task.iid2);
            task.sim = ComputeSimilarity(iv1, iv2);
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
            mModel->mSimMat[task.iid2][task.iid1] = task.sim;
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
        Log::Console("recsys", "computing item-item similarity...%d%%(%d/%d) done. total time=%.2lfs",
                progress, mProcessedTask, mTotoalTask, stopwatch.Toc());
        if (mProcessedTask >= mTotoalTask) break;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void ModelComputationMTStaticSchedule::ComputeModel(const TrainOption *option,
        RatingMatItems *trainData, ModelTrain *model) {
    mTrainOption = option;
    mTrainData = trainData;
    mModel = model;
    AdjustRating();
    int numItem = mTrainData->NumItem();
    int numThread = mTrainOption->NumThread();
    int64 totalTask = (int64)numItem * (numItem - 1) / 2;
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
        int iid1 = static_cast<int>((Math::Sqrt(8.0 * taskId + 1) + 1) / 2);
        int iid2 = taskId - iid1*(iid1-1)/2;
        const ItemVec& iv1 = mTrainData->GetItemVector(iid1);
        const ItemVec& iv2 = mTrainData->GetItemVector(iid2);
        float sim = ComputeSimilarity(iv1, iv2);
        mModel->PutSimilarity(iid1, iid2, sim);
    }
}

} //~ namespace ItemBased

} //~ namespace longan
