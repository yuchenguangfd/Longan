/*
 * user_based_mdoel_computation.h
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_USER_BASED_MODEL_COMPUTATION_H
#define RECSYS_NEIGHBOR_USER_BASED_MODEL_COMPUTATION_H

#include "user_based_model.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "common/threading/object_buffer.h"
#include "common/lang/types.h"

namespace longan {

namespace user_based {

class ModelComputation {
public:
    ModelComputation();
    virtual ~ModelComputation();
    virtual void ComputeModel(RatingMatrixAsUsers<> *ratingMatrix, ModelTrain *model) = 0;
protected:
    float ComputeSimilarity(const UserVector<>& firstUserVector, const UserVector<>& secondUserVector);
};

class SimpleModelComputation : public ModelComputation {
public:
    virtual void ComputeModel(RatingMatrixAsUsers<> *ratingMatrix, ModelTrain *model);
};

class StaticScheduledModelComputation : public ModelComputation {
public:
    virtual void ComputeModel(RatingMatrixAsUsers<> *ratingMatrix, ModelTrain *model);
protected:
    void DoWork(int64 taskIdBegin, int64 taskIdEnd);
protected:
    RatingMatrixAsUsers<> *mRatingMatrix;
    ModelTrain *mModel;
    std::vector<std::mutex*> mUpdateModelMutexs;
};

class DynamicScheduledModelComputation : public ModelComputation {
public:
    virtual void ComputeModel(RatingMatrixAsUsers<> *ratingMatrix, ModelTrain *model);
protected:
    void DoGeneratePairWork();
    void DoComputeSimilarityWork();
    void DoUpdateModelWork();
protected:
    struct Task {
        int firstUserId;
        int secondUserId;
        float similarity;
        Task(int uid1, int uid2) :
            firstUserId(uid1),
            secondUserId(uid2),
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
    RatingMatrixAsUsers<> *mRatingMatrix;
    ModelTrain *mModel;
    Scheduler *mScheduler;
};

} //~ namespace user_based

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_MODEL_COMPUTATION_H */
