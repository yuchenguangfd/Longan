/*
 * svd_train.h
 * Created on: Dec 13, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_SVD_SVD_TRAIN_H
#define RECSYS_SVD_SVD_TRAIN_H

#include "svd_util.h"
#include "svd_model.h"
#include "svd_model_computation.h"
#include "recsys/base/basic_train.h"
#include <vector>

namespace longan {

class SVDTrain : public BasicTrain {
public:
    SVDTrain(const std::string& ratingTrainFilepath, const std::string& ratingValidateFilepath,
            const std::string& configFilepath, const std::string& modelFilepath);
    virtual void Train() override;
private:
    void CreateTrainOption();
    void CreateParameter();
    void InitModel();
    void LoadRatings();
    void GenerateIdMapping(int size, std::vector<int> *idMapping);
    void ComputeModel();
    void SaveModel();
    void Cleanup();
    void InverseShuffleModel();
private:
    const std::string mRatingValidateFilepath;
    SVD::TrainOption *mTrainOption = nullptr;
    SVD::Parameter *mParameter = nullptr;
    SVD::ModelTrain *mModel = nullptr;
    SVD::GriddedMatrix *mTrainData = nullptr;
    SVD::GriddedMatrix *mValidateData = nullptr;
    std::vector<int> mUserIdMapping;
    std::vector<int> mItemIdMapping;
};

} //~ namespace longan

#endif /* RECSYS_SVD_SVD_TRAIN_H */
