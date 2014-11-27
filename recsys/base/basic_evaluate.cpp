/*
 * basic_evaluate.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "basic_evaluate.h"
#include "rating_list_loader.h"
#include "rating_matrix_as_users.h"
#include "common/math/math.h"
#include "common/logging/logging.h"
#include "common/system/file_util.h"
#include "common/util/running_statistic.h"
#include "common/base/algorithm.h"
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
    mResultFilepath(resultFilepath),
    mPredict(nullptr),
    mMAE(0.0),
    mRMSE(0.0),
    mPrecision(0.0),
    mRecall(0.0) { }

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
    mTestRatingList = RatingListLoader::Load(mRatingTestFilepath);
}

void BasicEvaluate::EvaluateRating() {
    Log::I("recsys", "BasicEvaluate::EvaluateRating()");
    double sumAbs = 0.0;
    double sumSqr = 0.0;
    for (int i = 0; i < mTestRatingList.NumRating(); ++i) {
        const RatingRecord& rr = mTestRatingList[i];
        // predict rating for userId and itemId in test data
        float predictedRating = mPredict->PredictRating(rr.UserId(), rr.ItemId());
        float trueRating = rr.Rating();
        float error = predictedRating - trueRating;
        sumAbs += Math::Abs(error);
        sumSqr += Math::Sqr(error);
    }
    mMAE = sumAbs / mTestRatingList.NumRating();
    mRMSE = Math::Sqrt(sumSqr / mTestRatingList.NumRating());
}

void BasicEvaluate::EvaluateRanking() {
    Log::I("recsys", "BasicEvaluate::EvaluateRanking()");
    RatingMatrixAsUsers<> ratingMatrix;
    ratingMatrix.Init(mTestRatingList);
    RunningAverage<double> raPrecision;
    RunningAverage<double> raRecall;
    int listSize = mConfig["rankingListSize"].asInt();
    for (int userId = 0; userId < ratingMatrix.NumUser(); ++userId) {
        const auto& uv = ratingMatrix.GetUserVector(userId);
        double userPrecision, userRecall;
        EvaluateRankingPerUser(userId, listSize, uv.Data(), uv.Size(), &userPrecision, &userRecall);
        raPrecision.Add(userPrecision);
        raRecall.Add(userRecall);
    }
    mPrecision = raPrecision.CurrentAverage();
    mRecall = raRecall.CurrentAverage();
}

void BasicEvaluate::EvaluateRankingPerUser(int userId, int listSize,
        const ItemRating* truthDataBegin, int truthDataSize, double* precision,
        double* recall) {
    ItemIdList recommendedItemList = mPredict->PredictTopNItem(userId, listSize);
    assert(recommendedItemList.size() == listSize);
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
    *precision = static_cast<double>(hitCount) / listSize;
    *recall = (truthDataSize == 0) ? 0.0 : static_cast<double>(hitCount) / truthDataSize;
}

void BasicEvaluate::WriteResult() {
    Log::I("recsys", "BasicEvaluate::WriteResult()");
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

void BasicEvaluate::DestroyPredict() {
    mPredict->Cleanup();
    delete mPredict;
    mPredict = nullptr;
}

} //~ namespace longan
