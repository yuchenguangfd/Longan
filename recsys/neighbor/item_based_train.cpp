/*
 * item_based_train.cpp
 * Created on: Oct 18, 2014
 * Author: chenguangyu
 */

#include "item_based_train.h"
#include "recsys/base/rating_list_loader.h"
#include "common/system/file_util.h"
#include "common/logging/logging.h"
#include "common/error.h"

namespace longan {

ItemBasedTrain::ItemBasedTrain(const std::string& trainRatingFilepath,
        const std::string& configFilepath, const std::string& modelFilepath) :
    mTrainRatingFilepath(trainRatingFilepath),
    mConfigFilepath(configFilepath),
    mModelFilepath(modelFilepath),
    mRatingMatrix(nullptr),
    mRatingTrait(nullptr),
    mModel(nullptr),
    mModelComputationDelegate(nullptr) { }

ItemBasedTrain::~ItemBasedTrain() { }

void ItemBasedTrain::Train() {
    LoadConfig();
    LoadRatings();
    AdjustRating();
    InitModel();
    ComputeModel();
    SaveModel();
    Cleanup();
}

void ItemBasedTrain::LoadConfig() {
    Log::I("recsys", "ItemBasedTrain::LoadConfig()");
    Log::I("recsys", "config file = " + mConfigFilepath);
    std::string content = FileUtil::LoadFileContent(mConfigFilepath);
    Json::Reader reader;
    if (!reader.parse(content, mConfig)) {
        throw LonganFileFormatError();
    }
}

void ItemBasedTrain::LoadRatings() {
    Log::I("recsys", "LoadRatings()");
    Log::I("recsys", "rating file = " + mTrainRatingFilepath);
    RatingList rlist = RatingListLoader::Load(mTrainRatingFilepath);
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
    Log::I("recsys", "InitModel()");
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
    Log::I("recsys", "ComputeModel()");
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
    Log::I("recsys", "SaveModel");
    mModel->Save(mModelFilepath);
}

void ItemBasedTrain::Cleanup() {
    delete mModelComputationDelegate;
    delete mModel;
    delete mRatingTrait;
    delete mRatingMatrix;
}

} //~ namespace longan
