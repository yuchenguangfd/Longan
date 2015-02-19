/*
 * pop_predict.h
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_POP_POP_PREDICT_H
#define RECSYS_POP_POP_PREDICT_H

#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_matrix_as_users.h"
#include <vector>

namespace longan {

class PopPredict : public BasicPredict {
public:
    using BasicPredict::BasicPredict;
    virtual void Init() override;
    virtual void Cleanup() override;
    virtual float PredictRating(int userId, int itemId) const override;
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const override;
private:
    void LoadTrainRating();
    void LoadModel();
    void SortItemsByAverage();
private:
    RatingMatUsers mTrainRating;
    std::vector<float> mItemAverages;
    std::vector<ItemRating> mSortedItemAverages;
};

} //~ namespace longan

#endif /* RECSYS_POP_POP_PREDICT_H */
