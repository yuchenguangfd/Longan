/*
 * user_based_predict.h
 * Created on: Nov 19, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_USER_BASED_PREDICT_H
#define RECSYS_NEIGHBOR_USER_BASED_PREDICT_H

#include "user_based_model.h"
#include "recsys/base/rating_matrix_as_items.h"
#include "common/lang/defines.h"
#include <json/json.h>
#include <string>

namespace longan {

typedef std::vector<int> RecommendedItemIdList;

class UserBasedPredict {
public:
    UserBasedPredict(const std::string& trainRatingFilepath, const std::string& configFilepath, const std::string& modelFilepath);
    ~UserBasedPredict();
    void Init();
    void Cleanup();
    float PredictRating(int userId, int itemId);
    RecommendedItemIdList PredictTopNItem(int userId, int listSize);
protected:
    void LoadConfig();
    void LoadRatings();
    void LoadModel();
protected:
    std::string mTrainRatingFilepath;
    std::string mConfigFilepath;
    std::string mModelFilepath;
    Json::Value mConfig;
    RatingMatrixAsItems<> *mRatingMatrix;
    user_based::ModelPredict *mModel;
    DISALLOW_COPY_AND_ASSIGN(UserBasedPredict);
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_PREDICT_H */
