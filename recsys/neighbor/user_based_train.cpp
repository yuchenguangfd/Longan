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

UserBasedTrain::UserBasedTrain(const std::string& trainRatingFilepath,
        const std::string& configFilepath, const std::string& modelFilepath) :
    mTrainRatingFilepath(trainRatingFilepath),
    mConfigFilepath(configFilepath),
    mModelFilepath(modelFilepath),
    mRatingMatrix(nullptr),
    mRatingTrait(nullptr),
    mModel(nullptr),
    mModelComputationDelegate(nullptr) { }

UserBasedTrain::~UserBasedTrain() { }

void UserBasedTrain::Train() {
    LoadConfig();
    LoadRatings();
    AdjustRating();
    InitModel();
    ComputeModel();
    SaveModel();
    Cleanup();
}

void UserBasedTrain::LoadConfig() {
    Log::I("recsys", "UserBasedTrain::LoadConfig()");
    Log::I("recsys", "config file = " + mConfigFilepath);
    std::string content = FileUtil::LoadFileContent(mConfigFilepath);
    Json::Reader reader;
    if (!reader.parse(content, mConfig)) {
        throw LonganFileFormatError();
    }
}

void UserBasedTrain::LoadRatings() {
    Log::I("recsys", "LoadRatings()");
    Log::I("recsys", "rating file = " + mTrainRatingFilepath);
    RatingList rlist = RatingListLoader::Load(mTrainRatingFilepath);
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
    Log::I("recsys", "AdjustRatingByMinusItemAverage()");
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
    Log::I("recsys", "AdjustRatingByMinusUserAverage()");
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
    Log::I("recsys", "InitModel()");
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
    Log::I("recsys", "ComputeModel()");
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
    Log::I("recsys", "SaveModel");
    mModel->Save(mModelFilepath);
}

void UserBasedTrain::Cleanup() {
    delete mModelComputationDelegate;
    delete mModel;
    delete mRatingTrait;
    delete mRatingMatrix;
}

} //~ namespace longan
