/*
 * svd_predict.h
 * Created on: Dec 12, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_SVD_SVD_PREDICT_H
#define RECSYS_SVD_SVD_PREDICT_H

#include "svd_model.h"
#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_matrix_as_users.h"

namespace longan {

class SVDPredict : public BasicPredict {
public:
    using BasicPredict::BasicPredict;
    virtual void Init() override;
    virtual void Cleanup() override;
    virtual float PredictRating(int userId, int itemId) const override;
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const override;
private:
    void LoadRatings();
    void LoadModel();
private:
    RatingMatrixAsUsers<> *mRatingMatrix;
    SVDModelPredict *mModel;
};

} //~ namespace longan

#endif /* RECSYS_SVD_SVD_PREDICT_H */
