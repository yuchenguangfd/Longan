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
protected:
    virtual void CreatePredictOption() override;
    virtual void CreateParameter() override;
    virtual void LoadTrainData() override;
    virtual void LoadModel() override;
    virtual float ComputeTopNItemScore(int userId, int itemId) const;
private:
    void InitBinaryCodes();
    float PredictRatingByCodeItemNeighbor(int userId, int itemId) const;
    float PredictRatingByCodeUserNeighbor(int userId, int itemId) const;
    float ComputeTopNItemScoreByCodeItemNeighbor(int userId, int itemId) const;
    float ComputeTopNItemScoreByCodeUserNeighbor(int userId, int itemId) const;
    float ComputeCodeSimilarity(int sampleId1, int sampleId2) const;
    float ComputeCodeDistance(int sampleId1, int sampleId2) const;
private:
    const CFAE::PredictOption *mPredictOption = nullptr;
    const CFAE::Parameter *mParameter = nullptr;
    CFAE::Model *mModel = nullptr;
    RatingMatItems *mTrainDataItems = nullptr;
    std::vector<CFAE::BinaryCode> mBinaryCodes;
};

} //~ namespace longan

#endif /* RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_PREDICT_H */
