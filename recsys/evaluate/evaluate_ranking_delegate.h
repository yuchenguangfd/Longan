/*
 * evaluate_ranking_delegate.h
 * Created on: Jan 14, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_EVALUATE_EVALUATE_RANKING_DELEGATE_H
#define RECSYS_EVALUATE_EVALUATE_RANKING_DELEGATE_H

#include "evaluate_util.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "common/threading/pipelined_scheduler.h"

namespace longan {

class BasicPredict;

class EvaluateRankingDelegate {
public:
    virtual ~EvaluateRankingDelegate() { }
    virtual void Evaluate(const BasicPredict *predict, const RatingMatUsers *testData,
            const EvaluateOption *option) = 0;
    double Precision() const { return mPrecision; }
    double Recall() const { return mRecall; }
    double F1Score() const { return mF1Score; }
protected:
    void EvaluateOneUser(int userId, const UserVec& userVec, int *hitCount, int* nPrecision, int *nRecall);
protected:
    const BasicPredict *mPredict = nullptr;
    const RatingMatUsers *mTestData = nullptr;
    const EvaluateOption *mOption = nullptr;
    int mRankingListSize = 0;
    double mPrecision = 0.0;
    double mRecall = 0.0;
    double mF1Score = 0.0;
};

class EvaluateRankingDelegateST : public EvaluateRankingDelegate {
public:
    virtual void Evaluate(const BasicPredict *predict, const RatingMatUsers *testData,
            const EvaluateOption *option) override;
};

class EvaluateRankingDelegateMT : public EvaluateRankingDelegate, public PipelinedSchedulerClient {
public:
    virtual void Evaluate(const BasicPredict *predict, const RatingMatUsers *testData,
            const EvaluateOption *option) override;
    virtual std::thread* CreateProducerThread() {
        return new std::thread(&EvaluateRankingDelegateMT::ProducerRun, this);
    }
    virtual std::thread* CreateWorkerThread() {
        return new std::thread(&EvaluateRankingDelegateMT::WorkerRun, this);
    }
    virtual std::thread* CreateConsumerThread() {
        return new std::thread(&EvaluateRankingDelegateMT::ConsumerRun, this);
    }
    virtual std::thread* CreateMonitorThread() {
        return mOption->MonitorProgress() ?
               new std::thread(&EvaluateRankingDelegateMT::MonitorRun, this) :
               nullptr;
    }
private:
    void ProducerRun();
    void WorkerRun();
    void ConsumerRun();
    void MonitorRun();
private:
    struct Task {
        int userId;
        int hitCount;
        int nPrecision;
        int nRecall;
        Task(int uid): userId(uid), hitCount(0), nPrecision(0), nRecall(0) { }
    };
    static const int TASK_BUNDLE_SIZE = 128;
    typedef std::vector<Task> TaskBundle;
    PipelinedScheduler<TaskBundle> *mScheduler = nullptr;

    int mTotoalTask = 0;
    int mProcessedTask = 0;
    int mRunningHitCount = 0;
    int mRunningNPrecision = 0;
    int mRunningNRecall = 0;
};

} //~ namespace longan

#endif /* RECSYS_EVALUATE_EVALUATE_RANKING_DELEGATE_H */
