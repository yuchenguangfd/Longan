/*
 * item_based_evaluate.cpp
 * Created on: Nov 12, 2014
 * Author: chenguangyu
 */

#include "item_based_evaluate.h"
#include "recsys/base/rating_list_loader.h"
#include "common/logging/logging.h"
#include "common/math/math.h"
#include "common/system/file_util.h"
#include "common/error.h"

namespace longan {

ItemBasedEvaluate::ItemBasedEvaluate(
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
    mMAE(0.0), mRMSE(0.0),
    mPrecision(0.0), mRecall(0.0) { }

void ItemBasedEvaluate::Evaluate() {
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
    Cleanup();
}

void ItemBasedEvaluate::LoadConfig() {
    Log::I("recsys", "ItemBasedEvaluate::LoadConfig()");
    Log::I("recsys", "config file = " + mConfigFilepath);
    std::string content = FileUtil::LoadFileContent(mConfigFilepath);
    Json::Reader reader;
    if (!reader.parse(content, mConfig)) {
        throw LonganFileFormatError();
    }
}

void ItemBasedEvaluate::LoadTestRatings() {
    Log::I("recsys", "ItemBasedEvaluate::LoadTestRatings()");
    mTestRatingList = std::move(RatingListLoader::Load(mRatingTestFilepath));
}

void ItemBasedEvaluate::CreatePredict() {
    Log::I("recsys", "ItemBasedEvaluate::CreatePredict()");
    mPredict = new ItemBasedPredict(mRatingTrainFilepath, mConfigFilepath, mModelFilepath);
    mPredict->Init();
}

void ItemBasedEvaluate::EvaluateRating() {
    Log::I("recsys", "ItemBasedEvaluate::EvaluateRating()");
    double sumMAE = 0.0;
    double sumRMSE = 0.0;
    for (int i = 0; i < mTestRatingList.NumRating(); ++i) {
        const RatingRecord& rr = mTestRatingList[i];
        float predictedRating = mPredict->PredictRating(rr.UserId(), rr.ItemId());
        float trueRating = rr.Rating();
        float error = predictedRating - trueRating;
        sumMAE += Math::Abs(error);
        sumRMSE += Math::Sqr(error);
    }
    mMAE = sumMAE / mTestRatingList.NumRating();
    mRMSE = Math::Sqrt(sumRMSE / mTestRatingList.NumRating());
}

void ItemBasedEvaluate::EvaluateRanking() {
    throw LonganNotSupportError();
}

void ItemBasedEvaluate::WriteResult() {
    Log::I("recsys", "ItemBasedEvaluate::WriteResult()");
    Json::Value result;
    if (mConfig["evaluateRating"].asBool()) {
        result["MAE"] = mMAE;
        result["RMSE"] = mRMSE;
    }
    if (mConfig["evaluateRanking"].asBool()) {
        result["Precision"] = mPrecision;
        result["Recall"] = mRecall;
    }
    std::string output = result.toStyledString();
    Log::I("recsys", "result = \n" + output);
    Log::I("recsys", "writing result to file = " + mResultFilepath);
    FileUtil::SaveFileContent(mResultFilepath, output);
}

void ItemBasedEvaluate::Cleanup() {
    delete mPredict;
}

} //~ namespace longan
