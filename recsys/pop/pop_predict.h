/*
 * pop_predict.h
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_POP_POP_PREDICT_H
#define RECSYS_POP_POP_PREDICT_H

#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_matrix_as_users.h"
#include <string>
#include <vector>

namespace longan {

class PopPredict : public BasicPredict {
public:
    PopPredict(const std::string& ratingTrainFilepath, const std::string& configFilepath,
            const std::string& modelFilepath);
    virtual void Init() override;
    virtual void Cleanup() override;
    virtual float PredictRating(int userId, int itemId) const override;
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const override;
private:
    void LoadRatings();
    void LoadModel();
    void SortItemAverages();
private:
    RatingMatrixAsUsers<> mRatingMatrix;
    std::vector<float> mItemAverages;
    std::vector<ItemRating> mSortedItemAverages;
};

} //~ namespace longan

#endif /* RECSYS_POP_POP_PREDICT_H */
