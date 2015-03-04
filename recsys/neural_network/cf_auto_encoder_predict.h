/*
 * cf_auto_encoder_predict.h
 * Created on: Feb 4, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_PREDICT_H
#define RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_PREDICT_H

#include "cf_auto_encoder_model.h"
#include "recsys/base/basic_predict.h"

namespace longan {

class CFAutoEncoderPredict : public BasicPredict {
public:
    using BasicPredict::BasicPredict;
	virtual void Init() override;
	virtual void Cleanup() override;
    virtual float PredictRating(int userId, int itemId) const override;
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const override;
private:
    void CreatePredictOption();
    void CreateParameter();
    void LoadTrainData();
    void LoadModel();
    void InitCachedTopNItems();
    void InitBinaryCodes();
    ItemIdList PredictTopNItemFromCache(int userId, int listSize) const;
    float PredictTopNItemComputeScore(int userId, int itemId) const;
private:
    const CFAE::PredictOption *mPredictOption = nullptr;
    const CFAE::Parameter *mParameter = nullptr;
    RatingMatUsers *mTrainData = nullptr;
    CFAE::Model *mModel = nullptr;
    mutable std::vector<ItemIdList> mCachedTopNItems;
    std::vector<CFAE::BinaryCode> mBinaryCodes;
};

} //~ namespace longan

#endif /* RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_PREDICT_H */
