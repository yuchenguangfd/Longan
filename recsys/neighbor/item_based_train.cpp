/*
 * item_based_train.cpp
 * Created on: Oct 18, 2014
 * Author: chenguangyu
 */

#include "item_based_train.h"
#include "recsys/base/rating_list_loader.h"
#include "recsys/base/rating_adjust.h"
#include "common/logging/logging.h"
#include "common/error.h"

namespace longan {

void ItemBasedTrain::Train() {
    LoadConfig();
    LoadRatings();
    AdjustRating();
    InitModel();
    ComputeModel();
    SaveModel();
    Cleanup();
}

void ItemBasedTrain::LoadRatings() {
    Log::I("recsys", "ItemBasedTrain::LoadRatings()");
    Log::I("recsys", "rating file = " + mRatingTrainFilepath);
    RatingList rlist = RatingListLoader::Load(mRatingTrainFilepath);
    Log::I("recsys", "create rating matrix");
    mRatingMatrix = new RatingMatrixAsItems<>();
    mRatingMatrix->Init(rlist);
    Log::I("recsys", "create rating trait");
    mRatingTrait = new RatingTrait();
    mRatingTrait->Init(rlist);
}

void ItemBasedTrain::AdjustRating() {
    Log::I("recsys", "ItemBasedTrain::AdjustRating()");
    if (mConfig["similarityType"].asString() == "adjustedCosine") {
        AdjustRatingByMinusUserAverage(*mRatingTrait, mRatingMatrix);
    } else if (mConfig["similarityType"].asString() == "correlation") {
        AdjustRatingByMinusItemAverage(*mRatingTrait, mRatingMatrix);
    }
}

void ItemBasedTrain::InitModel() {
    Log::I("recsys", "ItemBasedTrain::InitModel()");
    if (mConfig["modelType"].asString() == "fixedNeighborSize") {
        int neighborSize = mConfig["neighborSize"].asInt();
        mModel = new ItemBased::FixedNeighborSizeModel(mRatingMatrix->NumItem(), neighborSize);
    } else if (mConfig["modelType"].asString() == "fixedSimilarityThreshold") {
        float threshold = (float)mConfig["similarityThreshold"].asDouble();
        mModel = new ItemBased::FixedSimilarityThresholdModel(mRatingMatrix->NumItem(), threshold);
    } else {
        throw LonganConfigError("No Such modelType");
    }
}

void ItemBasedTrain::ComputeModel() {
    Log::I("recsys", "ItemBasedTrain::ComputeModel()");
    if (mConfig["modelComputation"].asString() == "simple") {
        mModelComputationDelegate = new ItemBased::SimpleModelComputation();
    } else if (mConfig["modelComputation"].asString() == "staticScheduled") {
        mModelComputationDelegate = new ItemBased::StaticScheduledModelComputation();
    } else if (mConfig["modelComputation"].asString() == "dynamicScheduled") {
        mModelComputationDelegate = new ItemBased::DynamicScheduledModelComputation();
    } else {
        mModelComputationDelegate = new ItemBased::DynamicScheduledModelComputation();
    }
    mModelComputationDelegate->ComputeModel(mRatingMatrix, mModel);
}

void ItemBasedTrain::SaveModel() {
    Log::I("recsys", "ItemBasedTrain::SaveModel()");
    mModel->Save(mModelFilepath);
}

void ItemBasedTrain::Cleanup() {
    Log::I("recsys", "ItemBasedTrain::Clueanup()");
    delete mModelComputationDelegate;
    delete mModel;
    delete mRatingTrait;
    delete mRatingMatrix;
}

} //~ namespace longan
