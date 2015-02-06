/*
 * cf_auto_encoder_predict.h
 * Created on: Feb 4, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_PREDICT_H
#define RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_PREDICT_H

#include "cf_auto_encoder.h"
#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_trait.h"

namespace longan {

class CFAutoEncoderPredict : public BasicPredict {
public:
    using BasicPredict::BasicPredict;
	virtual void Init() override;
	virtual void Cleanup() override;
    virtual float PredictRating(int userId, int itemId) const override;
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const override;
private:
    void LoadRatings();
    void LoadModel();
    void AdjustRating();
private:
    RatingMatrixAsItems<> *mRatingMatrixAsItems = nullptr;
    RatingTrait *mRatingTrait = nullptr;
    CFAutoEncoder *mModel = nullptr;
};

} //~ namespace longan

#endif /* RECSYS_NEURAL_NETWORK_CF_AUTO_ENCODER_PREDICT_H */
