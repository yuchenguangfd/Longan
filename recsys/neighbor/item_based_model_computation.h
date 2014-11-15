/*
 * item_based_mdoel_computation.h
 * Created on: Nov 9, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_ITEM_BASED_MODEL_COMPUTATION_H
#define RECSYS_NEIGHBOR_ITEM_BASED_MODEL_COMPUTATION_H

#include "item_based_model.h"
#include "recsys/base/rating_matrix_as_items.h"
#include "common/threading/object_buffer.h"
#include <mutex>

namespace longan {

class ItemBasedTrain;

namespace item_based {

class ModelComputation {
public:
    ModelComputation();
    virtual ~ModelComputation();
    virtual void ComputeModel(RatingMatrixAsItems<> *ratingMatrix, ModelTrain *model) = 0;
protected:
    float ComputeSimilarity(const ItemVector<>& firstItemVector, const ItemVector<>& secondItemVector);
};

class SimpleModelComputation : public ModelComputation {
public:
    virtual void ComputeModel(RatingMatrixAsItems<> *ratingMatrix, ModelTrain *model);
};

class StaticScheduledModelComputation : public ModelComputation {
public:
    virtual void ComputeModel(RatingMatrixAsItems<> *ratingMatrix, ModelTrain *model);
protected:
    void DoWork(int taskIdBegin, int taskIdEnd);
protected:
    RatingMatrixAsItems<> *mRatingMatrix;
    ModelTrain *mModel;
    std::vector<std::mutex*> mUpdateModelMutexs;
    std::mutex mtx;
};

class DynamicScheduledModelComputation : public ModelComputation {
public:
    virtual void ComputeModel(RatingMatrixAsItems<> *ratingMatrix, ModelTrain *model);
protected:
    void DoGeneratePairWork();
    void DoComputeSimilarityWork();
    void DoUpdateModelWork();
protected:
    struct Task {
        int firstItemId;
        int secondItemId;
        float similarity;
        Task(int iid1, int iid2) :
            firstItemId(iid1),
            secondItemId(iid2),
            similarity(0.0f) { }
    };
    static const int TASK_BUNDLE_SIZE = 16384;
    typedef std::vector<Task> TaskBundle;

    class Scheduler {
    public:
        Scheduler();
        ~Scheduler();
        void PutComputeSimilarityWork(TaskBundle* bundle);
        TaskBundle* GetComputeSimilarityWork();
        void PutUpdateModelWork(TaskBundle* bundle);
        TaskBundle* GetUpdateModelWork();
        void GeneratePairDone();
        void ComputeSimilarityDone();
        void UpdateModelDone();
        int NumGeneratePairWorker() const {
            return mNumGeneratePairWorker;
        }
        int NumComputeSimilarityWorker() const {
            return mNumComputeSimilarityWorker;
        }
        int NumUpdateModelWorker() const {
            return mNumUpdateModelWorker;
        }
    private:
        static const int BUFFER_SIZE = 100;
        int mNumGeneratePairWorker;
        int mNumComputeSimilarityWorker;
        int mNumUpdateModelWorker;
        ObjectBuffer<TaskBundle*> *mObjectBuffer1;
        ObjectBuffer<TaskBundle*> *mObjectBuffer2;
    };
protected:
    RatingMatrixAsItems<> *mRatingMatrix;
    ModelTrain *mModel;
    Scheduler *mScheduler;
};

} //~ namespace item_based

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_MODEL_COMPUTATION_H */
