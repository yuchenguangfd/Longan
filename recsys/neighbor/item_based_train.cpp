/*
 * item_based_train.cpp
 * Created on: Oct 18, 2014
 * Author: chenguangyu
 */

#include "item_based_train.h"
#include "recsys/base/rating_list_loader.h"
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
    if (mConfig["similarityType"].asString() == "adjustedCosine") {
        AdjustRatingByMinusUserAverage();
    } else if (mConfig["similarityType"].asString() == "correlation") {
        AdjustRatingByMinusItemAverage();
    }
}

void ItemBasedTrain::AdjustRatingByMinusItemAverage() {
    Log::I("recsys", "AdjustRatingByMinusItemAverage()");
    for (int iid = 0; iid < mRatingMatrix->NumItem(); ++iid) {
        auto& ivec = mRatingMatrix->GetItemVector(iid);
        float iavg = mRatingTrait->ItemAverage(iid);
        for (int i = 0; i < ivec.Size(); ++i) {
            auto& ur = ivec.Data()[i];
            ur.SetRating(ur.Rating() - iavg);
        }
    }
}

void ItemBasedTrain::AdjustRatingByMinusUserAverage() {
    Log::I("recsys", "AdjustRatingByMinusUserAverage()");
    for (int iid = 0; iid < mRatingMatrix->NumItem(); ++iid) {
        auto& ivec = mRatingMatrix->GetItemVector(iid);
        for (int i = 0; i < ivec.Size(); ++i) {
            auto& ur = ivec.Data()[i];
            float uavg = mRatingTrait->UserAverage(ur.UserId());
            ur.SetRating(ur.Rating() - uavg);
        }
    }
}

void ItemBasedTrain::InitModel() {
    Log::I("recsys", "ItemBasedTrain::InitModel()");
    if (mConfig["modelType"].asString() == "fixedNeighborSize") {
        int neighborSize = mConfig["neighborSize"].asInt();
        mModel = new item_based::FixedNeighborSizeModel(mRatingMatrix->NumItem(), neighborSize);
    } else if (mConfig["modelType"].asString() == "fixedSimilarityThreshold") {
        float threshold = (float)mConfig["similarityThreshold"].asDouble();
        mModel = new item_based::FixedSimilarityThresholdModel(mRatingMatrix->NumItem(), threshold);
    } else {
        throw LonganConfigError("No Such modelType");
    }
}

void ItemBasedTrain::ComputeModel() {
    Log::I("recsys", "ItemBasedTrain::ComputeModel()");
    if (mConfig["modelComputation"].asString() == "simple") {
        mModelComputationDelegate = new item_based::SimpleModelComputation();
    } else if (mConfig["modelComputation"].asString() == "staticScheduled") {
        mModelComputationDelegate = new item_based::StaticScheduledModelComputation();
    } else if (mConfig["modelComputation"].asString() == "dynamicScheduled") {
        mModelComputationDelegate = new item_based::DynamicScheduledModelComputation();
    } else {
        mModelComputationDelegate = new item_based::DynamicScheduledModelComputation();
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
