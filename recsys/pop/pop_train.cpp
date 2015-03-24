/*
 * pop_train.cpp
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#include "pop_train.h"
#include "recsys/base/rating_list.h"
#include "recsys/base/rating_trait.h"
#include "common/logging/logging.h"
#include "common/lang/binary_output_stream.h"

namespace longan {

void PopTrain::Train() {
    Log::I("recsys", "PopTrain::Train()");
    Log::I("recsys", "loading train rating from file %s...", mRatingTrainFilepath.c_str());
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    Log::I("recsys", "computing item popularity & average...");
    RatingTrait rtrait;
    rtrait.Init(rlist);
    Log::I("recsys", "saving model to file %s...", mModelFilepath.c_str());
    BinaryOutputStream bos(mModelFilepath);
    bos << rlist.NumItem();
    for (int iid = 0; iid < rlist.NumItem(); ++iid) {
        bos << rtrait.ItemAverage(iid);
    }
    for (int iid = 0; iid < rlist.NumItem(); ++iid) {
        bos << rtrait.ItemPopularity(iid);
    }
    bos << rlist.NumUser();
    for (int uid = 0; uid < rlist.NumUser(); ++uid) {
        bos << rtrait.UserAverage(uid);
    }
    for (int uid = 0; uid < rlist.NumUser(); ++uid) {
        bos << rtrait.UserActivity(uid);
    }
}

} //~ namespace longan
