/*
 * basic_evaluate.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "basic_evaluate.h"
#include "recsys/evaluate/evaluate_rating_delegate.h"
#include "recsys/evaluate/evaluate_ranking_delegate.h"
#include "common/config/json_config_helper.h"
#include "common/logging/logging.h"
#include "common/error.h"
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
    mResultFilepath(resultFilepath) { }

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
    JsonConfigHelper::LoadFromFile(mConfigFilepath, mConfig);
}

void BasicEvaluate::LoadTestRatings() {
    Log::I("recsys", "BasicEvaluate::LoadTestRatings()");
    Log::I("recsys", "test rating file = " + mRatingTestFilepath);
    mTestRatingList = new RatingList(RatingList::LoadFromBinaryFile(mRatingTestFilepath));
}

void BasicEvaluate::EvaluateRating() {
    if (!mConfig["evaluateOption"]["evaluateRating"].asBool()) {
        return;
    }
    Log::I("recsys", "BasicEvaluate::EvaluateRating()");
    EvaluateRatingDelegate *evaluate = nullptr;
    if (mConfig["evaluateOption"]["accelerate"].asBool()) {
        evaluate = new EvaluateRatingDelegateMT();
    } else {
        evaluate = new EvaluateRatingDelegateST();
    }
    evaluate->Evaluate(mPredict, mTestRatingList);
    mEvaluateResult.mMAE = evaluate->MAE();
    mEvaluateResult.mRMSE = evaluate->RMSE();
    delete evaluate;
}

void BasicEvaluate::EvaluateRanking() {
    if (!mConfig["evaluateOption"]["evaluateRanking"].asBool()) {
        return;
    }
    Log::I("recsys", "BasicEvaluate::EvaluateRanking()");
    EvaluateRankingDelegate *evaluate = nullptr;
    RatingMatUsers *rmat = new RatingMatUsers();
    rmat->Init(*mTestRatingList);
    int listSize = mConfig["evaluateOption"]["rankingListSize"].asInt();
    assert(listSize > 0);
    if (mConfig["evaluateOption"]["accelerate"].asBool()) {
        evaluate = new EvaluateRankingDelegateMT();
    } else {
        evaluate = new EvaluateRankingDelegateST();
    }
    evaluate->Evaluate(mPredict, rmat, listSize);
    mEvaluateResult.mPrecision = evaluate->Precision();
    mEvaluateResult.mRecall = evaluate->Recall();
    mEvaluateResult.mF1Score = evaluate->F1Score();
    delete rmat;
    delete evaluate;
}

void BasicEvaluate::WriteResult() {
    Log::I("recsys", "BasicEvaluate::WriteResult()");
    Json::Value result;
    if (mConfig["evaluateOption"]["evaluateRating"].asBool()) {
        result["ratingResult"]["MAE"] = mEvaluateResult.mMAE;
        result["ratingResult"]["RMSE"] = mEvaluateResult.mRMSE;
    }
    if (mConfig["evaluateOption"]["evaluateRanking"].asBool()) {
        result["rankingResult"]["Precision"] = mEvaluateResult.mPrecision;
        result["rankingResult"]["Recall"] = mEvaluateResult.mRecall;
        result["rankingResult"]["F1Score"] = mEvaluateResult.mF1Score;
    }
    Log::I("recsys", "result = \n" + result.toStyledString());
    Log::I("recsys", "writing result to file = " + mResultFilepath);
    JsonConfigHelper::WriteToFile(mResultFilepath, result);
}

void BasicEvaluate::Cleanup() {
    mPredict->Cleanup();
    delete mPredict;
    delete mTestRatingList;
}

} //~ namespace longan
