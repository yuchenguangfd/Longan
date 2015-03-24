/*
 * character_dataset.cpp
 * Created on: Mar 11, 2015
 * Author: chenguangyu
 */

#include "character_dataset.h"
#include "common/common.h"

namespace longan {

CharacterDataset::CharacterDataset(const std::string& ratingFilepath, const std::string& resultFilepath) :
    mRatingFilepath(ratingFilepath),
    mResultFilepath(resultFilepath) { }

void CharacterDataset::Character() {
    Log::I("recsys", "CharacterDataset::Character()");
    Log::I("recsys", "load rating file = " + mRatingFilepath);
    mRatingData = RatingList::LoadFromTextFile(mRatingFilepath);
    mRatingTrait.Init(mRatingData);
    ReportMeta();
    ReportUserActivity();
    ReportItemPopularity();
    WriteResult();
}

void CharacterDataset::ReportMeta() {
    Json::Value & meta = mResult["meta"];
    meta["numUser"] = mRatingData.NumUser();
    meta["numItem"] = mRatingData.NumItem();
    meta["numRating"] = mRatingData.NumRating();
    meta["sparsity"] = mRatingData.Sparsity();
    meta["minRating"] = mRatingTrait.Min();
    meta["maxRating"] = mRatingTrait.Max();
    meta["averageRating"] = mRatingTrait.Average();
    meta["stdRating"] = mRatingTrait.Std();
}

void CharacterDataset::ReportUserActivity() {
    const int K = 10;
    RunningMinK<int> runningMinK(K);
    RunningMaxK<int> runningMaxK(K);
    RunningStd<float> runningStd;
    for (int uid = 0; uid < mRatingData.NumUser(); ++uid) {
        int act = mRatingTrait.UserActivity(uid);
        runningMinK.Add(act);
        runningMaxK.Add(act);
        runningStd.Add(act);
    }
    Json::Value& userActivity = mResult["userActivity"];
    userActivity["averageActivity"] = runningStd.CurrentAverage();
    userActivity["stdActivity"] = runningStd.CurrentStd();
    std::vector<int> minK = runningMinK.CurrentMinK();
    for (int act : minK) {
        userActivity["minActivities"].append(act);
    }
    std::vector<int> maxK = runningMaxK.CurrentMaxK();
    for (int act : maxK) {
        userActivity["maxActivities"].append(act);
    }
}

void CharacterDataset::ReportItemPopularity() {
    const int K = 10;
    RunningMinK<int> runningMinK(K);
    RunningMaxK<int> runningMaxK(K);
    RunningStd<float> runningStd;
    for (int iid = 0; iid < mRatingData.NumItem(); ++iid) {
        int pop = mRatingTrait.ItemPopularity(iid);
        runningMinK.Add(pop);
        runningMaxK.Add(pop);
        runningStd.Add(pop);
    }
    Json::Value& itemPopularity = mResult["itemPopularity"];
    itemPopularity["averagePopularity"] = runningStd.CurrentAverage();
    itemPopularity["stdPopularity"] = runningStd.CurrentStd();
    std::vector<int> minK = runningMinK.CurrentMinK();
    for (int pop : minK) {
        itemPopularity["minPopularities"].append(pop);
    }
    std::vector<int> maxK = runningMaxK.CurrentMaxK();
    for (int pop : maxK) {
        itemPopularity["maxPopularities"].append(pop);
    }
}

void CharacterDataset::WriteResult() {
    Log::I("recsys", "CharacterDataset::WriteResult()");
    Log::I("recsys", "result = \n" + mResult.toStyledString());
    Log::I("recsys", "writing result to file = " + mResultFilepath);
    JsonConfigHelper::WriteToFile(mResultFilepath, mResult);
}

} //~ namespace longan
