/*
 * pop_predict.h
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_POP_POP_PREDICT_H
#define RECSYS_POP_POP_PREDICT_H

#include "recsys/base/item_rating.h"
#include "recsys/base/user_rating.h"
#include <string>
#include <vector>

namespace longan {

typedef std::vector<int> RecommendedItemIdList;
typedef std::vector<int> RecommendedUserIdList;

class PopPredict {
public:
    PopPredict(const std::string& ratingTrainFilePath, const std::string& modelFilePath);
    void Init();
    float PredictRating(int userId, int itemId);
    RecommendedItemIdList PredictItemTopN(int userId, int listSize);
    RecommendedUserIdList PredictUserTopN(int itemId, int listSize);
private:
    void SortItemAverage();
    void SortUserAverage();
private:
    const std::string mRatingTrainFilePath;
    const std::string mModelFilePath;
    bool mModelLoaded;
    int mNumItem;
    int mNumUser;
    std::vector<float> mItemAverages;
    std::vector<float> mUserAverages;
    std::vector<ItemRating> mSortAverageItems;
    std::vector<UserRating> mSortAverageUsers;
};

} //~ namespace longan

#endif /* RECSYS_POP_POP_PREDICT_H */
