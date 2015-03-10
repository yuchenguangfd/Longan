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
    using BasicTrain::BasicTrain;
protected:
    virtual void CreateTrainOption() override;
    virtual void CreateParameter() override;
    virtual void LoadTrainData() override;
    virtual void LoadValidateData() override;
    virtual void InitModel() override;
    virtual void ComputeModel() override;
    virtual void SaveModel() override;
    virtual void Cleanup() override;
private:
    void GenerateIdMapping(int size, std::vector<int> *idMapping);
    void InverseShuffleModel();
private:
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
