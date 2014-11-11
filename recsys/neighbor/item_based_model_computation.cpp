/*
 * item_based_model_computation.cpp
 * Created on: Nov 9, 2014
 * Author: chenguangyu
 */

#include "item_based_model_computation.h"
#include "common/lang/double.h"
#include "common/system/system_info.h"
#include "common/error.h"
#include <thread>
#include <vector>
#include <cmath>
#include <cassert>

namespace longan {

namespace item_based {

ModelComputation::ModelComputation() { }

ModelComputation::~ModelComputation() { }

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
    if (Double::DblZero(denominator)) {
        return 0.0f;
    }
    return (float)(sum / denominator);
}

void SimpleModelComputation::ComputeModel(RatingMatrixAsItems<> *ratingMatrix, Model *model) {
    int numItem = ratingMatrix->NumItem();
    for (int i = 0; i < numItem; ++i) {
        const auto& iv1 = ratingMatrix->GetItemVector(i);
        for (int j = i + 1; j < numItem; ++j) {
            const auto& iv2 = ratingMatrix->GetItemVector(j);
            model->Update(i, j, ComputeSimilarity(iv1, iv2));
        }
    }
}

void StaticScheduledModelComputation::ComputeModel(RatingMatrixAsItems<> *ratingMatrix, Model *model) {
    mRatingMatrix = ratingMatrix;
    mModel = model;
    int numItem = mRatingMatrix->NumItem();
    mUpdateModelMutexs.clear();
    for (int i = 0; i < numItem; ++i) {
        mUpdateModelMutexs.push_back(new std::mutex);
    }
    int numTaskTotal = numItem * (numItem - 1) / 2;
    int numThread = SystemInfo::GetNumCPUCore();
    int numTaskPerThread = numTaskTotal / numThread;
    std::vector<std::thread> workers;
    for (int i = 0; i < numThread; ++i) {
        int begin = i * numTaskPerThread;
        int end = (i != numThread-1)? (begin + numTaskPerThread) : numTaskTotal;
        workers.push_back(std::thread(&StaticScheduledModelComputation::DoWork, this, begin, end));
    }
    for (std::thread& t : workers) {
       t.join();
    }
    for (int i = 0; i < numItem; ++i) {
        delete mUpdateModelMutexs[i];
    }
}

void StaticScheduledModelComputation::DoWork(int taskIdBegin, int taskIdEnd) {
    for (int taskId = taskIdBegin; taskId < taskIdEnd; ++taskId) {
        int iid1 = static_cast<int>((sqrt(8.0*taskId+1)+1)/2);
        int iid2 = taskId - iid1*(iid1-1)/2;
        const auto& iv1 = mRatingMatrix->GetItemVector(iid1);
        const auto& iv2 = mRatingMatrix->GetItemVector(iid2);
        float similarity = ComputeSimilarity(iv1, iv2);
        mUpdateModelMutexs[iid1]->lock();
        mUpdateModelMutexs[iid2]->lock();
        mModel->Update(iid1, iid2, similarity);
        mUpdateModelMutexs[iid2]->unlock();
        mUpdateModelMutexs[iid1]->unlock();
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

void DynamicScheduledModelComputation::ComputeModel(RatingMatrixAsItems<> *ratingMatrix, Model *model) {
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
    for (std::thread& t : workers) {
        t.join();
    }
    delete mScheduler;
}

void DynamicScheduledModelComputation::DoGeneratePairWork() {
    int numItem = mRatingMatrix->NumItem();
    TaskBundle *currentBundle = new TaskBundle();
    currentBundle->reserve(TASK_BUNDLE_SIZE);
    for (int firstItemId = 0; firstItemId < numItem; ++firstItemId)  {
        for (int secondItemId = firstItemId + 1; secondItemId < numItem; ++secondItemId) {
            if (currentBundle->size() == TASK_BUNDLE_SIZE) {
                mScheduler->PutComputeSimilarityWork(currentBundle);
                currentBundle = new TaskBundle();
                currentBundle->reserve(TASK_BUNDLE_SIZE);
            }
            currentBundle->push_back(Task(firstItemId, secondItemId));
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
            auto& iv1 = mRatingMatrix->GetItemVector(task.firstItemId);
            auto& iv2 = mRatingMatrix->GetItemVector(task.secondItemId);
            task.similarity = ComputeSimilarity(iv1, iv2);
        }
        mScheduler->PutUpdateModelWork(currentBundle);
    }
    mScheduler->ComputeSimilarityDone();
}

void DynamicScheduledModelComputation::DoUpdateModelWork() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->GetUpdateModelWork();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            mModel->Update(task.firstItemId, task.secondItemId, task.similarity);
        }
        delete currentBundle;
    }
    mScheduler->UpdateModelDone();
}

} //~ namespace item_based

} //~ namespace longan
