/*
 * svd_predict.cpp
 * Created on: Dec 12, 2014
 * Author: chenguangyu
 */

#include "svd_predict.h"
#include "recsys/base/rating_list.h"
#include "common/logging/logging.h"
#include "common/lang/binary_input_stream.h"
#include <algorithm>
#include <cassert>

namespace longan {

void SVDPredict::Init() {
    Log::I("recsys", "SVDPredict::Init()");
    LoadConfig();
    CreateTrainOption();
    CreateParameter();
    LoadTrainData();
    LoadModel();
}

void SVDPredict::CreateTrainOption() {
    Log::I("recsys", "SVDPredict::CreateTrainOption()");
    mTrainOption = new SVD::TrainOption(mConfig["trainOption"]);
}

void SVDPredict::CreateParameter() {
    Log::I("recsys", "SVDPredict::CreateParameter()");
    mParameter = new SVD::Parameter(mConfig["parameter"]);
}

void SVDPredict::LoadTrainData() {
    Log::I("recsys", "SVDPredict::LoadTrainData()");
    Log::I("recsys", "rating file = " + mRatingTrainFilepath);
    BinaryInputStream bis(mRatingTrainFilepath);
    int numRating, numUser, numItem;
    float avgRating;
    bis >> numRating >> numUser >> numItem
        >> avgRating;
    RatingList rlist(numUser, numItem, numRating);
    for (int i = 0; i < numRating; ++i) {
        int uid, iid;
        float rating;
        bis >> uid >> iid >> rating;
        rlist.Add(RatingRecord(uid, iid, rating));
    }
    mTrainData = new RatingMatUsers();
    mTrainData->Init(rlist);
}

void SVDPredict::LoadModel() {
    Log::I("recsys", "SVDPredict::LoadModel()");
    Log::I("recsys", "model file = " + mModelFilepath);
    mModel = new SVD::ModelPredict(mParameter);
    mModel->Load(mModelFilepath);
}

void SVDPredict::Cleanup() {
    delete mTrainOption;
    delete mParameter;
    delete mTrainData;
    delete mModel;
}

float SVDPredict::PredictRating(int userId, int itemId) const {
    assert(userId >= 0 && userId < mModel->NumUser());
    assert(itemId >= 0 && itemId < mModel->NumItem());
    const Vector32F& userFeature = mModel->UserFeature(userId);
    const Vector32F& itemFeature = mModel->ItemFeature(itemId);
    float predRating = InnerProd(userFeature, itemFeature);
    if (mParameter->LambdaUserBias() >= 0.0f) {
        predRating += mModel->UserBias(userId);
    }
    if (mParameter->LambdaItemBias() >= 0.0f) {
        predRating += mModel->ItemBias(itemId);
    }
    if (mTrainOption->UseRatingAverage()) {
        predRating += mModel->RatingAverage() ;
    }
    return predRating;
}

ItemIdList SVDPredict::PredictTopNItem(int userId, int listSize) const {
    assert(userId >= 0 && userId < mModel->NumUser());
    assert(listSize > 0);
    int numItem = mTrainData->NumItem();
    const UserVec& uv = mTrainData->GetUserVector(userId);
    const ItemRating *data = uv.Data();
    int size = uv.Size();
    std::vector<ItemRating> ratings;
    ratings.reserve(numItem);
    int begin = -1, end = -1;
    for (int i = 0; i < size; ++i) {
        begin = end + 1;
        end = data[i].ItemId();
        for (int iid = begin; iid < end; ++iid) {
            float predRating = PredictRating(userId, iid);
            ratings.push_back(ItemRating(iid, predRating));
        }
    }
    begin = end + 1;
    end = numItem;
    for (int iid = begin; iid < end; ++iid) {
        float predRating = PredictRating(userId, iid);
        ratings.push_back(ItemRating(iid, predRating));
    }
    std::sort(ratings.begin(), ratings.end(),
        [](const ItemRating& lhs, const ItemRating& rhs)->bool {
            return lhs.Rating() > rhs.Rating();
    });
    ItemIdList topNItem(listSize);
    if (listSize <= ratings.size()) {
        for (int i = 0; i < listSize; ++i) {
            topNItem[i] = ratings[i].ItemId();
        }
    } else {
        for (int i = 0; i < ratings.size(); ++i) {
           topNItem[i] = ratings[i].ItemId();
        }
    }
    return std::move(topNItem);
}

} //~ namespace longan
