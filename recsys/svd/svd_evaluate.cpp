/*
 * svd_evaluate.cpp
 * Created on: Dec 12, 2014
 * Author: chenguangyu
 */

#include "svd_evaluate.h"
#include "svd_predict.h"
#include "common/logging/logging.h"
#include "common/lang/binary_input_stream.h"

namespace longan {

void SVDEvaluate::CreatePredict() {
    Log::I("recsys", "SVDEvaluate::CreatePredict()");
    mPredict = new SVDPredict(mRatingTrainFilepath, mConfigFilepath, mModelFilepath);
    mPredict->Init();
}

RatingList* SVDEvaluate::LoadRatingData(const std::string& filename) {
    BinaryInputStream bis(filename);
    int numRating, numUser, numItem;
    float avgRating;
    bis >> numRating >> numUser >> numItem
        >> avgRating;
    RatingList rlist(numUser, numItem, numRating);
    for (int i = 0; i < numRating; ++i) {
        int uid, iid;
        float rating;
        bis >> uid >> iid >> rating;
        rlist.Add(RatingRecord(uid, iid, rating));
    }
    return new RatingList(std::move(rlist));
}

void SVDEvaluate::LoadTrainData() {
    if (mOption->EvaluateNovelty()) {
        Log::I("recsys", "SVDEvaluate::LoadTrainData()");
        Log::I("recsys", "train rating file = " + mRatingTrainFilepath);
        mTrainData = LoadRatingData(mRatingTrainFilepath);
    }
}

void SVDEvaluate::LoadTestData() {
    if (mOption->EvaluateRating() || mOption->EvaluateRanking()
        || mOption->EvaluateCoverage() || mOption->EvaluateDiversity()) {
        Log::I("recsys", "SVDEvaluate::LoadTestData()");
        Log::I("recsys", "test rating file = " + mRatingTestFilepath);
        mTestData = LoadRatingData(mRatingTestFilepath);
    }
}

} //~ namespace longan
