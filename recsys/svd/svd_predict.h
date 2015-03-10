/*
 * svd_predict.h
 * Created on: Dec 12, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_SVD_SVD_PREDICT_H
#define RECSYS_SVD_SVD_PREDICT_H

#include "svd_model.h"
#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_matrix_as_items.h"

namespace longan {

class SVDPredict : public BasicPredict {
public:
    using BasicPredict::BasicPredict;
    virtual void Cleanup() override;
    virtual float PredictRating(int userId, int itemId) const override;
protected:
    virtual void CreatePredictOption() override;
    virtual void CreateParameter() override;
    virtual void LoadTrainData() override;
    virtual void LoadModel() override;
    virtual float ComputeTopNItemScore(int userId, int itemId) const override;
private:
    float ComputeTopNItemScoreByLatentItemNeighbor(int userId, int itemId) const;
    float ComputeTopNItemScoreByLatentUserNeighbor(int userId, int itemId) const;
    float ComputeLatentDistance(const Vector32F& vec1, const Vector32F& vec2) const;
protected:
    const SVD::PredictOption *mPredictOption = nullptr;
    const SVD::Parameter *mParameter = nullptr;
    SVD::Model *mModel = nullptr;
    RatingMatItems *mTrainDataItems = nullptr;
};

} //~ namespace longan

#endif /* RECSYS_SVD_SVD_PREDICT_H */
