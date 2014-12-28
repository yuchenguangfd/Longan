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

void SVDEvaluate::LoadTestRatings() {
    Log::I("recsys", "SVDEvaluate::LoadTestRatings()");
    BinaryInputStream bis(mRatingTestFilepath);
    int numUser, numItem;
    int64 numRating;
    float avgRating;
    bis >> numUser >> numItem >> numRating
        >> avgRating;
    RatingList rlist(numUser, numItem, numRating);
    for (int i = 0; i < numRating; ++i) {
        int uid, iid;
        float rating;
        bis >> uid >> iid >> rating;
        rlist.Add(RatingRecord(uid, iid, rating));
    }
    mTestRatingList = std::move(rlist);
}

} //~ namespace longan
