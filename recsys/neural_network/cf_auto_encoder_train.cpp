/*
 * cf_auto_encoder_train.cpp
 * Created on: Feb 3, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder_train.h"
#include "cf_auto_encoder.h"
#include "recsys/base/rating_list.h"
#include "recsys/base/rating_adjust.h"
#include "common/logging/logging.h"
#include "common/error.h"

namespace longan {

void CFAutoEncoderTrain::Train() {
    LoadConfig();
    LoadRatings();
    InitModel();
    TrainModel();
    SaveModel();
    Cleanup();
}

void CFAutoEncoderTrain::LoadRatings() {
    Log::I("recsys", "CFAutoEncoderTrain::LoadRatings()");
    Log::I("recsys", "training rating file = " + mRatingTrainFilepath);
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    Log::I("recsys", "create rating matrix");
    if (mConfig["parameter"]["encodeFeature"].asString() == "user") {
        mRatingMatrixAsUsers = new RatingMatrixAsUsers<>();
        mRatingMatrixAsUsers->Init(rlist);
    } else {
        throw LonganConfigError();
    }
}

void CFAutoEncoderTrain::AdjustRatings() {
//    if (mConfig["parameter"]["ratingRepresentBy"].asString() == "softMax") {
    for (int uid = 0; uid < mRatingMatrixAsUsers->NumUser(); ++uid) {
        auto& uvec = mRatingMatrixAsUsers->GetUserVector(uid);
        ItemRating *data = uvec.Data();
        int size = uvec.Size();
        for (int i = 0; i < size; ++i) {
            auto& ir = data[i];
            ir.SetRating(ir.Rating() - 1.0);
        }
    }
//    }
//    Log::I("recsys", "CFAutoEncoderTrain::AdjustRatings()");
//    if (mConfig["parameter"]["encodeFeature"].asString() == "user") {
//        AdjustRatingByMinusUserAverage(*mRatingTrait, mRatingMatrixAsUsers);
//        AdjustRatingByNormalizeUserMaxAbs(mRatingMatrixAsUsers);
//    } else if (mConfig["parameter"]["encodeFeature"].asString() == "item") {
//        AdjustRatingByMinusItemAverage(*mRatingTrait, mRatingMatrixAsItems);
//        // TODO
//    } else {
//        throw LonganConfigError();
//    }
}

void CFAutoEncoderTrain::InitModel() {
    Log::I("recsys", "CFAutoEncoderTrain::InitModel()");
    CFAE::Architecture arch(mConfig["architecture"]);
    mAutoEncoder = new CFAutoEncoder(arch);
}

void CFAutoEncoderTrain::TrainModel() {
    Log::I("recsys", "CFAutoEncoderTrain::TrainModel()");
    CFAE::Parameter parameter(mConfig["parameter"]);
    CFAE::TrainOption option(mConfig["trainOption"]);
    if (mConfig["parameter"]["encodeFeature"].asString() == "user") {
        mAutoEncoder->Train(&parameter, &option, mRatingMatrixAsUsers);
    } else {
        throw LonganConfigError();
    }
}

void CFAutoEncoderTrain::SaveModel() {
    Log::I("recsys", "CFAutoEncoderTrain::SaveModel()");
    Log::I("recsys", "model file = " + mModelFilepath);
    mAutoEncoder->Save(mModelFilepath);
}

void CFAutoEncoderTrain::Cleanup() {
    delete mRatingMatrixAsUsers;
    delete mRatingMatrixAsItems;
//    delete mRatingTrait;
    delete mAutoEncoder;
}

} //~ namespace longan
