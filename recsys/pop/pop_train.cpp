/*
 * pop_train.cpp
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#include "pop_train.h"
#include "recsys/base/rating_list_loader.h"
#include "common/logging/logging.h"
#include "common/lang/binary_output_stream.h"
#include "common/error.h"
#include <cstdio>
#include <cassert>

namespace longan {

PopTrain::PopTrain(const std::string& trainRatingFilepath, const std::string& configFilepath,
        const std::string& modelFilepath) :
    BasicTrain(trainRatingFilepath, configFilepath, modelFilepath) { }

void PopTrain::Train() {
    Log::I("recsys", "PopTrain::Train()");
    Log::I("recsys", "loading train rating, file = " + mRatingTrainFilepath);
    mRatingList = RatingListLoader::Load(mRatingTrainFilepath);
    Log::I("recsys", "compute average rating of items");
    mItemsAverage.resize(mRatingList.NumItem());
    for (int i = 0; i < mRatingList.NumRating(); ++i) {
        const RatingRecord& rr = mRatingList[i];
        mItemsAverage[rr.ItemId()].Add(rr.Rating());
    }
    SaveModel();
}

/**
 * Model file(a binary file) format layout:
 * numItem(int)|ItemAvg_0|ItemAvg_1|...|ItemAvg_n-1(float)
 */
void PopTrain::SaveModel() {
    Log::I("recsys", "saving model, file = " + mModelFilepath);
    BinaryOutputStream bos(mModelFilepath);
    bos << mRatingList.NumItem();
    for (int i = 0; i < mItemsAverage.size(); ++i) {
        bos << mItemsAverage[i].CurrentAverage();
    }
}

} //~ namespace longan
