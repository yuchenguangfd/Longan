/*
 * user_based_train.cpp
 * Created on: Nov 18, 2014
 * Author: chenguangyu
 */

#include "user_based_train.h"
#include "recsys/base/rating_list_loader.h"
#include "common/system/file_util.h"
#include "common/logging/logging.h"
#include "common/error.h"

namespace longan {

void UserBasedTrain::Train() {
    LoadConfig();
    LoadRatings();
    AdjustRating();
    InitModel();
    ComputeModel();
    SaveModel();
    Cleanup();
}

void UserBasedTrain::LoadRatings() {
    Log::I("recsys", "UserBasedTrain::LoadRatings()");
    Log::I("recsys", "rating file = " + mRatingTrainFilepath);
    RatingList rlist = RatingListLoader::Load(mRatingTrainFilepath);
    Log::I("recsys", "create rating matrix");
    mRatingMatrix = new RatingMatrixAsUsers<>();
    mRatingMatrix->Init(rlist);
    Log::I("recsys", "create rating trait");
    mRatingTrait = new RatingTrait();
    mRatingTrait->Init(rlist);
}

void UserBasedTrain::AdjustRating() {
    if (mConfig["similarityType"].asString() == "adjustedCosine") {
        AdjustRatingByMinusItemAverage();
    } else if (mConfig["similarityType"].asString() == "correlation") {
        AdjustRatingByMinusUserAverage();
    }
}

void UserBasedTrain::AdjustRatingByMinusItemAverage() {
    Log::I("recsys", "UserBasedTrain::AdjustRatingByMinusItemAverage()");
    for (int uid = 0; uid < mRatingMatrix->NumUser(); ++uid) {
        auto& uvec = mRatingMatrix->GetUserVector(uid);
        for (int i = 0; i < uvec.Size(); ++i) {
            auto& ur = uvec.Data()[i];
            float iavg = mRatingTrait->ItemAverage(ur.ItemId());
            ur.SetRating(ur.Rating() - iavg);
        }
    }
}

void UserBasedTrain::AdjustRatingByMinusUserAverage() {
    Log::I("recsys", "UserBasedTrain::AdjustRatingByMinusUserAverage()");
    for (int uid = 0; uid < mRatingMatrix->NumUser(); ++uid) {
        auto& uvec = mRatingMatrix->GetUserVector(uid);
        float uavg = mRatingTrait->UserAverage(uid);
        for (int i = 0; i < uvec.Size(); ++i) {
            auto& ur = uvec.Data()[i];
            ur.SetRating(ur.Rating() - uavg);
        }
    }
}

void UserBasedTrain::InitModel() {
    Log::I("recsys", "UserBasedTrain::InitModel()");
    if (mConfig["modelType"].asString() == "fixedNeighborSize") {
        int neighborSize = mConfig["neighborSize"].asInt();
        mModel = new user_based::FixedNeighborSizeModel(mRatingMatrix->NumUser(), neighborSize);
    } else if (mConfig["modelType"].asString() == "fixedSimilarityThreshold") {
        float threshold = (float)mConfig["similarityThreshold"].asDouble();
        mModel = new user_based::FixedSimilarityThresholdModel(mRatingMatrix->NumUser(), threshold);
    } else {
        throw LonganConfigError("No Such modelType");
    }
}

void UserBasedTrain::ComputeModel() {
    Log::I("recsys", "UserBasedTrain::ComputeModel()");
      if (mConfig["modelComputation"].asString() == "simple") {
          mModelComputationDelegate = new user_based::SimpleModelComputation();
      } else if (mConfig["modelComputation"].asString() == "staticScheduled") {
          mModelComputationDelegate = new user_based::StaticScheduledModelComputation();
      } else if (mConfig["modelComputation"].asString() == "dynamicScheduled") {
          mModelComputationDelegate = new user_based::DynamicScheduledModelComputation();
      } else {
          mModelComputationDelegate = new user_based::DynamicScheduledModelComputation();
      }
      mModelComputationDelegate->ComputeModel(mRatingMatrix, mModel);
}

void UserBasedTrain::SaveModel() {
    Log::I("recsys", "UserBasedTrain::SaveModel()");
    mModel->Save(mModelFilepath);
}

void UserBasedTrain::Cleanup() {
    Log::I("recsys", "UserBasedTrain::Cleanup()");
    delete mModelComputationDelegate;
    delete mModel;
    delete mRatingTrait;
    delete mRatingMatrix;
}

} //~ namespace longan
