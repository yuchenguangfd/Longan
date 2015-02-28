/*
 * svd_train.cpp
 * Created on: Dec 13, 2014
 * Author: chenguangyu
 */

#include "svd_train.h"
#include "svd_model_computation.h"
#include "common/util/array_helper.h"
#include "common/logging/logging.h"

namespace longan {

SVDTrain::SVDTrain(const std::string& ratingTrainFilepath,
        const std::string& ratingValidateFilepath,
        const std::string& configFilepath, const std::string& modelFilepath) :
    BasicTrain(ratingTrainFilepath, configFilepath, modelFilepath),
    mRatingValidateFilepath(ratingValidateFilepath) { }

void SVDTrain::Train() {
    LoadConfig();
    CreateTrainOption();
    CreateParameter();
    InitModel();
    LoadRatings();
    ComputeModel();
    SaveModel();
    Cleanup();
}

void SVDTrain::CreateTrainOption() {
    Log::I("recsys", "SVDTrain::CreateTrainOption()");
    mTrainOption = new SVD::TrainOption(mConfig["trainOption"]);
}

void SVDTrain::CreateParameter() {
    Log::I("recsys", "SVDTrain::CreateParameter()");
    mParameter = new SVD::Parameter(mConfig["parameter"]);
}

void SVDTrain::InitModel() {
    Log::I("recsys", "SVDTrain::InitModel()");
    SVD::Matrix metaMat = SVD::Matrix::LoadFromBinaryFile(mRatingTrainFilepath, true);
    mModel = new SVD::ModelTrain(mParameter, metaMat.NumUser(), metaMat.NumItem(), metaMat.RatingAverage());
    mModel->RandomInit();
}

void SVDTrain::LoadRatings() {
    Log::I("recsys", "SVDTrain::LoadRatings()");
    GenerateIdMapping(mModel->NumUser(), &mUserIdMapping);
    GenerateIdMapping(mModel->NumItem(), &mItemIdMapping);
    Log::I("recsys", "train rating file = " + mRatingTrainFilepath);
    mTrainData = new SVD::GriddedMatrix(SVD::GriddedMatrix::LoadFromBinaryFile(mRatingTrainFilepath,
            mTrainOption->NumUserBlock(), mTrainOption->NumItemBlock(),
            mUserIdMapping, mItemIdMapping));
    Log::I("recsys", "validate rating file = " + mRatingValidateFilepath);
    mValidateData = new SVD::GriddedMatrix(SVD::GriddedMatrix::LoadFromBinaryFile(mRatingValidateFilepath,
            mTrainOption->NumUserBlock(), mTrainOption->NumItemBlock(),
            mUserIdMapping, mItemIdMapping));
}

void SVDTrain::GenerateIdMapping(int size, std::vector<int> *idMapping) {
    idMapping->resize(size);
    ArrayHelper::FillRange(idMapping->data(), idMapping->size());
    if (mTrainOption->UseRandomShuffle()) {
        ArrayHelper::RandomShuffle(idMapping->data(), idMapping->size());
    }
}

void SVDTrain::ComputeModel() {
    Log::I("recsys", "SVDTrain::ComputeModel()");
    SVD::ModelComputation *modelComputationDelegate = nullptr;
    if (mTrainOption->Accelerate()) {
        modelComputationDelegate = new SVD::ModelComputationMT();
    } else {
        modelComputationDelegate = new SVD::ModelComputationST();
    }
    modelComputationDelegate->ComputeModel(mTrainOption, mTrainData, mValidateData, mModel);
    delete modelComputationDelegate;
}

void SVDTrain::SaveModel() {
    Log::I("recsys", "SVDTrain::SaveModel()");
    Log::I("recsys", "model file = " + mModelFilepath);
    if (mTrainOption->UseRandomShuffle()) {
        InverseShuffleModel();
    }
    mModel->Save(mModelFilepath);
}

void SVDTrain::InverseShuffleModel() {
    auto getInverseIdMaping = [] (const std::vector<int> &mapping)->std::vector<int> {
        std::vector<int> invMapping(mapping.size());
        for (int i = 0; i < mapping.size(); i++) {
          invMapping[mapping[i]] = i;
        }
        return std::move(invMapping);
    };
    auto inverseShuffleFeatures = [] (std::vector<Vector<float>>& features, const std::vector<int>& mapping) {
        assert(features.size() == mapping.size());
        std::vector<Vector<float>> temp(mapping.size());
        for (int i = 0; i < mapping.size(); ++i) {
            temp[i] = std::move(features[i]);
        }
        for(int i = 0; i < mapping.size(); ++i) {
            features[mapping[i]] = std::move(temp[i]);
        }
    };
    auto inverseShuffleBiases = [] (std::vector<float>& biases, const std::vector<int>& mapping){
        assert(biases.size() == mapping.size());
        std::vector<float> temp(mapping.size());
        std::copy(biases.begin(), biases.end(), temp.begin());
        for (int i = 0; i < mapping.size(); ++i) {
            biases[mapping[i]] = temp[i];
        }
    };
    const std::vector<int> uidInvMapping = getInverseIdMaping(mUserIdMapping);
    const std::vector<int> iidInvMapping = getInverseIdMaping(mItemIdMapping);
    inverseShuffleFeatures(mModel->mUserFeatures, uidInvMapping);
    inverseShuffleFeatures(mModel->mItemFeatures, iidInvMapping);
    if(mParameter->LambdaUserBias() >= 0.0f) {
        inverseShuffleBiases(mModel->mUserBiases, uidInvMapping);
    }
    if(mParameter->LambdaItemBias() >= 0.0f) {
        inverseShuffleBiases(mModel->mItemBiases, iidInvMapping);
    }
}

void SVDTrain::Cleanup() {
    delete mTrainOption;
    delete mParameter;
    delete mModel;
    delete mTrainData;
    delete mValidateData;
}

} //~ namespace longan
