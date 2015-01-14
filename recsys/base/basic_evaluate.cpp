/*
 * basic_evaluate.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "basic_evaluate.h"
#include "common/logging/logging.h"
#include "common/system/file_util.h"
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
    Log::I("recsys", "BasicEvaluate::Evaluate()");
    LoadConfig();
    LoadTestRatings();
    CreatePredict();
    EvaluateRating();
    EvaluateRanking();
    WriteResult();
    Cleanup();
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
    if (!mConfig["evaluateOption"]["evaluateRating"].asBool()) return;
    Log::I("recsys", "BasicEvaluate::EvaluateRating()");
    if (mConfig["evaluateOption"]["accelerate"].asBool()) {
        mEvaluateRatingDelegate = new DynamicScheduledEvaluateRatingDelegate();
    } else {
        mEvaluateRatingDelegate = new SimpleEvaluateRatingDelegate();
    }
    mEvaluateRatingDelegate->Evaluate(mPredict, &mTestRatingList);
    mMAE = mEvaluateRatingDelegate->MAE();
    mRMSE = mEvaluateRatingDelegate->RMSE();
    delete mEvaluateRatingDelegate;
}

void BasicEvaluate::EvaluateRanking() {
    if (!mConfig["evaluateOption"]["evaluateRanking"].asBool()) return;
    Log::I("recsys", "BasicEvaluate::EvaluateRanking()");
    RatingMatrixAsUsers<> ratingMatrix;
    ratingMatrix.Init(mTestRatingList);
    int listSize = mConfig["evaluateOption"]["rankingListSize"].asInt();
    if (mConfig["evaluateOption"]["accelerate"].asBool()) {
        mEvaluateRankingDelegate = new DynamicScheduledEvaluateRankingDelegate();
    } else {
        mEvaluateRankingDelegate = new SimpleEvaluateRankingDelegate();
    }
    mEvaluateRankingDelegate->Evaluate(mPredict, &ratingMatrix, listSize);
    mPrecision = mEvaluateRankingDelegate->Precision();
    mRecall = mEvaluateRankingDelegate->Recall();
    mF1Score = mEvaluateRankingDelegate->F1Score();
    delete mEvaluateRankingDelegate;
}

void BasicEvaluate::WriteResult() {
    Log::I("recsys", "BasicEvaluate::WriteResult()");
    Json::Value result;
    if (mConfig["evaluateOption"]["evaluateRating"].asBool()) {
        result["ratingResult"]["MAE"] = mMAE;
        result["ratingResult"]["RMSE"] = mRMSE;
    }
    if (mConfig["evaluateOption"]["evaluateRanking"].asBool()) {
        result["rankingResult"]["Precision"] = mPrecision;
        result["rankingResult"]["Recall"] = mRecall;
        result["rankingResult"]["F1Score"] = mF1Score;
    }
    std::string output = result.toStyledString();
    Log::I("recsys", "result = \n" + output);
    Log::I("recsys", "writing result to file = " + mResultFilepath);
    FileUtil::SaveFileContent(mResultFilepath, output);
}

void BasicEvaluate::Cleanup() {
    mPredict->Cleanup();
    delete mPredict;
    mPredict = nullptr;
}

} //~ namespace longan
