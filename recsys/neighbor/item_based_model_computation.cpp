/*
 * item_based_model_computation.cpp
 * Created on: Nov 9, 2014
 * Author: chenguangyu
 */

#include "item_based_model_computation.h"
#include "common/lang/types.h"
#include "common/lang/double.h"
#include "common/lang/integer.h"
#include "common/system/system_info.h"
#include "common/util/string_helper.h"
#include "common/logging/logging.h"
#include "common/error.h"
#include <thread>
#include <vector>
#include <cmath>
#include <cassert>

namespace longan {

namespace ItemBased {

float ModelComputation::ComputeSimilarity(const ItemVector<>& firstItemVector, const ItemVector<>& secondItemVector) {
    int size1 = firstItemVector.Size();
    if (size1 == 0) return 0.0f;
    int size2 = secondItemVector.Size();
    if (size2 == 0) return 0.0f;
    const UserRating* data1 = firstItemVector.Data();
    const UserRating* data2 = secondItemVector.Data();
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
    double denominator = sqrt(norm1 * norm2);
    return (float)(sum / (denominator + Double::EPS));
}

void SimpleModelComputation::ComputeModel(RatingMatrixAsItems<> *ratingMatrix, ModelTrain *model) {
    int numItem = ratingMatrix->NumItem();
    for (int iid1 = 0; iid1 < numItem; ++iid1) {
        const auto& iv1 = ratingMatrix->GetItemVector(iid1);
        for (int iid2 = iid1 + 1; iid2 < numItem; ++iid2) {
            const auto& iv2 = ratingMatrix->GetItemVector(iid2);
            model->AddPairSimilarity(iid1, iid2, ComputeSimilarity(iv1, iv2));
        }
    }
}

void StaticScheduledModelComputation::ComputeModel(RatingMatrixAsItems<> *ratingMatrix, ModelTrain *model) {
    mRatingMatrix = ratingMatrix;
    mModel = model;
    int numItem = mRatingMatrix->NumItem();
    mUpdateModelMutexs.clear();
    for (int i = 0; i < numItem; ++i) {
        mUpdateModelMutexs.push_back(new std::mutex());
    }
    int64 numTaskTotal = (int64)numItem * (numItem - 1) / 2;
    int numThread = SystemInfo::GetNumCPUCore();
    int64 numTaskPerThread = numTaskTotal / numThread;
    std::vector<std::thread> workers;
    for (int i = 0; i < numThread; ++i) {
        int64 begin = i * numTaskPerThread;
        int64 end = (i != numThread-1)? (begin + numTaskPerThread) : numTaskTotal;
        workers.push_back(std::thread(&StaticScheduledModelComputation::DoWork, this, begin, end));
    }
    for (std::thread& t : workers) {
       t.join();
    }
    for (int i = 0; i < numItem; ++i) {
        delete mUpdateModelMutexs[i];
    }
}

void StaticScheduledModelComputation::DoWork(int64 taskIdBegin, int64 taskIdEnd) {
    for (int64 taskId = taskIdBegin; taskId < taskIdEnd; ++taskId) {
        int iid1 = static_cast<int>((sqrt(8.0*taskId+1)+1)/2);
        int iid2 = taskId - iid1*(iid1-1)/2;
        const auto& iv1 = mRatingMatrix->GetItemVector(iid1);
        const auto& iv2 = mRatingMatrix->GetItemVector(iid2);
        float similarity = ComputeSimilarity(iv1, iv2);
        mUpdateModelMutexs[iid1]->lock();
        mUpdateModelMutexs[iid2]->lock();
        mModel->AddPairSimilarity(iid1, iid2, similarity);
        mUpdateModelMutexs[iid2]->unlock();
        mUpdateModelMutexs[iid1]->unlock();
    }
}

void DynamicScheduledModelComputation::ComputeModel(RatingMatrixAsItems<> *ratingMatrix, ModelTrain *model) {
    mRatingMatrix = ratingMatrix;
    mModel = model;
    mScheduler = new PipelinedScheduler<TaskBundle>(this, 1, mNumThread, 1);
    mScheduler->Start();
    mScheduler->WaitFinish();
    delete mScheduler;
}

void DynamicScheduledModelComputation::ProducerRun() {
    int numItem = mRatingMatrix->NumItem();
    TaskBundle *currentBundle = new TaskBundle();
    currentBundle->reserve(TASK_BUNDLE_SIZE);
    for (int firstItemId = 0; firstItemId < numItem; ++firstItemId)  {
        for (int secondItemId = firstItemId + 1; secondItemId < numItem; ++secondItemId) {
            if (currentBundle->size() == TASK_BUNDLE_SIZE) {
                mScheduler->ProducerPutTask(currentBundle);
                currentBundle = new TaskBundle();
                currentBundle->reserve(TASK_BUNDLE_SIZE);
            }
            currentBundle->push_back(Task(firstItemId, secondItemId));
        }
    }
    mScheduler->ProducerPutTask(currentBundle);
    mScheduler->ProducerDone();
}

void DynamicScheduledModelComputation::WorkerRun() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->WorkerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            const auto& iv1 = mRatingMatrix->GetItemVector(task.firstItemId);
            const auto& iv2 = mRatingMatrix->GetItemVector(task.secondItemId);
            task.similarity = ComputeSimilarity(iv1, iv2);
        }
        mScheduler->WorkerPutTask(currentBundle);
    }
    mScheduler->WorkerDone();
}

void DynamicScheduledModelComputation::ConsumerRun() {
    int64 totoalTask = static_cast<int64>(mRatingMatrix->NumItem())*mRatingMatrix->NumItem()/2;
    int64 processedTask = 0;
    while (true) {
        TaskBundle *currentBundle = mScheduler->ConsumerGetTask();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            mModel->AddPairSimilarity(task.firstItemId, task.secondItemId, task.similarity);
        }
        processedTask += currentBundle->size();
        mCurrentProgress = static_cast<double>(processedTask)/totoalTask;
        delete currentBundle;
    }
    mScheduler->ConsumerDone();
}

void DynamicScheduledModelComputation::MonitorRun() {
    while (true) {
        ConsoleLog::I("recsys", "Computing Model..."
                + Integer::ToString((int)(mCurrentProgress*100)) + "% done.");
        if (mCurrentProgress > 0.99) break;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

} //~ namespace ItemBased

} //~ namespace longan
