/*
 * user_based_predict.h
 * Created on: Nov 19, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_USER_BASED_PREDICT_H
#define RECSYS_NEIGHBOR_USER_BASED_PREDICT_H

#include "user_based_model.h"
#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_matrix_as_items.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "recsys/base/rating_trait.h"

namespace longan {

class UserBasedPredict : public BasicPredict {
public:
    using BasicPredict::BasicPredict;
    virtual void Init() override;
    virtual void Cleanup() override;
    virtual float PredictRating(int userId, int itemId) const override;
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const override;
protected:
    void LoadRatings();
    void LoadModel();
    void AdjustRating();
protected:
    RatingMatrixAsItems<> *mRatingMatrixAsItems = nullptr;
    RatingMatrixAsUsers<> *mRatingMatrixAsUsers = nullptr;
    RatingTrait *mRatingTrait = nullptr;
    UserBased::ModelPredict *mModel = nullptr;
    enum SIM_TYPE {
        SIM_TYPE_ADJUSTED_COSINE,
        SIM_TYPE_CORRELATION,
        SIM_TYPE_COSINE
    } mSimType;
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_PREDICT_H */
