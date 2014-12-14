/*
 * svd_predict.cpp
 * Created on: Dec 12, 2014
 * Author: chenguangyu
 */

#include "svd_predict.h"
#include "recsys/base/rating_list_loader.h"
#include "common/logging/logging.h"
#include <algorithm>
#include <cassert>

namespace longan {

void SVDPredict::Init() {
    LoadRatings();
    LoadModel();
}

void SVDPredict::LoadRatings() {
    Log::I("recsys", "SVDPredict::LoadRatings()");
    Log::I("recsys", "rating file = " + mRatingTrainFilepath);
    RatingList rlist = RatingListLoader::Load(mRatingTrainFilepath);
    Log::I("recsys", "create rating matrix");
    mRatingMatrix = new RatingMatrixAsUsers<>();
    mRatingMatrix->Init(rlist);
}

void SVDPredict::LoadModel() {
    mModel = new SVDModelPredict();
    mModel->Load(mModelFilepath);
}

void SVDPredict::Cleanup() {
    delete mModel;
}

float SVDPredict::PredictRating(int userId, int itemId) const {
    assert(userId >= 0 && userId < mModel->mNumUser);
    assert(itemId >= 0 && itemId < mModel->mNumItem);
    int dim = mModel->mParam.dim;
    float predRating = std::inner_product(
        mModel->P + userId * dim,
        mModel->P + userId * dim + dim,
        mModel->Q + itemId * dim,
        0.0);
    predRating += mModel->mAvg;
    if (mModel->mParam.lub >= 0) {
        predRating += mModel->UB[userId];
    }
    if (mModel->mParam.lib >= 0) {
        predRating += mModel->IB[itemId];
    }
    return predRating;
}

ItemIdList SVDPredict::PredictTopNItem(int userId, int listSize) const {
    assert(userId >= 0 && userId < mModel->mNumUser);
    assert(listSize > 0);
    int numItem = mModel->mNumItem;
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
