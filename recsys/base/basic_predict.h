/*
 * basic_predict.h
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_BASIC_PREDICT_H
#define RECSYS_BASE_BASIC_PREDICT_H

#include "rating_matrix_as_users.h"
#include "common/lang/defines.h"
#include <json/json.h>
#include <vector>
#include <string>

namespace longan {

typedef std::vector<int> ItemIdList;

class BasicPredict {
public:
    BasicPredict(const std::string& ratingTrainFilepath, const std::string& configFilepath,
            const std::string& modelFilepath);
    virtual ~BasicPredict() { }
    virtual void Init();
    virtual void Cleanup();
    virtual float PredictRating(int userId, int itemId) const;
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const;
    virtual float ComputeItemSimilarity(int itemId1, int itemId2) const;
protected:
    virtual void LoadConfig() final;
    virtual void CreatePredictOption();
    virtual void CreateParameter();
    virtual void LoadTrainData();
    virtual void LoadModel();
    virtual float ComputeTopNItemScore(int userId, int itemId) const;
private:
    void InitCachedTopNItems();
    ItemIdList PredictTopNItemFromCache(int userId, int listSize) const;
protected:
    const std::string mRatingTrainFilepath;
    const std::string mConfigFilepath;
    const std::string mModelFilepath;
    Json::Value mConfig;
    RatingMatUsers *mTrainData = nullptr;
    mutable std::vector<ItemIdList> mCachedTopNItems;
    DISALLOW_COPY_AND_ASSIGN(BasicPredict);
};

} //~ namespace longan

#endif /* RECSYS_BASE_BASIC_PREDICT_H */
