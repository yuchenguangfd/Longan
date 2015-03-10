/*
 * basic_predict.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "basic_predict.h"
#include "common/common.h"

namespace longan {

BasicPredict::BasicPredict(
    const std::string& ratingTrainFilepath,
    const std::string& configFilepath,
    const std::string& modelFilepath) :
    mRatingTrainFilepath(ratingTrainFilepath),
    mConfigFilepath(configFilepath),
    mModelFilepath(modelFilepath) { }

void BasicPredict::Init() {
    Log::I("recsys", "BasicPredict::Init()");
    LoadConfig();
    CreatePredictOption();
    CreateParameter();
    LoadTrainData();
    LoadModel();
    InitCachedTopNItems();
}

void BasicPredict::LoadConfig() {
    Log::I("recsys", "BasicPredict::LoadConfig()");
    Log::I("recsys", "config file = " + mConfigFilepath);
    JsonConfigHelper::LoadFromFile(mConfigFilepath, mConfig);
    Log::I("recsys", "config = \n" + mConfig.toStyledString());
}

void BasicPredict::CreatePredictOption() {
    Log::I("recsys", "BasicPredict::CreatePredictOption()");
}

void BasicPredict::CreateParameter() {
    Log::I("recsys", "BasicPredict::CreateParameter()");
}

void BasicPredict::LoadTrainData() {
    Log::I("recsys", "BasicPredict::LoadTrainData()");
    Log::I("recsys", "train rating file = " + mRatingTrainFilepath);
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    mTrainData = new RatingMatUsers();
    mTrainData->Init(rlist);
}

void BasicPredict::LoadModel() {
    Log::I("recsys", "BasicPredict::LoadModel()");
}

void BasicPredict::InitCachedTopNItems() {
    Log::I("recsys", "BasicPredict::InitCachedTopNItems()");
    assert(mTrainData != nullptr);
    mCachedTopNItems.resize(mTrainData->NumUser());
}

float BasicPredict::PredictRating(int userId, int itemId) const {
    return 0.0f;
}

ItemIdList BasicPredict::PredictTopNItem(int userId, int listSize) const {
    assert(mTrainData != nullptr);
    assert(userId >= 0 && userId < mTrainData->NumUser());
    assert(listSize > 0);
    assert(mCachedTopNItems.size() == mTrainData->NumUser());

    if (listSize <= mCachedTopNItems[userId].size()) {
        return PredictTopNItemFromCache(userId, listSize);
    }

    const UserVec& uv = mTrainData->GetUserVector(userId);
    const ItemRating *data = uv.Data();
    int size = uv.Size();
    int numItem = mTrainData->NumItem();
    std::vector<ItemRating> scores;
    scores.reserve(numItem);
    int begin = -1, end = -1;
    for (int i = 0; i < size; ++i) {
        begin = end + 1;
        end = data[i].ItemId();
        for (int iid = begin; iid < end; ++iid) {
            scores.push_back(ItemRating(iid, ComputeTopNItemScore(userId, iid)));
        }
    }
    begin = end + 1;
    end = numItem;
    for (int iid = begin; iid < end; ++iid) {
        scores.push_back(ItemRating(iid, ComputeTopNItemScore(userId, iid)));
    }
    std::sort(scores.begin(), scores.end(),
        [](const ItemRating& lhs, const ItemRating& rhs)->bool {
            return lhs.Rating() > rhs.Rating();
    });
    ItemIdList topNItem(listSize);
    if (listSize <= scores.size()) {
        for (int i = 0; i < listSize; ++i) {
            topNItem[i] = scores[i].ItemId();
        }
    } else {
       for (int i = 0; i < scores.size(); ++i) {
           topNItem[i] = scores[i].ItemId();
       }
   }
   mCachedTopNItems[userId] = topNItem;
   return std::move(topNItem);
}

ItemIdList BasicPredict::PredictTopNItemFromCache(int userId, int listSize) const {
    ItemIdList topNItem(listSize);
    for (int i = 0; i < listSize; ++i) {
        topNItem[i] = mCachedTopNItems[userId][i];
    }
    return std::move(topNItem);
}

float BasicPredict::ComputeTopNItemScore(int userId, int itemId) const {
    return 0.0f;
}

float BasicPredict::ComputeItemSimilarity(int itemId1, int itemId2) const {
    return 0.0f;
}

void BasicPredict::Cleanup() {
    Log::I("recsys", "BasicPredict::Cleanup()");
    delete mTrainData;
}

} //~ namespace longan
