/*
 * cf_auto_encoder_predict.cpp
 * Created on: Feb 4, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder_predict.h"
#include "recsys/base/rating_adjust.h"
#include "common/common.h"

namespace longan {

void CFAutoEncoderPredict::Init() {
    BasicPredict::Init();
    if (mPredictOption->CodeDistanceType() == CFAE::PredictOption::CodeDistanceType_Hamming) {
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
    if (mPredictOption->PredictRatingMethod() == CFAE::PredictOption::PredictRatingMethod_CodeUserNeighbor
     || mPredictOption->PredictRankingMethod() == CFAE::PredictOption::PredictRankingMethod_CodeUserNeighbor) {
        mTrainDataItems = new RatingMatItems();
        mTrainDataItems->Init(rlist);
    }
}

void CFAutoEncoderPredict::LoadModel() {
    Log::I("recsys", "CFAutoEncoderPredict::LoadModel()");
    if (mParameter->CodeType() == CFAE::Parameter::CodeType_Item) {
        mModel = new CFAE::Model(mParameter, mTrainData->NumUser(), mTrainData->NumItem());
    } else if (mParameter->CodeType() == CFAE::Parameter::CodeType_User) {
        mModel = new CFAE::Model(mParameter, mTrainData->NumItem(), mTrainData->NumUser());
    }
    Log::I("recsys", "mdoel file = " + mModelFilepath);
    mModel->Load(mModelFilepath);
}

void CFAutoEncoderPredict::InitBinaryCodes() {
    Log::I("recsys", "CFAutoEncoderPredict::InitCachedDistance()");
    mBinaryCodes.reserve(mModel->NumSample());
    for (int sampleId = 0; sampleId < mModel->NumSample(); ++sampleId) {
        mBinaryCodes.push_back(CFAE::BinaryCode(mModel->Code(sampleId)));
    }
}

float CFAutoEncoderPredict::PredictRating(int userId, int itemId) const {
	assert(userId >= 0 && userId < mTrainData->NumUser());
	assert(itemId >= 0 && itemId < mTrainData->NumItem());
	if (mPredictOption->PredictRatingMethod() == CFAE::PredictOption::PredictRatingMethod_Reconstruct) {
	    return mModel->Reconstruct(userId, itemId);
	} else if (mPredictOption->PredictRatingMethod() == CFAE::PredictOption::PredictRatingMethod_CodeItemNeighbor) {
        assert(mParameter->CodeType() == CFAE::Parameter::CodeType_Item);
        return PredictRatingByCodeItemNeighbor(userId, itemId);
	} else if (mPredictOption->PredictRatingMethod() == CFAE::PredictOption::PredictRatingMethod_CodeUserNeighbor) {
        assert(mParameter->CodeType() == CFAE::Parameter::CodeType_User);
        return PredictRatingByCodeUserNeighbor(userId, itemId);
	} else {
	    return 0.0f;
	}
}

float CFAutoEncoderPredict::PredictRatingByCodeItemNeighbor(int userId, int itemId) const {
    if (mPredictOption->NeighborSize() <= 0) {
        const UserVec& uv = mTrainData->GetUserVector(userId);
        double numerator = 0.0;
        double denominator = 0.0;
        for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
            float sim = ComputeCodeSimilarity(ir->ItemId(), itemId);
            numerator += sim * ir->Rating();
            denominator += Math::Abs(sim);
        }
        double predRating = numerator / (denominator + Double::EPS);
        return (float)predRating;
    } else {
        const UserVec& uv = mTrainData->GetUserVector(userId);
        int neighborSize = Math::Min(mPredictOption->NeighborSize(), uv.Size());
        RunningMaxK<std::pair<float, float>> neighbors(neighborSize);
        for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
            float sim = ComputeCodeSimilarity(ir->ItemId(), itemId);
            neighbors.Add(std::make_pair(sim, ir->Rating()));
        }
        double numerator = 0.0;
        double denominator = 0.0;
        for (const std::pair<float, float> *ni = neighbors.CurrentMaxKBegin(), *end = neighbors.CurrentMaxKEnd();
                ni != end; ++ni) {
            numerator += ni->first * ni->second;
            denominator += Math::Abs(ni->first);
        }
        double predRating = numerator / (denominator + Double::EPS);
        return (float)predRating;
    }
}

float CFAutoEncoderPredict::PredictRatingByCodeUserNeighbor(int userId, int itemId) const {
    if (mPredictOption->NeighborSize() <= 0) {
        const ItemVec& iv = mTrainDataItems->GetItemVector(itemId);
        double numerator = 0.0;
        double denominator = 0.0;
        for (const UserRating *ur = iv.Begin(), *end = iv.End(); ur != end; ++ur) {
            float sim = ComputeCodeSimilarity(ur->UserId(), userId);
            numerator += sim * ur->Rating();
            denominator += Math::Abs(sim);
        }
        double predRating = numerator / (denominator + Double::EPS);
        return (float)predRating;
    } else {
        const ItemVec& iv = mTrainDataItems->GetItemVector(itemId);
        int neighborSize = Math::Min(mPredictOption->NeighborSize(), iv.Size());
        RunningMaxK<std::pair<float, float>> neighbors(neighborSize);
        for (const UserRating *ur = iv.Begin(), *end = iv.End(); ur != end; ++ur) {
            float sim = ComputeCodeSimilarity(ur->UserId(), userId);
            neighbors.Add(std::make_pair(sim, ur->Rating()));
        }
        double numerator = 0.0;
        double denominator = 0.0;
        for (const std::pair<float, float> *ni = neighbors.CurrentMaxKBegin(), *end = neighbors.CurrentMaxKEnd();
                ni != end; ++ni) {
            numerator += ni->first * ni->second;
            denominator += Math::Abs(ni->first);
        }
        double predRating = numerator / (denominator + Double::EPS);
        return (float)predRating;
    }
}


float CFAutoEncoderPredict::ComputeTopNItemScore(int userId, int itemId) const {
    if (mPredictOption->PredictRankingMethod() == CFAE::PredictOption::PredictRankingMethod_PredictRating) {
        return PredictRating(userId, itemId);
    } else if (mPredictOption->PredictRankingMethod() == CFAE::PredictOption::PredictRankingMethod_CodeItemNeighbor) {
        return ComputeTopNItemScoreByCodeItemNeighbor(userId, itemId);
    } else if (mPredictOption->PredictRankingMethod() == CFAE::PredictOption::PredictRankingMethod_CodeUserNeighbor) {
        return ComputeTopNItemScoreByCodeUserNeighbor(userId, itemId);
    } else {
        return 0.0f;
    }
}

float CFAutoEncoderPredict::ComputeTopNItemScoreByCodeItemNeighbor(int userId, int itemId) const {
  if (mPredictOption->NeighborSize() <= 0) {
      const UserVec& uv = mTrainData->GetUserVector(userId);
      float score = 0.0f;
      for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
          float dist = ComputeCodeDistance(ir->ItemId(), itemId);
          score += dist;
      }
      score = -score;
      return score;
  } else {
      const UserVec& uv = mTrainData->GetUserVector(userId);
      int neighborSize = Math::Min(mPredictOption->NeighborSize(), uv.Size());
      RunningMinK<float> neighbors(neighborSize);
      for (const ItemRating *ir = uv.Begin(), *end = uv.End(); ir != end; ++ir) {
          float dist = ComputeCodeDistance(ir->ItemId(), itemId);
          neighbors.Add(dist);
      }
      float score = 0.0f;
      for (const float *ni = neighbors.CurrentMinKBegin(), *end = neighbors.CurrentMinKEnd();
              ni != end; ++ni) {
          score += *ni;
      }
      score = -score;
      return score;
  }
}

float CFAutoEncoderPredict::ComputeTopNItemScoreByCodeUserNeighbor(int userId, int itemId) const {
  if (mPredictOption->NeighborSize() <= 0) {
      const ItemVec& iv = mTrainDataItems->GetItemVector(itemId);
      float score = 0.0f;
      for (const UserRating *ur = iv.Begin(), *end = iv.End(); ur != end; ++ur) {
          float dist = ComputeCodeDistance(ur->UserId(), userId);
          score += dist;
      }
      score = -score;
      return score;
  } else {
      const ItemVec& iv = mTrainDataItems->GetItemVector(itemId);
      int neighborSize = Math::Min(mPredictOption->NeighborSize(), iv.Size());
      RunningMinK<float> neighbors(neighborSize);
      for (const UserRating *ur = iv.Begin(), *end = iv.End(); ur != end; ++ur) {
          float dist = ComputeCodeDistance(ur->UserId(), userId);
          neighbors.Add(dist);
      }
      float score = 0.0f;
      for (const float *nu = neighbors.CurrentMinKBegin(), *end = neighbors.CurrentMinKEnd();
              nu != end; ++nu) {
          score += *nu;
      }
      score = -score;
      return score;
  }
}

float CFAutoEncoderPredict::ComputeCodeSimilarity(int sampleId1, int sampleId2) const {
    float dist = ComputeCodeDistance(sampleId1, sampleId2);
    float sim = 1.0f / (1.0f + dist);
    return sim;
}

float CFAutoEncoderPredict::ComputeCodeDistance(int sampleId1, int sampleId2) const {
    const Vector64F& vec1 = mModel->Code(sampleId1);
    const Vector64F& vec2 = mModel->Code(sampleId2);
    switch (mPredictOption->CodeDistanceType()) {
        case CFAE::PredictOption::CodeDistanceType_NormL1:
            return DistanceL1(vec1, vec2);
        case CFAE::PredictOption::CodeDistanceType_NormL2:
            return DistanceL2(vec1, vec2);
        case CFAE::PredictOption::CodeDistanceType_Cosine:
            return DistanceCosine(vec1, vec2);
        case CFAE::PredictOption::CodeDistanceType_Correlation:
            return DistanceCorrelation(vec1, vec2);
        case CFAE::PredictOption::CodeDistanceType_Hamming:
            return CFAE::HammingDistance(mBinaryCodes[sampleId1], mBinaryCodes[sampleId2]);
        default:
          return 0.0f;
    }
}

void CFAutoEncoderPredict::Cleanup() {
    Log::I("recsys", "CFAutoEncoderPredict::Cleanup()");
    delete mPredictOption;
    delete mParameter;
    delete mModel;
    delete mTrainData;
    delete mTrainDataItems;
}

} //~ namespace longan
