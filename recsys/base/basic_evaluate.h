/*
 * basic_evaluate.h
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_BASIC_EVALUATE_H
#define RECSYS_BASE_BASIC_EVALUATE_H

#include "basic_predict.h"
#include "rating_list.h"
#include "rating_matrix_as_users.h"
#include "common/lang/defines.h"
#include "common/threading/object_buffer.h"
#include <json/json.h>
#include <string>

namespace longan {

class EvaluateRatingDelegate;
class EvaluateRankingDelegate;

class BasicEvaluate {
public:
    BasicEvaluate(const std::string& ratingTrainFilepath, const std::string& configFilepath,
            const std::string& modelFilepath, const std::string& ratingTestFilepath,
            const std::string& resultFilepath);
    virtual ~BasicEvaluate();
    virtual void Evaluate() final;
protected:
    virtual void CreatePredict() = 0;
    virtual void LoadConfig() final;
    virtual void LoadTestRatings();
    virtual void EvaluateRating();
    virtual void EvaluateRanking();
    virtual void WriteResult() final;
    virtual void DestroyPredict() final;
protected:
    const std::string mRatingTrainFilepath;
    const std::string mConfigFilepath;
    const std::string mModelFilepath;
    const std::string mRatingTestFilepath;
    const std::string mResultFilepath;
    Json::Value mConfig;
    RatingList mTestRatingList;
    BasicPredict *mPredict;
    EvaluateRatingDelegate *mEvaluateRatingDelegate;
    EvaluateRankingDelegate *mEvaluateRankingDelegate;
    double mMAE;
    double mRMSE;
    double mPrecision;
    double mRecall;
    double mF1Score;
    DISALLOW_COPY_AND_ASSIGN(BasicEvaluate);
};

class EvaluateRatingDelegate {
public:
    EvaluateRatingDelegate(const BasicPredict *predict, const RatingList *rlist);
    virtual ~EvaluateRatingDelegate();
    virtual void Evaluate() = 0;
    double MAE() const { return mMAE; }
    double RMSE() const { return mRMSE; }
protected:
    const BasicPredict *mPredict;
    const RatingList *mTestRatingList;
    double mMAE;
    double mRMSE;
};

class SimpleEvaluateRatingDelegate : public EvaluateRatingDelegate {
public:
    using EvaluateRatingDelegate::EvaluateRatingDelegate;
    virtual void Evaluate() override;
};

class DynamicScheduledEvaluateRatingDelegate : public EvaluateRatingDelegate {
public:
    using EvaluateRatingDelegate::EvaluateRatingDelegate;
    virtual void Evaluate() override;
private:
    void DoGenerateTaskWork();
    void DoPredictRatingWork();
    void DoCalcuateErrorWork();
    void DoMonitorProgress();
    struct Task {
        int userId;
        int itemId;
        float trueRating;
        float predictedRating;
        Task(int uid, int iid, float rating): userId(uid), itemId(iid),
                trueRating(rating), predictedRating(0.0f) { }
    };
    static const int TASK_BUNDLE_SIZE = 4096;
    typedef std::vector<Task> TaskBundle;
    class Scheduler {
    public:
        Scheduler();
        ~Scheduler();
        void PutPredictRatingWork(TaskBundle* bundle);
        TaskBundle* GetPredictRatingWork();
        void PutCalcuateErrorWork(TaskBundle* bundle);
        TaskBundle* GetCalcuateErrorWork();
        void GenerateTaskDone();
        void PredictRatingDone();
        void CalcuateErrorDone();
        int NumGenerateTaskWorker() const { return mNumGenerateTaskWorker; }
        int NumPredictRatingWorker() const { return mNumPredictRatingWorker; }
        int NumCalcuateErrorWorker() const { return mNumCalcuateErrorWorker; }
    private:
        static const int BUFFER_SIZE = 100;
        int mNumGenerateTaskWorker;
        int mNumPredictRatingWorker;
        int mNumCalcuateErrorWorker;
        ObjectBuffer<TaskBundle*> *mObjectBuffer1;
        ObjectBuffer<TaskBundle*> *mObjectBuffer2;
    };
private:
    Scheduler *mScheduler;
    double mProgress;
};

class EvaluateRankingDelegate {
public:
    EvaluateRankingDelegate(const BasicPredict *predict, const RatingMatrixAsUsers<> *rmat, int listSize);
    virtual ~EvaluateRankingDelegate();
    virtual void Evaluate() = 0;
    double Precision() const { return mPrecision; }
    double Recall() const { return mRecall; }
    double F1Score() const { return mF1Score; }
protected:
    void EvaluateOneUser(int userId, const ItemRating* truthDataBegin, int truthDataSize, double* precision, double* recall);
protected:
    const BasicPredict *mPredict;
    const RatingMatrixAsUsers<> *mTestRatingMatrix;
    int mRankingListSize;
    double mPrecision;
    double mRecall;
    double mF1Score;
};

class SimpleEvaluateRankingDelegate : public EvaluateRankingDelegate {
public:
    using EvaluateRankingDelegate::EvaluateRankingDelegate;
    virtual void Evaluate() override;
};

class DynamicScheduledEvaluateRankingDelegate : public EvaluateRankingDelegate {
public:
    using EvaluateRankingDelegate::EvaluateRankingDelegate;
    virtual void Evaluate() override;
private:
    void DoGenerateTaskWork();
    void DoEvaluateRankingWork();
    void DoAverageResultWork();
    void DoMonitorProgress();
    struct Task {
        int userId;
        double precision;
        double recall;
        Task(int uid): userId(uid), precision(0.0), recall(0.0) { }
    };
    static const int TASK_BUNDLE_SIZE = 128;
    typedef std::vector<Task> TaskBundle;
    class Scheduler {
    public:
        Scheduler();
        ~Scheduler();
        void PutEvaluateRankingWork(TaskBundle* bundle);
        TaskBundle* GetEvaluateRankingWork();
        void PutAverageResultWork(TaskBundle* bundle);
        TaskBundle* GetAverageResultWork();
        void GenerateTaskDone();
        void EvaluateRankingDone();
        void AverageResultDone();
        int NumGenerateTaskWorker() const { return mNumGenerateTaskWorker; }
        int NumEvaluateRankingWorker() const { return mNumEvaluateRankingWorker; }
        int NumAverageResultWorker() const { return mNumAverageResultWorker; }
    private:
        static const int BUFFER_SIZE = 100;
        int mNumGenerateTaskWorker;
        int mNumEvaluateRankingWorker;
        int mNumAverageResultWorker;
        ObjectBuffer<TaskBundle*> *mObjectBuffer1;
        ObjectBuffer<TaskBundle*> *mObjectBuffer2;
    };
private:
    Scheduler *mScheduler;
    double mProgress;
};

} //~ namespace longan

#endif /* RECSYS_BASE_BASIC_EVALUATE_H */
