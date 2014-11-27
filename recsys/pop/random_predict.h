/*
 * random_predict.h
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_POP_RANDOM_PREDICT_H
#define RECSYS_POP_RANDOM_PREDICT_H

#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_matrix_as_users.h"

namespace longan {

class RandomPredict : public BasicPredict {
public:
    RandomPredict(const std::string& ratingTrainFilepath, const std::string& configFilepath,
            const std::string& modelFilepath);
    ~RandomPredict();
    virtual void Init() override;
    virtual void Cleanup() override;
    virtual float PredictRating(int userId, int itemId) const override;
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const override;
private:
    RatingMatrixAsUsers<> mRatingMatrix;
    std::vector<float> mUserMinRatings;
    std::vector<float> mUserMaxRatings;
    mutable std::vector<int> mRandomItemIds;
};

} //~ namespace longan

#endif /* RECSYS_POP_RANDOM_PREDICT_H */
