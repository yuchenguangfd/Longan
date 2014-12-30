/*
 * basic_evaluate.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "basic_evaluate.h"
#include "common/math/math.h"
#include "common/logging/logging.h"
#include "common/system/file_util.h"
#include "common/system/system_info.h"
#include "common/util/running_statistic.h"
#include "common/util/string_helper.h"
#include "common/base/algorithm.h"
#include "common/error.h"
#include <thread>
#include <cassert>

namespace longan {

BasicEvaluate::BasicEvaluate(
    const std::string& ratingTrainFilepath,
    const std::string& configFilepath,
    const std::string& modelFilepath,
    const std::string& ratingTestFilepath,
    const std::string& resultFilepath) :
    mRatingTrainFilepath(ratingTrainFilepath),
    mConfigFilepath(configFilepath),
    mModelFilepath(modelFilepath),
    mRatingTestFilepath(ratingTestFilepath),
    mResultFilepath(resultFilepath),
    mPredict(nullptr),
    mEvaluateRatingDelegate(nullptr),
    mEvaluateRankingDelegate(nullptr),
    mMAE(0.0),
    mRMSE(0.0),
    mPrecision(0.0),
    mRecall(0.0),
    mF1Score(0.0) { }

BasicEvaluate::~BasicEvaluate() { }

void BasicEvaluate::Evaluate() {
    LoadConfig();
    LoadTestRatings();
    CreatePredict();
    if (mConfig["evaluateRating"].asBool()) {
        EvaluateRating();
    }
    if (mConfig["evaluateRanking"].asBool()) {
        EvaluateRanking();
    }
    WriteResult();
    DestroyPredict();
}

void BasicEvaluate::LoadConfig() {
    Log::I("recsys", "BasicEvaluate::LoadConfig()");
    Log::I("recsys", "config file = " + mConfigFilepath);
    std::string content = FileUtil::LoadFileContent(mConfigFilepath);
    Json::Reader reader;
    if (!reader.parse(content, mConfig)) {
        throw LonganFileFormatError();
    }
}

void BasicEvaluate::LoadTestRatings() {
    Log::I("recsys", "BasicEvaluate::LoadTestRatings()");
    mTestRatingList = RatingList::LoadFromBinaryFile(mRatingTestFilepath);
}

void BasicEvaluate::EvaluateRating() {
    Log::I("recsys", "BasicEvaluate::EvaluateRating()");
    mEvaluateRatingDelegate = new DynamicScheduledEvaluateRatingDelegate(mPredict, &mTestRatingList);
    mEvaluateRatingDelegate->Evaluate();
    mMAE = mEvaluateRatingDelegate->MAE();
    mRMSE = mEvaluateRatingDelegate->RMSE();
    delete mEvaluateRatingDelegate;
}

void BasicEvaluate::EvaluateRanking() {
    Log::I("recsys", "BasicEvaluate::EvaluateRanking()");
    RatingMatrixAsUsers<> ratingMatrix;
    ratingMatrix.Init(mTestRatingList);
    int listSize = mConfig["rankingListSize"].asInt();
    mEvaluateRankingDelegate = new DynamicScheduledEvaluateRankingDelegate(mPredict, &ratingMatrix, listSize);
    mEvaluateRankingDelegate->Evaluate();
    mPrecision = mEvaluateRankingDelegate->Precision();
    mRecall = mEvaluateRankingDelegate->Recall();
    mF1Score = mEvaluateRankingDelegate->F1Score();
    delete mEvaluateRankingDelegate;
}

void BasicEvaluate::WriteResult() {
    Log::I("recsys", "BasicEvaluate::WriteResult()");
    Json::Value result;
    if (mConfig["evaluateRating"].asBool()) {
        result["ratingResult"]["MAE"] = mMAE;
        result["ratingResult"]["RMSE"] = mRMSE;
    }
    if (mConfig["evaluateRanking"].asBool()) {
        result["rankingResult"]["Precision"] = mPrecision;
        result["rankingResult"]["Recall"] = mRecall;
        result["rankingResult"]["F1Score"] = mF1Score;
    }
    std::string output = result.toStyledString();
    Log::I("recsys", "result = \n" + output);
    Log::I("recsys", "writing result to file = " + mResultFilepath);
    FileUtil::SaveFileContent(mResultFilepath, output);
}

void BasicEvaluate::DestroyPredict() {
    mPredict->Cleanup();
    delete mPredict;
    mPredict = nullptr;
}

EvaluateRatingDelegate::EvaluateRatingDelegate(const BasicPredict *predict, const RatingList *rlist):
    mPredict(predict),
    mTestRatingList(rlist),
    mMAE(0.0),
    mRMSE(0.0) { }

EvaluateRatingDelegate::~EvaluateRatingDelegate() { }

void SimpleEvaluateRatingDelegate::Evaluate() {
    double sumAbs = 0.0;
    double sumSqr = 0.0;
    for (int i = 0; i < mTestRatingList->NumRating(); ++i) {
        const RatingRecord& rr = mTestRatingList->At(i);
        float predictedRating = mPredict->PredictRating(rr.UserId(), rr.ItemId());
        float trueRating = rr.Rating();
        float error = predictedRating - trueRating;
        sumAbs += Math::Abs(error);
        sumSqr += Math::Sqr(error);
    }
    mMAE = sumAbs / mTestRatingList->NumRating();
    mRMSE = Math::Sqrt(sumSqr / mTestRatingList->NumRating());
}

void DynamicScheduledEvaluateRatingDelegate::Evaluate() {
    mScheduler = new Scheduler();
    std::vector<std::thread> workers;
    for (int i = 0; i < mScheduler->NumGenerateTaskWorker(); ++i) {
        workers.push_back(std::thread(&DynamicScheduledEvaluateRatingDelegate::DoGenerateTaskWork, this));
    }
    for (int i = 0; i < mScheduler->NumPredictRatingWorker(); ++i) {
        workers.push_back(std::thread(&DynamicScheduledEvaluateRatingDelegate::DoPredictRatingWork,this));
    }
    for (int i = 0; i < mScheduler->NumCalcuateErrorWorker(); ++i) {
        workers.push_back(std::thread(&DynamicScheduledEvaluateRatingDelegate::DoCalcuateErrorWork, this));
    }
    workers.push_back(std::thread(&DynamicScheduledEvaluateRatingDelegate::DoMonitorProgress, this));
    for (std::thread& t : workers) {
        t.join();
    }
    delete mScheduler;
}

void DynamicScheduledEvaluateRatingDelegate::DoGenerateTaskWork() {
    TaskBundle *currentBundle = new TaskBundle();
    currentBundle->reserve(TASK_BUNDLE_SIZE);
    for (int i = 0; i < mTestRatingList->NumRating(); ++i) {
        const RatingRecord& rr = mTestRatingList->At(i);
        if (currentBundle->size() == TASK_BUNDLE_SIZE) {
            mScheduler->PutPredictRatingWork(currentBundle);
            currentBundle = new TaskBundle();
            currentBundle->reserve(TASK_BUNDLE_SIZE);
        }
        currentBundle->push_back(Task(rr.UserId(), rr.ItemId(), rr.Rating()));
    }
    mScheduler->PutPredictRatingWork(currentBundle);
    mScheduler->GenerateTaskDone();
}

void DynamicScheduledEvaluateRatingDelegate::DoPredictRatingWork() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->GetPredictRatingWork();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            task.predictedRating = mPredict->PredictRating(task.userId, task.itemId);
        }
        mScheduler->PutCalcuateErrorWork(currentBundle);
    }
    mScheduler->PredictRatingDone();
}

void DynamicScheduledEvaluateRatingDelegate::DoCalcuateErrorWork() {
    int totoalTask = mTestRatingList->NumRating();
    int processedTask = 0;
    double sumAbs = 0.0;
    double sumSqr = 0.0;
    while (true) {
        TaskBundle *currentBundle = mScheduler->GetCalcuateErrorWork();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            float error = task.predictedRating - task.trueRating;
            sumAbs += Math::Abs(error);
            sumSqr += Math::Sqr(error);
        }
        processedTask += currentBundle->size();
        mProgress = static_cast<double>(processedTask)/totoalTask;
        delete currentBundle;
    }
    mScheduler->CalcuateErrorDone();
    mMAE = sumAbs / mTestRatingList->NumRating();
    mRMSE = Math::Sqrt(sumSqr / mTestRatingList->NumRating());
}

void DynamicScheduledEvaluateRatingDelegate::DoMonitorProgress() {
    while (true) {
        ConsoleLog::I("recsys", "Evaluate Rating..." + StringHelper::ToString((int)(mProgress*100)) + "% done.");
        if (mProgress > 0.99) break;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

DynamicScheduledEvaluateRatingDelegate::Scheduler::Scheduler() {
    mNumGenerateTaskWorker = 1;
    mNumPredictRatingWorker = SystemInfo::GetNumCPUCore();
    mNumCalcuateErrorWorker = 1;
    mObjectBuffer1 = new ObjectBuffer<TaskBundle*>("GenerateTask-PredictRating", BUFFER_SIZE,
               mNumGenerateTaskWorker, mNumPredictRatingWorker);
    mObjectBuffer2 = new ObjectBuffer<TaskBundle*>("PredictRating-CalcuateError", BUFFER_SIZE,
               mNumPredictRatingWorker, mNumCalcuateErrorWorker);
}

DynamicScheduledEvaluateRatingDelegate::Scheduler::~Scheduler() {
    delete mObjectBuffer2;
    delete mObjectBuffer1;
}

void DynamicScheduledEvaluateRatingDelegate::Scheduler::PutPredictRatingWork(
        DynamicScheduledEvaluateRatingDelegate::TaskBundle* bundle) {
    int rtn = mObjectBuffer1->Push(bundle);
    assert(rtn == LONGAN_SUCC);
}

DynamicScheduledEvaluateRatingDelegate::TaskBundle*
    DynamicScheduledEvaluateRatingDelegate::Scheduler::GetPredictRatingWork() {
    TaskBundle* bundle;
    int rtn = mObjectBuffer1->Pop(bundle);
    return (rtn == LONGAN_SUCC) ? bundle : nullptr;
}

void DynamicScheduledEvaluateRatingDelegate::Scheduler::PutCalcuateErrorWork(
    DynamicScheduledEvaluateRatingDelegate::TaskBundle* bundle) {
    int rtn = mObjectBuffer2->Push(bundle);
    assert(rtn == LONGAN_SUCC);
}

DynamicScheduledEvaluateRatingDelegate::TaskBundle*
    DynamicScheduledEvaluateRatingDelegate::Scheduler::GetCalcuateErrorWork() {
    TaskBundle* bundle;
    int rtn = mObjectBuffer2->Pop(bundle);
    return (rtn == LONGAN_SUCC) ? bundle : nullptr;
}

void DynamicScheduledEvaluateRatingDelegate::Scheduler::GenerateTaskDone() {
    int rtn = mObjectBuffer1->ProducerQuit();
    assert(rtn == LONGAN_SUCC);
}

void DynamicScheduledEvaluateRatingDelegate::Scheduler::PredictRatingDone() {
    int rtn1 = mObjectBuffer1->ConsumerQuit();
    assert(rtn1 == LONGAN_SUCC);
    int rtn2 = mObjectBuffer2->ProducerQuit();
    assert(rtn2 == LONGAN_SUCC);
}

void DynamicScheduledEvaluateRatingDelegate::Scheduler::CalcuateErrorDone() {
    int rtn = mObjectBuffer2->ConsumerQuit();
    assert(rtn == LONGAN_SUCC);
}

EvaluateRankingDelegate::EvaluateRankingDelegate(const BasicPredict *predict, const RatingMatrixAsUsers<> *rmat, int listSize) :
    mPredict(predict),
    mTestRatingMatrix(rmat),
    mRankingListSize(listSize),
    mPrecision(0.0),
    mRecall(0.0),
    mF1Score(0.0) { }

EvaluateRankingDelegate::~EvaluateRankingDelegate() { }

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

void SimpleEvaluateRankingDelegate::Evaluate() {
    assert(mRankingListSize > 0);
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

void DynamicScheduledEvaluateRankingDelegate::Evaluate() {
    assert(mRankingListSize > 0);
    mScheduler = new Scheduler();
    std::vector<std::thread> workers;
    for (int i = 0; i < mScheduler->NumGenerateTaskWorker(); ++i) {
        workers.push_back(std::thread(&DynamicScheduledEvaluateRankingDelegate::DoGenerateTaskWork, this));
    }
    for (int i = 0; i < mScheduler->NumEvaluateRankingWorker(); ++i) {
        workers.push_back(std::thread(&DynamicScheduledEvaluateRankingDelegate::DoEvaluateRankingWork,this));
    }
    for (int i = 0; i < mScheduler->NumAverageResultWorker(); ++i) {
        workers.push_back(std::thread(&DynamicScheduledEvaluateRankingDelegate::DoAverageResultWork, this));
    }
    workers.push_back(std::thread(&DynamicScheduledEvaluateRankingDelegate::DoMonitorProgress, this));
    for (std::thread& t : workers) {
        t.join();
    }
    delete mScheduler;
}

void DynamicScheduledEvaluateRankingDelegate::DoGenerateTaskWork() {
    TaskBundle *currentBundle = new TaskBundle();
    currentBundle->reserve(TASK_BUNDLE_SIZE);
    for (int uid = 0; uid < mTestRatingMatrix->NumUser(); ++uid) {
        if (currentBundle->size() == TASK_BUNDLE_SIZE) {
            mScheduler->PutEvaluateRankingWork(currentBundle);
            currentBundle = new TaskBundle();
            currentBundle->reserve(TASK_BUNDLE_SIZE);
        }
        currentBundle->push_back(Task(uid));
    }
    mScheduler->PutEvaluateRankingWork(currentBundle);
    mScheduler->GenerateTaskDone();
}

void DynamicScheduledEvaluateRankingDelegate::DoEvaluateRankingWork() {
    while (true) {
        TaskBundle *currentBundle = mScheduler->GetEvaluateRankingWork();
        if (currentBundle == nullptr) break;
        for (int i = 0; i < currentBundle->size(); ++i) {
            Task& task = currentBundle->at(i);
            const auto& uv = mTestRatingMatrix->GetUserVector(task.userId);
            EvaluateOneUser(task.userId, uv.Data(), uv.Size(), &task.precision, &task.recall);
        }
        mScheduler->PutAverageResultWork(currentBundle);
    }
    mScheduler->EvaluateRankingDone();
}

void DynamicScheduledEvaluateRankingDelegate::DoAverageResultWork() {
    int totoalTask = mTestRatingMatrix->NumUser();
    int processedTask = 0;
    RunningAverage<double> raPrecision;
    RunningAverage<double> raRecall;
    while (true) {
        TaskBundle *currentBundle = mScheduler->GetAverageResultWork();
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
    mScheduler->AverageResultDone();
    mPrecision = raPrecision.CurrentAverage();
    mRecall = raRecall.CurrentAverage();
    mF1Score = 2.0 * (mPrecision * mRecall)/(mPrecision + mRecall);
}

void DynamicScheduledEvaluateRankingDelegate::DoMonitorProgress() {
    while (true) {
        ConsoleLog::I("recsys", "Evaluate Ranking..." + StringHelper::ToString((int)(mProgress*100)) + "% done.");
        if (mProgress > 0.99) break;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

DynamicScheduledEvaluateRankingDelegate::Scheduler::Scheduler() {
    mNumGenerateTaskWorker = 1;
    mNumEvaluateRankingWorker = SystemInfo::GetNumCPUCore();
    mNumAverageResultWorker = 1;
    mObjectBuffer1 = new ObjectBuffer<TaskBundle*>("GenerateTask-EvaluateRanking", BUFFER_SIZE,
               mNumGenerateTaskWorker, mNumEvaluateRankingWorker);
    mObjectBuffer2 = new ObjectBuffer<TaskBundle*>("EvaluateRanking-AverageResult", BUFFER_SIZE,
               mNumEvaluateRankingWorker, mNumAverageResultWorker);
}

DynamicScheduledEvaluateRankingDelegate::Scheduler::~Scheduler() {
    delete mObjectBuffer2;
    delete mObjectBuffer1;
}

void DynamicScheduledEvaluateRankingDelegate::Scheduler::PutEvaluateRankingWork(
        TaskBundle* bundle) {
    int rtn = mObjectBuffer1->Push(bundle);
    assert(rtn == LONGAN_SUCC);
}

DynamicScheduledEvaluateRankingDelegate::TaskBundle*
    DynamicScheduledEvaluateRankingDelegate::Scheduler::GetEvaluateRankingWork() {
    TaskBundle* bundle;
    int rtn = mObjectBuffer1->Pop(bundle);
    return (rtn == LONGAN_SUCC) ? bundle : nullptr;
}

void DynamicScheduledEvaluateRankingDelegate::Scheduler::PutAverageResultWork(
        TaskBundle* bundle) {
    int rtn = mObjectBuffer2->Push(bundle);
    assert(rtn == LONGAN_SUCC);
}

DynamicScheduledEvaluateRankingDelegate::TaskBundle*
    DynamicScheduledEvaluateRankingDelegate::Scheduler::GetAverageResultWork() {
    TaskBundle* bundle;
    int rtn = mObjectBuffer2->Pop(bundle);
    return (rtn == LONGAN_SUCC) ? bundle : nullptr;
}

void DynamicScheduledEvaluateRankingDelegate::Scheduler::GenerateTaskDone() {
    int rtn = mObjectBuffer1->ProducerQuit();
    assert(rtn == LONGAN_SUCC);
}

void DynamicScheduledEvaluateRankingDelegate::Scheduler::EvaluateRankingDone() {
    int rtn1 = mObjectBuffer1->ConsumerQuit();
    assert(rtn1 == LONGAN_SUCC);
    int rtn2 = mObjectBuffer2->ProducerQuit();
    assert(rtn2 == LONGAN_SUCC);
}

void DynamicScheduledEvaluateRankingDelegate::Scheduler::AverageResultDone() {
    int rtn = mObjectBuffer2->ConsumerQuit();
    assert(rtn == LONGAN_SUCC);
}

} //~ namespace longan
