/*
 * user_based_model_computation.cpp
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#include "user_based_model_computation.h"
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

namespace UserBased {

ModelComputation::ModelComputation() { }

ModelComputation::~ModelComputation() { }

float ModelComputation::ComputeSimilarity(const UserVector<>& firstUserVector, const UserVector<>& secondUserVector) {
    int size1 = firstUserVector.Size();
    if (size1 == 0) return 0.0f;
    int size2 = secondUserVector.Size();
    if (size2 == 0) return 0.0f;
    const ItemRating* data1 = firstUserVector.Data();
    const ItemRating* data2 = secondUserVector.Data();
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
    double denominator = sqrt(norm1 * norm2);
    return (float)(sum / (denominator + Double::EPS));
}

void SimpleModelComputation::ComputeModel(RatingMatrixAsUsers<> *ratingMatrix, ModelTrain *model) {
    int numUser = ratingMatrix->NumUser();
    for (int uid1 = 0; uid1 < numUser; ++uid1) {
        const auto& uv1 = ratingMatrix->GetUserVector(uid1);
        for (int uid2 = uid1 + 1; uid2 < numUser; ++uid2) {
            const auto& uv2 = ratingMatrix->GetUserVector(uid2);
            model->AddPairSimilarity(uid1, uid2, ComputeSimilarity(uv1, uv2));
        }
    }
}

void StaticScheduledModelComputation::ComputeModel(RatingMatrixAsUsers<> *ratingMatrix, ModelTrain *model) {
    mRatingMatrix = ratingMatrix;
    mModel = model;
    int numUser = mRatingMatrix->NumUser();
    mUpdateModelMutexs.clear();
    for (int i = 0; i < numUser; ++i) {
        mUpdateModelMutexs.push_back(new std::mutex);
    }
    int64 numTaskTotal = (int64)numUser * (numUser - 1) / 2;
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
    for (int i = 0; i < numUser; ++i) {
        delete mUpdateModelMutexs[i];
    }
}

void StaticScheduledModelComputation::DoWork(int64 taskIdBegin, int64 taskIdEnd) {
    for (int64 taskId = taskIdBegin; taskId < taskIdEnd; ++taskId) {
        int uid1 = static_cast<int>((sqrt(8.0*taskId+1)+1)/2);
        int uid2 = taskId - (int64)uid1*(uid1-1)/2;
        const auto& iv1 = mRatingMatrix->GetUserVector(uid1);
        const auto& iv2 = mRatingMatrix->GetUserVector(uid2);
        float similarity = ComputeSimilarity(iv1, iv2);
        mUpdateModelMutexs[uid1]->lock();
        mUpdateModelMutexs[uid2]->lock();
        mModel->AddPairSimilarity(uid1, uid2, similarity);
        mUpdateModelMutexs[uid2]->unlock();
        mUpdateModelMutexs[uid1]->unlock();
    }
}

DynamicScheduledModelComputation::Scheduler::Scheduler() {
    mNumGeneratePairWorker = 1;
    mNumComputeSimilarityWorker = SystemInfo::GetNumCPUCore();
    mNumUpdateModelWorker = 1;
    mObjectBuffer1 = new ObjectBuffer<TaskBundle*>("GeneratePair-ComputeSimilarity", BUFFER_SIZE,
            mNumGeneratePairWorker, mNumComputeSimilarityWorker);
    mObjectBuffer2 = new ObjectBuffer<TaskBundle*>("ComputeSimilarity-UpdateModel", BUFFER_SIZE,
            mNumComputeSimilarityWorker, mNumUpdateModelWorker);
}

DynamicScheduledModelComputation::Scheduler::~Scheduler() {
    delete mObjectBuffer2;
    delete mObjectBuffer1;
}

void DynamicScheduledModelComputation::Scheduler::PutComputeSimilarityWork(
    DynamicScheduledModelComputation::TaskBundle* bundle) {
    int rtn = mObjectBuffer1->Push(bundle);
    assert(rtn == LONGAN_SUCC);
}

DynamicScheduledModelComputation::TaskBundle* DynamicScheduledModelComputation::
    Scheduler::GetComputeSimilarityWork() {
    TaskBundle* bundle;
    int rtn = mObjectBuffer1->Pop(bundle);
    return (rtn == LONGAN_SUCC) ? bundle : nullptr;
}

void DynamicScheduledModelComputation::Scheduler::PutUpdateModelWork(
    DynamicScheduledModelComputation::TaskBundle* bundle) {
    int rtn = mObjectBuffer2->Push(bundle);
    assert(rtn == LONGAN_SUCC);
}

DynamicScheduledModelComputation::TaskBundle* DynamicScheduledModelComputation::
    Scheduler::GetUpdateModelWork() {
    TaskBundle* bundle;
    int rtn = mObjectBuffer2->Pop(bundle);
    return (rtn == LONGAN_SUCC) ? bundle : nullptr;
}

void DynamicScheduledModelComputation::Scheduler::GeneratePairDone() {
    int rtn = mObjectBuffer1->ProducerQuit();
    assert(rtn == LONGAN_SUCC);
}

void DynamicScheduledModelComputation::Scheduler::ComputeSimilarityDone() {
    int rtn1 = mObjectBuffer1->ConsumerQuit();
    assert(rtn1 == LONGAN_SUCC);
    int rtn2 = mObjectBuffer2->ProducerQuit();
    assert(rtn2 == LONGAN_SUCC);
}

void DynamicScheduledModelComputation::Scheduler::UpdateModelDone() {
    int rtn = mObjectBuffer2->ConsumerQuit();
    assert(rtn == LONGAN_SUCC);
}

void DynamicScheduledModelComputation::ComputeModel(RatingMatrixAsUsers<> *ratingMatrix, ModelTrain *model) {
    mRatingMatrix = ratingMatrix;
    mModel = model;
    mScheduler = new Scheduler();
    std::vector<std::thread> workers;
    for (int i = 0; i < mScheduler->NumGeneratePairWorker(); ++i) {
        workers.push_back(std::thread(&DynamicScheduledModelComputation::DoGeneratePairWork, this));
    }
    for (int i = 0; i < mScheduler->NumComputeSimilarityWorker(); ++i) {
        workers.push_back(std::thread(&DynamicScheduledModelComputation::DoComputeSimilarityWork,this));
    }
    for (int i = 0; i < mScheduler->NumUpdateModelWorker(); ++i) {
        workers.push_back(std::thread(&DynamicScheduledModelComputation::DoUpdateModelWork, this));
    }
    workers.push_back(std::thread(&DynamicScheduledModelComputation::DoMonitorProgress, this));
    for (std::thread& t : workers) {
        t.join();
    }
    delete mScheduler;
}

void DynamicScheduledModelComputation::DoGeneratePairWork() {
    int numUser = mRatingMatrix->NumUser();
    TaskBundle *currentBundle = new TaskBundle();
    currentBundle->reserve(TASK_BUNDLE_SIZE);
    for (int firstUserId = 0; firstUserId < numUser; ++firstUserId)  {
        for (int secondUserId = firstUserId + 1; secondUserId < numUser; ++secondUserId) {
            if (currentBundle->size() == TASK_BUNDLE_SIZE) {
                mScheduler->PutComputeSimilarityWork(currentBundle);
                currentBundle = new TaskBundle();
                currentBundle->reserve(TASK_BUNDLE_SIZE);
            }
            currentBundle->push_back(Task(firstUserId, secondUserId));
        }
    }
    mScheduler->PutComputeSimilarityWork(currentBundle);
    mScheduler->GeneratePairDone();
}

void DynamicScheduledModelComputation::DoComputeSimilarityWork() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->GetComputeSimilarityWork();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            auto& iv1 = mRatingMatrix->GetUserVector(task.firstUserId);
            auto& iv2 = mRatingMatrix->GetUserVector(task.secondUserId);
            task.similarity = ComputeSimilarity(iv1, iv2);
        }
        mScheduler->PutUpdateModelWork(currentBundle);
    }
    mScheduler->ComputeSimilarityDone();
}

void DynamicScheduledModelComputation::DoUpdateModelWork() {
    int64 totoalTask = static_cast<int64>(mRatingMatrix->NumUser())*mRatingMatrix->NumUser()/2;
    int64 processedTask = 0;
    while (true) {
        TaskBundle *currentBundle = mScheduler->GetUpdateModelWork();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            mModel->AddPairSimilarity(task.firstUserId, task.secondUserId, task.similarity);
        }
        processedTask += currentBundle->size();
        mProgress = static_cast<double>(processedTask)/totoalTask;
        delete currentBundle;
    }
    mScheduler->UpdateModelDone();
}

void DynamicScheduledModelComputation::DoMonitorProgress() {
    while (true) {
        ConsoleLog::I("recsys", "Computing Model..." + Integer::ToString((int)(mProgress*100)) + "% done.");
        if (mProgress > 0.99) break;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

} //~ namespace UserBased

} //~ namespace longan
