/*
 * user_based_train.cpp
 * Created on: Nov 18, 2014
 * Author: chenguangyu
 */

#include "user_based_train.h"
#include "recsys/base/rating_adjust.h"
#include "common/system/file_util.h"
#include "common/logging/logging.h"
#include "common/system/system_info.h"
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
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    Log::I("recsys", "create rating matrix");
    mRatingMatrix = new RatingMatrixAsUsers<>();
    mRatingMatrix->Init(rlist);
    Log::I("recsys", "create rating trait");
    mRatingTrait = new RatingTrait();
    mRatingTrait->Init(rlist);
}

void UserBasedTrain::AdjustRating() {
    Log::I("recsys", "UserBasedTrain::AdjustRating()");
    if (mConfig["parameter"]["similarityType"].asString() == "adjustedCosine") {
        AdjustRatingByMinusItemAverage(*mRatingTrait, mRatingMatrix);
    } else if (mConfig["parameter"]["similarityType"].asString() == "correlation") {
        AdjustRatingByMinusUserAverage(*mRatingTrait, mRatingMatrix);
    } else { // if (mConfig["parameter"]["similarityType"].asString() == "cosine")
    }
}

void UserBasedTrain::InitModel() {
    Log::I("recsys", "UserBasedTrain::InitModel()");
    if (mConfig["parameter"]["modelType"].asString() == "fixedNeighborSize") {
        int neighborSize = mConfig["parameter"]["neighborSize"].asInt();
        mModel = new UserBased::FixedNeighborSizeModel(mRatingMatrix->NumUser(), neighborSize);
    } else if (mConfig["parameter"]["modelType"].asString() == "fixedSimilarityThreshold") {
        float threshold = static_cast<float>(mConfig["parameter"]["similarityThreshold"].asDouble());
        mModel = new UserBased::FixedSimilarityThresholdModel(mRatingMatrix->NumUser(), threshold);
    } else {
        throw LonganConfigError("No Such modelType");
    }
}

void UserBasedTrain::ComputeModel() {
    Log::I("recsys", "UserBasedTrain::ComputeModel()");
      if (mConfig["trainOption"]["accelerate"].asBool()) {
          int numThread = mConfig["trainOption"]["numThread"].asInt();
          if (numThread == 0) {
              numThread = SystemInfo::GetNumCPUCore();
          }
          mModelComputationDelegate = new UserBased::DynamicScheduledModelComputation(numThread);
      } else {
          mModelComputationDelegate = new UserBased::SimpleModelComputation();
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
