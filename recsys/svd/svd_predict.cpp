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
    LoadRatings();
    LoadModel();
}

void SVDPredict::LoadRatings() {
    Log::I("recsys", "SVDPredict::LoadRatings()");
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
    Log::I("recsys", "create rating matrix");
    mRatingMatrix = new RatingMatrixAsUsers<>();
    mRatingMatrix->Init(rlist);
    mRatingAverage = (SVD::TrainOption(mConfig).UseRatingAverage())? avgRating : 0.0f;
}

void SVDPredict::LoadModel() {
    mParameter = new SVD::Parameter(mConfig);
    mModel = new SVD::ModelPredict(*mParameter);
    mModel->Load(mModelFilepath);
}

void SVDPredict::Cleanup() {
    delete mParameter;
    delete mModel;
}

float SVDPredict::PredictRating(int userId, int itemId) const {
    assert(userId >= 0 && userId < mModel->NumUser());
    assert(itemId >= 0 && itemId < mModel->NumItem());
    const auto& uvec = mModel->UserFeature(userId);
    const auto& ivec = mModel->ItemFeature(itemId);
    float predRating = InnerProd(uvec, ivec);
    if (mParameter->LambdaUserBias() >= 0.0f) {
        predRating += mModel->UserBias(userId);
    }
    if (mParameter->LambdaItemBias() >= 0.0f) {
        predRating += mModel->ItemBias(itemId);
    }
    predRating += mRatingAverage;
    return predRating;
}

ItemIdList SVDPredict::PredictTopNItem(int userId, int listSize) const {
    assert(userId >= 0 && userId < mRatingMatrix->NumUser());
    assert(listSize > 0);
    int numItem = mRatingMatrix->NumItem();
    const auto& uv = mRatingMatrix->GetUserVector(userId);
    const ItemRating *data1 = uv.Data();
    int size1 = uv.Size();
    std::vector<ItemRating> ratings;
    ratings.reserve(numItem);
    int begin = -1, end = -1;
    for (int i = 0; i < size1; ++i) {
        begin = end + 1;
        end = data1[i].ItemId();
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
    ItemIdList resultList(listSize);
    if (listSize <= ratings.size()) {
        for (int i = 0; i < listSize; ++i) {
            resultList[i] = ratings[i].ItemId();
        }
    } else {
        for (int i = 0; i < ratings.size(); ++i) {
           resultList[i] = ratings[i].ItemId();
        }
        for (int i = ratings.size(); i < listSize; ++i) {
           resultList[i] = -1;
        }
    }
    return std::move(resultList);
}

} //~ namespace longan
