/*
 * item_based_predict.h
 * Created on: Nov 12, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_ITEM_BASED_PREDICT_H
#define RECSYS_NEIGHBOR_ITEM_BASED_PREDICT_H

#include "item_based_model.h"
#include "recsys/base/rating_matrix_as_users.h"
#include "common/lang/defines.h"
#include <json/json.h>
#include <string>

namespace longan {

typedef std::vector<int> RecommendedItemIdList;

class ItemBasedPredict {
public:
	ItemBasedPredict(const std::string& trainRatingFilepath, const std::string& configFilepath, const std::string& modelFilepath);
    ~ItemBasedPredict();
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
    RatingMatrixAsUsers<> *mRatingMatrix;
    item_based::ModelPredict *mModel;
    DISALLOW_COPY_AND_ASSIGN(ItemBasedPredict);
};

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_TRAIN_H */
