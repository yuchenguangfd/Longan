/*
 * user_based_train.cpp
 * Created on: Nov 18, 2014
 * Author: chenguangyu
 */

#include "user_based_train.h"
#include "recsys/base/rating_adjust.h"
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
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    Log::I("recsys", "create rating matrix");
    mRatingMatrix = new RatingMatrixAsUsers<>();
    mRatingMatrix->Init(rlist);
    Log::I("recsys", "create rating trait");
    mRatingTrait = new RatingTrait();
    mRatingTrait->Init(rlist);
}

void UserBasedTrain::AdjustRating() {
    if (mConfig["similarityType"].asString() == "adjustedCosine") {
        AdjustRatingByMinusItemAverage(*mRatingTrait, mRatingMatrix);
    } else if (mConfig["similarityType"].asString() == "correlation") {
        AdjustRatingByMinusUserAverage(*mRatingTrait, mRatingMatrix);
    }
}

void UserBasedTrain::InitModel() {
    Log::I("recsys", "UserBasedTrain::InitModel()");
    if (mConfig["modelType"].asString() == "fixedNeighborSize") {
        int neighborSize = mConfig["neighborSize"].asInt();
        mModel = new UserBased::FixedNeighborSizeModel(mRatingMatrix->NumUser(), neighborSize);
    } else if (mConfig["modelType"].asString() == "fixedSimilarityThreshold") {
        float threshold = (float)mConfig["similarityThreshold"].asDouble();
        mModel = new UserBased::FixedSimilarityThresholdModel(mRatingMatrix->NumUser(), threshold);
    } else {
        throw LonganConfigError("No Such modelType");
    }
}

void UserBasedTrain::ComputeModel() {
    Log::I("recsys", "UserBasedTrain::ComputeModel()");
      if (mConfig["modelComputation"].asString() == "simple") {
          mModelComputationDelegate = new UserBased::SimpleModelComputation();
      } else if (mConfig["modelComputation"].asString() == "staticScheduled") {
          mModelComputationDelegate = new UserBased::StaticScheduledModelComputation();
      } else if (mConfig["modelComputation"].asString() == "dynamicScheduled") {
          mModelComputationDelegate = new UserBased::DynamicScheduledModelComputation();
      } else {
          mModelComputationDelegate = new UserBased::DynamicScheduledModelComputation();
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
