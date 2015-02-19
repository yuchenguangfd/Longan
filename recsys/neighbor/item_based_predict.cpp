/*
 * item_based_predict.cpp
 * Created on: Nov 12, 2014
 * Author: chenguangyu
 */

#include "item_based_predict.h"
#include "recsys/base/rating_adjust.h"
#include "common/logging/logging.h"
#include "common/base/algorithm.h"
#include "common/math/math.h"
#include "common/lang/double.h"
#include "common/error.h"
#include <algorithm>
#include <cassert>

namespace longan {

void ItemBasedPredict::Init() {
    Log::I("recsys", "ItemBasedPredict::Init()");
    LoadConfig();
    LoadRatings();
    AdjustRating();
    LoadModel();
}

void ItemBasedPredict::Cleanup() {
    Log::I("recsys", "ItemBasedPredict::Cleanup()");
    delete mModel;
    delete mRatingMatrix;
    delete mRatingTrait;
}

void ItemBasedPredict::LoadRatings() {
    Log::I("recsys", "ItemBasedPredict::LoadRatings()");
    Log::I("recsys", "rating file = " + mRatingTrainFilepath);
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    Log::I("recsys", "create rating matrix");
    mRatingMatrix = new RatingMatrixAsUsers<>();
    mRatingMatrix->Init(rlist);
    Log::I("recsys", "create rating trait");
    mRatingTrait = new RatingTrait();
    mRatingTrait->Init(rlist);
}

void ItemBasedPredict::AdjustRating() {
    Log::I("recsys", "ItemBasedPredict::AdjustRating()");
    if (mConfig["parameter"]["similarityType"].asString() == "adjustedCosine") {
        mSimTpye = SIM_TYPE_ADJUSTED_COSINE;
        AdjustRatingByMinusUserAverage(*mRatingTrait, mRatingMatrix);
    } else if (mConfig["parameter"]["similarityType"].asString() == "correlation") {
        mSimTpye = SIM_TYPE_CORRELATION;
        AdjustRatingByMinusItemAverage(*mRatingTrait, mRatingMatrix);
    } else { // (mConfig["parameter"]["similarityType"].asString() == "cosine")
        mSimTpye = SIM_TYPE_COSINE;
    }
}

void ItemBasedPredict::LoadModel() {
    Log::I("recsys", "ItemBasedPredict::LoadModel()");
    Log::I("recsys", "mdoel file = " + mModelFilepath);
    mModel = new ItemBased::ModelPredict();
    mModel->Load(mModelFilepath);
}

float ItemBasedPredict::PredictRating(int userId, int itemId) const {
	assert(userId >= 0 && userId < mRatingMatrix->NumUser());
	assert(itemId >= 0 && itemId < mRatingMatrix->NumItem());
    const auto& uv = mRatingMatrix->GetUserVector(userId);
    const ItemRating *data1 = uv.Data();
    int size1 = uv.Size();
    const ItemBased::NeighborItem* data2 = mModel->NeighborBegin(itemId);
    int size2 = mModel->NeighborSize(itemId);
    double numerator = 0.0;
    double denominator = 0.0;
    if (size1 < size2) {
        for (int i = 0; i < size1; ++i) {
            const ItemRating& ir = data1[i];
            int pos = BSearch(ir.ItemId(), data2, size2,
                    [](int lhs, const ItemBased::NeighborItem& rhs)->int {
                return lhs - rhs.ItemId();
            });
            if (pos >= 0) {
                const ItemBased::NeighborItem& ni = data2[pos];
                numerator += ni.Similarity() * ir.Rating();
                denominator += Math::Abs(ni.Similarity());
            }
        }
    } else {
        for (int i = 0; i < size2; ++i) {
            const ItemBased::NeighborItem& ni = data2[i];
            int pos = BSearch(ni.ItemId(), data1, size1,
                    [](int lhs, const ItemRating& rhs)->int {
                return lhs - rhs.ItemId();
            });
            if (pos >= 0) {
                const ItemRating& ir = data1[pos];
                numerator += ni.Similarity() * ir.Rating();
                denominator += Math::Abs(ni.Similarity());
            }
        }
    }
    double predictedRating = numerator / (denominator + Double::EPS);
    if (mSimTpye == SIM_TYPE_ADJUSTED_COSINE) {
        predictedRating += mRatingTrait->UserAverage(userId);
    } else if (mSimTpye == SIM_TYPE_CORRELATION) {
        predictedRating += mRatingTrait->ItemAverage(itemId);
    }
    return (float)predictedRating;
}

ItemIdList ItemBasedPredict::PredictTopNItem(int userId, int listSize) const {
    assert(userId >= 0 && userId < mRatingMatrix->NumUser());
    assert(listSize > 0);
    int numItem = mRatingMatrix->NumItem();
    std::vector<double> numerators(numItem);
    std::vector<double> denominator(numItem);
    const auto& uv = mRatingMatrix->GetUserVector(userId);
    const ItemRating *data1 = uv.Data();
    int size1 = uv.Size();
    for (int i = 0; i < size1; ++i) {
        int iid = data1[i].ItemId();
        float rating = data1[i].Rating();
        const ItemBased::NeighborItem *data2 = mModel->ReverseNeighborBegin(iid);
        int size2 = mModel->ReverseNeighborSize(iid);
        for (int j = 0; j < size2; ++j) {
            int idx = data2[j].ItemId();
            numerators[idx] += data2[j].Similarity() * rating;
            denominator[idx] += Math::Abs(data2[j].Similarity());
        }
    }
    std::vector<ItemRating> ratings;
    ratings.reserve(numItem);
    int begin = -1, end = -1;
    for (int i = 0; i < size1; ++i) {
        begin = end + 1;
        end = data1[i].ItemId();
        for (int iid = begin; iid < end; ++iid) {
            float predRating = numerators[iid] / (denominator[iid] + Double::EPS);
            ratings.push_back(ItemRating(iid, predRating));
        }
    }
    begin = end + 1;
    end = numItem;
    for (int iid = begin; iid < end; ++iid) {
        float predRating = numerators[iid] / (denominator[iid] + Double::EPS);
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

float ItemBasedPredict::ComputeItemSimilarity(int itemId1, int itemId2) const {
    // TODO
    return 0.5;
}
} //~ namespace longan
