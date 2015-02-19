/*
 * basic_evaluate.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "basic_evaluate.h"
#include "recsys/evaluate/evaluate_rating_delegate.h"
#include "recsys/evaluate/evaluate_ranking_delegate.h"
#include "recsys/evaluate/evaluate_coverage_delegate.h"
#include "recsys/evaluate/evaluate_diversity_delegate.h"
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
    CreateEvaluateOption();
    mResult.clear();
    EvaluateRating();
    EvaluateRanking();
    EvaluateCoverage();
    EvaluateDiversity();
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

void BasicEvaluate::CreateEvaluateOption() {
    Log::I("recsys", "BasicEvaluate::CreateEvaluateOption()");
    mOption = new EvaluateOption(mConfig["evaluateOption"]);
}

void BasicEvaluate::EvaluateRating() {
    if (!mOption->EvaluateRating()) return;
    Log::I("recsys", "BasicEvaluate::EvaluateRating()");
    EvaluateRatingDelegate *evaluate = nullptr;
    if (mOption->Accelerate()) {
        evaluate = new EvaluateRatingDelegateMT();
    } else {
        evaluate = new EvaluateRatingDelegateST();
    }
    evaluate->Evaluate(mPredict, mTestRatingList, mOption);
    mResult["ratingResult"]["MAE"] = evaluate->MAE();
    mResult["ratingResult"]["RMSE"] = evaluate->RMSE();
    Log::I("recsys", "evaluate rating result = \n" + mResult["ratingResult"].toStyledString());
    delete evaluate;
}

void BasicEvaluate::EvaluateRanking() {
    if (!mOption->EvaluateRanking()) return;
    Log::I("recsys", "BasicEvaluate::EvaluateRanking()");
    EvaluateRankingDelegate *evaluate = nullptr;
    RatingMatUsers *rmat = new RatingMatUsers();
    rmat->Init(*mTestRatingList);
    if (mOption->Accelerate()) {
        evaluate = new EvaluateRankingDelegateMT();
    } else {
        evaluate = new EvaluateRankingDelegateST();
    }
    evaluate->Evaluate(mPredict, rmat, mOption);
    mResult["rankingResult"]["Precision"] = evaluate->Precision();
    mResult["rankingResult"]["Recall"] = evaluate->Recall();
    mResult["rankingResult"]["F1Score"] = evaluate->F1Score();
    Log::I("recsys", "evaluate ranking result = \n" + mResult["rankingResult"].toStyledString());
    delete rmat;
    delete evaluate;
}

void BasicEvaluate::EvaluateCoverage() {
    if (!mOption->EvaluateCoverage()) return;
    Log::I("recsys", "BasicEvaluate::EvaluateCoverage()");
    EvaluateCoverageDelegate *evaluate = nullptr;
    if (mOption->Accelerate()) {
        evaluate = new EvaluateCoverageDelegateMT();
    } else {
        evaluate = new EvaluateCoverageDelegateST();
    }
    evaluate->Evaluate(mPredict, mTestRatingList, mOption);
    mResult["coverageResult"]["Coverage"] = evaluate->Coverage();
    mResult["coverageResult"]["Entropy"] = evaluate->Entropy();
    mResult["coverageResult"]["GiniIndex"] = evaluate->GiniIndex();
    Log::I("recsys", "evaluate coverage result = \n" + mResult["coverageResult"].toStyledString());
    delete evaluate;
}

void BasicEvaluate::EvaluateDiversity() {
    if (!mOption->EvaluateDiversity()) return;
    Log::I("recsys", "BasicEvaluate::EvaluateDiversity()");
    EvaluateDiversityDelegate *evaluate = nullptr;
    if (mOption->Accelerate()) {
        evaluate = new EvaluateDiversityDelegateMT();
    } else {
        evaluate = new EvaluateDiversityDelegateST();
    }
    evaluate->Evaluate(mPredict, mTestRatingList, mOption);
    mResult["diversityResult"]["diversity"] = evaluate->Diversity();
    delete evaluate;
}

void BasicEvaluate::WriteResult() {
    Log::I("recsys", "BasicEvaluate::WriteResult()");
    Log::I("recsys", "result = \n" + mResult.toStyledString());
    Log::I("recsys", "writing result to file = " + mResultFilepath);
    JsonConfigHelper::WriteToFile(mResultFilepath, mResult);
}

void BasicEvaluate::Cleanup() {
    mPredict->Cleanup();
    delete mPredict;
    delete mOption;
    delete mTestRatingList;
}

} //~ namespace longan
