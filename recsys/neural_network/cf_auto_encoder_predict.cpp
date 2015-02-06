/*
 * cf_auto_encoder_predict.cpp
 * Created on: Feb 4, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder_predict.h"
#include "recsys/base/rating_adjust.h"
#include "common/logging/logging.h"
#include "common/base/algorithm.h"
#include "common/math/math.h"
#include "common/lang/double.h"
#include "common/error.h"
#include <algorithm>
#include <cassert>

namespace longan {

void CFAutoEncoderPredict::Init() {
    Log::I("recsys", "CFAutoEncoderPredict::Init()");
    LoadConfig();
    LoadRatings();
//    AdjustRating();
    LoadModel();
}

void CFAutoEncoderPredict::Cleanup() {
    Log::I("recsys", "CFAutoEncoderPredict::Cleanup()");
    delete mModel;
    delete mRatingMatrixAsItems;
//    delete mRatingTrait;
}

void CFAutoEncoderPredict::LoadRatings() {
    Log::I("recsys", "CFAutoEncoderPredict::LoadRatings()");
    Log::I("recsys", "rating file = " + mRatingTrainFilepath);
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    Log::I("recsys", "create rating matrix");
    mRatingMatrixAsItems = new RatingMatrixAsItems<>();
    mRatingMatrixAsItems->Init(rlist);
//    Log::I("recsys", "create rating trait");
//    mRatingTrait = new RatingTrait();
//    mRatingTrait->Init(rlist);
}

void CFAutoEncoderPredict::AdjustRating() {
//    Log::I("recsys", "CFAutoEncoderPredict::AdjustRating()");
//    AdjustRatingByMinusUserAverage(*mRatingTrait, mRatingMatrix);
}

void CFAutoEncoderPredict::LoadModel() {
    Log::I("recsys", "CFAutoEncoderPredict::LoadModel()");
    Log::I("recsys", "mdoel file = " + mModelFilepath);
    mModel = new CFAutoEncoder();
    mModel->Load(mModelFilepath);
}

float CFAutoEncoderPredict::PredictRating(int userId, int itemId) const {
	assert(userId >= 0 && userId < mRatingMatrixAsItems->NumUser());
	assert(itemId >= 0 && itemId < mRatingMatrixAsItems->NumItem());
	auto& iv = mRatingMatrixAsItems->GetItemVector(itemId);
	const UserRating *data = iv.Data();
	int size = iv.Size();
	const Vector64F& userCode = mModel->GetCode(userId);
	double norm1 = NormL2(userCode);
	double sum = 0.0, sumWeight = 0.0;
	for (int i = 0; i < size; ++i) {
	    int uid = data[i].UserId();
	    float rating = data[i].Rating();
	    const Vector64F& code = mModel->GetCode(uid);
	    double sim = InnerProd(userCode, code)/(norm1*NormL2(code));
	    if (sim > 0.0) {
	        sum += rating*sim;
	        sumWeight += Math::Abs(sim);
	    }
	}
    int predRating = sum / sumWeight;
    return (float)predRating;
}

ItemIdList CFAutoEncoderPredict::PredictTopNItem(int userId, int listSize) const {
    assert(userId >= 0 && userId < mRatingMatrixAsItems->NumUser());
    assert(listSize > 0);
    Vector64F output = mModel->Reconstruct(userId);
    std::vector<ItemRating> ratings;
    ratings.reserve(mRatingMatrixAsItems->NumItem());
    for (int iid = 0; iid < mRatingMatrixAsItems->NumItem(); ++iid) {
        int predictedRating = Math::MaxIndex(output.Data() + iid*5, 5);
        ratings.push_back(ItemRating(iid, predictedRating));
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
