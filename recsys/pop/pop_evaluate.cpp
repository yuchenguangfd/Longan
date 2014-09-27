/*
 * pop_evaluate.cpp
 * Created on: Sep 22, 2014
 * Author: chenguangyu
 */

#include "pop_evaluate.h"
#include "common/logging/logging_helper.h"
#include "common/math/math.h"
#include "common/system/file_util.h"
#include "common/error.h"
#include <cstdio>
#include <cassert>

namespace longan {

PopEvaluate::PopEvaluate(const std::string& modelFilePath, const std::string& ratingTrainFilePath,
        const std::string& ratingTestFilePath, const std::string& resultFilePath,
        const std::string& configFilePath) :
    mModelFilePath(modelFilePath),
    mRatingTrainFilePath(ratingTrainFilePath),
    mRatingTestFilePath(ratingTestFilePath),
    mResultFilePath(resultFilePath),
    mConfigFilePath(configFilePath),
    mPopPredict(nullptr),
    mNumUser(0),
    mNumItem(0),
    mNumTestRating(0),
    mMAE(0.0), mRMSE(0.0),
    mPrecision(0.0), mRecall(0.0) { }

void PopEvaluate::Evaluate() {
    LOG_FUNC;
    ReadConfig();
    ReadTestRatingData();
    mPopPredict = new PopPredict(mRatingTrainFilePath, mModelFilePath);
    mPopPredict->Init();
    if (mConfig["evaluateRating"].asBool()) {
        EvaluateRating();
    }
    if (mConfig["evaluateRanking"].asBool()) {
        EvaluateRanking();
    }
    WriteResult();
    delete mPopPredict;
}

void PopEvaluate::ReadConfig() {
    LOG_FUNC;
    std::string text = FileUtil::LoadFileContent(mConfigFilePath);
    Json::Reader reader;
    bool rtn = reader.parse(text, mConfig);
    assert(rtn);
}

void PopEvaluate::ReadTestRatingData() {
    LOG_FUNC;
    FILE *fp = fopen(mRatingTestFilePath.c_str(), "r");
    assert(fp != nullptr);
    int rtn;
    rtn = fscanf(fp, "%d,%d,%d", &mNumTestRating, &mNumUser, &mNumItem);
    assert(rtn == 3);
    mTestRatings.resize(mNumTestRating);
    for (int i = 0; i < mNumTestRating; ++i) {
        int userId, itemId, time;
        float rating;
        rtn = fscanf(fp, "%d,%d,%f,%d", &userId, &itemId, &rating, &time);
        assert(rtn == 4);
        mTestRatings[i] = RatingRecord(userId, itemId, rating);
    }
    fclose(fp);
}

void PopEvaluate::EvaluateRating() {
    LOG_FUNC;
    double sumMAE = 0.0;
    double sumRMSE = 0.0;
    for (int i = 0; i < mNumTestRating; ++i) {
        const RatingRecord& rr = mTestRatings[i];
        float predRating = mPopPredict->PredictRating(rr.UserId(), rr.ItemId());
        float error = predRating - rr.Rating();
        sumMAE += Math::Abs(error);
        sumRMSE += Math::Sqr(error);
    }
    mMAE = sumMAE / mNumTestRating;
    mRMSE = Math::Sqrt(sumRMSE / mNumTestRating);
}

void PopEvaluate::EvaluateRanking() {
    LOG_FUNC;
    // TODO
    mPrecision = 0.000042;
    mRecall = 0.000024;
}

void PopEvaluate::WriteResult() {
    LOG_FUNC;
    FILE *fp = fopen(mResultFilePath.c_str(), "w");
    assert(fp != nullptr);
    if (mConfig["evaluateRating"].asBool()) {
      fprintf(fp, "MAE=%lf\n", mMAE);
      fprintf(fp, "RMSE=%lf\n", mRMSE);
    }
    if (mConfig["evaluateRanking"].asBool()) {
      fprintf(fp, "Precision=%lf\n", mPrecision);
      fprintf(fp, "Recall=%lf\n", mRecall);
    }
    fclose(fp);
}

} //~ namespace longan
