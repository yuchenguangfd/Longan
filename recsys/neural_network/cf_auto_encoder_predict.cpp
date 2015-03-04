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
#include "common/util/running_statistic.h"
#include <algorithm>
#include <cassert>

namespace longan {

void CFAutoEncoderPredict::Init() {
    Log::I("recsys", "CFAutoEncoderPredict::Init()");
    LoadConfig();
    CreatePredictOption();
    CreateParameter();
    LoadTrainData();
    LoadModel();
    InitCachedTopNItems();
    if (mParameter->RatingType() == CFAE::Parameter::RatingTypeBinary
            && mPredictOption->CodeDistanceType() == CFAE::PredictOption::CodeDistanceTypeHamming) {
        InitBinaryCodes();
    }
}

void CFAutoEncoderPredict::CreatePredictOption() {
    Log::I("recsys", "CFAutoEncoderPredict::CreatePredictOption()");
    mPredictOption = new CFAE::PredictOption(mConfig["predictOption"]);
}

void CFAutoEncoderPredict::CreateParameter() {
    Log::I("recsys", "CFAutoEncoderPredict::CreateParameter()");
    mParameter = new CFAE::Parameter(mConfig["parameter"]);
}

void CFAutoEncoderPredict::LoadTrainData() {
    Log::I("recsys", "CFAutoEncoderPredict::LoadTrainData()");
    Log::I("recsys", "rating file = " + mRatingTrainFilepath);
    RatingList rlist = RatingList::LoadFromBinaryFile(mRatingTrainFilepath);
    mTrainData = new RatingMatUsers();
    mTrainData->Init(rlist);
}

void CFAutoEncoderPredict::LoadModel() {
    Log::I("recsys", "CFAutoEncoderPredict::LoadModel()");
    if (mParameter->CodeType() == CFAE::Parameter::CodeTypeItem) {
        mModel = new CFAE::Model(mParameter, mTrainData->NumUser(), mTrainData->NumItem());
    } else if (mParameter->CodeType() == CFAE::Parameter::CodeTypeUser) {
        mModel = new CFAE::Model(mParameter, mTrainData->NumItem(), mTrainData->NumUser());
    }
    Log::I("recsys", "mdoel file = " + mModelFilepath);
    mModel->Load(mModelFilepath);
}

void CFAutoEncoderPredict::InitCachedTopNItems() {
    Log::I("recsys", "CFAutoEncoderPredict::InitCachedTopNItems()");
    mCachedTopNItems.resize(mTrainData->NumUser());
}

void CFAutoEncoderPredict::InitBinaryCodes() {
    Log::I("recsys", "CFAutoEncoderPredict::InitBinaryCodes()");
    mBinaryCodes.reserve(mModel->NumSample());
    for (int sampleId = 0; sampleId < mModel->NumSample(); ++sampleId) {
        mBinaryCodes.push_back(CFAE::BinaryCode(mModel->Code(sampleId)));
    }
}

float CFAutoEncoderPredict::PredictRating(int userId, int itemId) const {
	assert(userId >= 0 && userId < mTrainData->NumUser());
	assert(itemId >= 0 && itemId < mTrainData->NumItem());
	assert(mParameter->CodeType() == CFAE::Parameter::CodeTypeItem);
	const Vector64F& itemCode = mModel->Code(itemId);
	const UserVec& uv = mTrainData->GetUserVector(userId);
	double numerator = 0.0;
	double denominator = 0.0;
	for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
	    const Vector64F& item2Code = mModel->Code(ir->ItemId());
	    float sim = 1.0 / (1.0 +NormL2(itemCode-item2Code));
	    numerator += sim * ir->Rating();
	    denominator += Math::Abs(sim);
    }
	double predRating = numerator / (denominator + Double::EPS);
	return (float)predRating;
}

ItemIdList CFAutoEncoderPredict::PredictTopNItem(int userId, int listSize) const {
    assert(userId >= 0 && userId < mTrainData->NumUser());
    assert(listSize > 0);
    if (listSize <= mCachedTopNItems[userId].size()) {
        return PredictTopNItemFromCache(userId, listSize);
    }
    int numItem = mTrainData->NumItem();
    const UserVec& uv = mTrainData->GetUserVector(userId);
    const ItemRating *data = uv.Data();
    int size = uv.Size();
    std::vector<ItemRating> scores;
    scores.reserve(numItem);
    int begin = -1, end = -1;
    for (int i = 0; i < size; ++i) {
        begin = end + 1;
        end = data[i].ItemId();
        for (int iid = begin; iid < end; ++iid) {
            scores.push_back(ItemRating(iid, PredictRating(userId, iid)));
        }
    }
    begin = end + 1;
    end = numItem;
    for (int iid = begin; iid < end; ++iid) {
        scores.push_back(ItemRating(iid, PredictRating(userId, iid)));
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

ItemIdList CFAutoEncoderPredict::PredictTopNItemFromCache(int userId, int listSize) const {
    ItemIdList topNItem(listSize);
    for (int i = 0; i < listSize; ++i) {
        topNItem[i] = mCachedTopNItems[userId][i];
    }
    return std::move(topNItem);
}

float CFAutoEncoderPredict::PredictTopNItemComputeScore(int userId, int itemId) const {
    if (mParameter->RatingType() == CFAE::Parameter::RatingTypeNumerical) {
        return PredictRating(userId, itemId);
    } else if (mParameter->RatingType() == CFAE::Parameter::RatingTypeBinary) {
        if (mPredictOption->CodeDistanceType() == CFAE::PredictOption::CodeDistanceTypeHamming) {
            const UserVec& uv = mTrainData->GetUserVector(userId);
            int minDist = 0x7fffffff;
            for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
                int dist = CFAE::HammingDistance(mBinaryCodes[itemId], mBinaryCodes[ir->ItemId()]);
                minDist = Math::Min(minDist, dist);
            }
            float score = (float)(-minDist);
            return score;
        }
        return 0.0f;
    } else {
        return 0.0f;
    }
}

void CFAutoEncoderPredict::Cleanup() {
    Log::I("recsys", "CFAutoEncoderPredict::Cleanup()");
    delete mModel;
    delete mTrainData;
}

} //~ namespace longan
