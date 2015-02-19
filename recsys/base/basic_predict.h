/*
 * basic_predict.h
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_BASE_BASIC_PREDICT_H
#define RECSYS_BASE_BASIC_PREDICT_H

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
    virtual ~BasicPredict();
    virtual void Init() = 0;
    virtual void Cleanup() = 0;
    virtual float PredictRating(int userId, int itemId) const = 0;
    virtual ItemIdList PredictTopNItem(int userId, int listSize) const = 0;
    // return item-item similarity in range [0, 1]
    virtual float ComputeItemSimilarity(int itemId1, int itemId2) const;
protected:
    virtual void LoadConfig();
protected:
    const std::string mRatingTrainFilepath;
    const std::string mConfigFilepath;
    const std::string mModelFilepath;
    Json::Value mConfig;
    DISALLOW_COPY_AND_ASSIGN(BasicPredict);
};

} //~ namespace longan

#endif /* RECSYS_BASE_BASIC_PREDICT_H */
