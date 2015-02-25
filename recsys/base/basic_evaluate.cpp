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
#include "recsys/evaluate/evaluate_novelty_delegate.h"
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
    CreatePredict();
    CreateEvaluateOption();
    LoadTrainData();
    LoadTestData();
    mResult.clear();
    if (mOption->EvaluateRating()) EvaluateRating();
    if (mOption->EvaluateRanking()) EvaluateRanking();
    if (mOption->EvaluateCoverage()) EvaluateCoverage();
    if (mOption->EvaluateDiversity()) EvaluateDiversity();
    if (mOption->EvaluateNovelty()) EvaluateNovelty();
    WriteResult();
    Cleanup();
}

void BasicEvaluate::LoadConfig() {
    Log::I("recsys", "BasicEvaluate::LoadConfig()");
    Log::I("recsys", "config file = " + mConfigFilepath);
    JsonConfigHelper::LoadFromFile(mConfigFilepath, mConfig);
}

void BasicEvaluate::CreateEvaluateOption() {
    Log::I("recsys", "BasicEvaluate::CreateEvaluateOption()");
    mOption = new EvaluateOption(mConfig["evaluateOption"]);
}

void BasicEvaluate::LoadTrainData() {
    if (mOption->EvaluateNovelty()) {
        Log::I("recsys", "BasicEvaluate::LoadTrainData()");
        Log::I("recsys", "train rating file = " + mRatingTrainFilepath);
        mTrainData = new RatingList(RatingList::LoadFromBinaryFile(mRatingTrainFilepath));
    }
}

void BasicEvaluate::LoadTestData() {
    if (mOption->EvaluateRating() || mOption->EvaluateRanking()
     || mOption->EvaluateCoverage() || mOption->EvaluateDiversity()) {
        Log::I("recsys", "BasicEvaluate::LoadTestData()");
        Log::I("recsys", "test rating file = " + mRatingTestFilepath);
        mTestData = new RatingList(RatingList::LoadFromBinaryFile(mRatingTestFilepath));
    }
}

void BasicEvaluate::EvaluateRating() {
    Log::I("recsys", "BasicEvaluate::EvaluateRating()");
    EvaluateRatingDelegate *evaluate = nullptr;
    if (mOption->Accelerate()) {
        evaluate = new EvaluateRatingDelegateMT();
    } else {
        evaluate = new EvaluateRatingDelegateST();
    }
    evaluate->Evaluate(mPredict, mTestData, mOption);
    mResult["ratingResult"]["MAE"] = evaluate->MAE();
    mResult["ratingResult"]["RMSE"] = evaluate->RMSE();
    Log::I("recsys", "evaluate rating result = \n" + mResult["ratingResult"].toStyledString());
    delete evaluate;
}

void BasicEvaluate::EvaluateRanking() {
    Log::I("recsys", "BasicEvaluate::EvaluateRanking()");
    EvaluateRankingDelegate *evaluate = nullptr;
    if (mOption->Accelerate()) {
        evaluate = new EvaluateRankingDelegateMT();
    } else {
        evaluate = new EvaluateRankingDelegateST();
    }
    for (int size : mOption->RankingListSizes()) {
        Log::I("recsys", "evaluating ranking list size = %d", size);
        mOption->SetCurrentRankingListSize(size);
        evaluate->Evaluate(mPredict, mTestData, mOption);
        mResult["rankingResult"]["Precision"].append(evaluate->Precision());
        mResult["rankingResult"]["Recall"].append(evaluate->Recall());
        mResult["rankingResult"]["F1Score"].append(evaluate->F1Score());
    }
    Log::I("recsys", "evaluate ranking result = \n" + mResult["rankingResult"].toStyledString());
    delete evaluate;
}

void BasicEvaluate::EvaluateCoverage() {
    Log::I("recsys", "BasicEvaluate::EvaluateCoverage()");
    EvaluateCoverageDelegate *evaluate = nullptr;
    if (mOption->Accelerate()) {
        evaluate = new EvaluateCoverageDelegateMT();
    } else {
        evaluate = new EvaluateCoverageDelegateST();
    }
    for (int size : mOption->RankingListSizes()) {
        Log::I("recsys", "evaluating ranking list size = %d", size);
        mOption->SetCurrentRankingListSize(size);
        evaluate->Evaluate(mPredict, mTestData, mOption);
        mResult["coverageResult"]["Coverage"].append(evaluate->Coverage());
        mResult["coverageResult"]["Entropy"].append(evaluate->Entropy());
        mResult["coverageResult"]["GiniIndex"].append(evaluate->GiniIndex());
    }
    Log::I("recsys", "evaluate coverage result = \n" + mResult["coverageResult"].toStyledString());
    delete evaluate;
}

void BasicEvaluate::EvaluateDiversity() {
    Log::I("recsys", "BasicEvaluate::EvaluateDiversity()");
    EvaluateDiversityDelegate *evaluate = nullptr;
    if (mOption->Accelerate()) {
        evaluate = new EvaluateDiversityDelegateMT();
    } else {
        evaluate = new EvaluateDiversityDelegateST();
    }
    for (int size : mOption->RankingListSizes()) {
        Log::I("recsys", "evaluating ranking list size = %d", size);
        mOption->SetCurrentRankingListSize(size);
        evaluate->Evaluate(mPredict, mTestData, mOption);
        mResult["diversityResult"]["diversity"].append(evaluate->Diversity());
    }
    Log::I("recsys", "evaluate diversity result = \n" + mResult["diversityResult"].toStyledString());
    delete evaluate;
}

void BasicEvaluate::EvaluateNovelty() {
    Log::I("recsys", "BasicEvaluate::EvaluateNovelty()");
    EvaluateNoveltyDelegate *evaluate = nullptr;
    if (mOption->Accelerate()) {
        evaluate = new EvaluateNoveltyDelegateMT();
    } else {
        evaluate = new EvaluateNoveltyDelegateST();
    }
    for (int size : mOption->RankingListSizes()) {
        Log::I("recsys", "evaluating ranking list size = %d", size);
        mOption->SetCurrentRankingListSize(size);
        evaluate->Evaluate(mPredict, mTrainData, mOption);
        mResult["noveltyResult"]["novelty"].append(evaluate->Novelty());
    }
    Log::I("recsys", "evaluate novelty result = \n" + mResult["noveltyResult"].toStyledString());
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
    delete mTrainData;
    delete mTestData;
}

} //~ namespace longan
