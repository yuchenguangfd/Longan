/*
 * svd_util.cpp
 * Created on: Jan 5, 2015
 * Author: chenguangyu
 */

#include "svd_util.h"
#include "common/lang/binary_input_stream.h"
#include "common/system/system_info.h"
#include "common/math/math.h"
#include <cassert>

namespace longan {

namespace SVD {

Parameter::Parameter(const Json::Value& param) {
    if (param["ratingType"].asString() == "numerical") {
        mRatingType = RatingType_Numerical;
    } else if (param["ratingType"].asString() == "binary") {
        mRatingType = RatingType_Binary;
    } else {
        throw LonganConfigError();
    }
    mDim = param["dim"].asInt();
    assert(mDim > 0);
    mLambdaUserFeature = (float)(param["lambdaUserFeature"].asDouble());
    mLambdaItemFeature = (float)(param["lambdaItemFeature"].asDouble());
    mLambdaUserBias = (float)(param["lambdaUserBias"].asDouble());
    mLambdaItemBias = (float)(param["lambdaItemBias"].asDouble());
    mUseRatingAverage = param["useRatingAverage"].asBool();
    mUseSigmoid = param["useSigmoid"].asBool();
    if (mRatingType == RatingType_Numerical) {
        assert(!mUseSigmoid);
    }
}

TrainOption::TrainOption(const Json::Value& option) {
    mRandomInit = option["randomInit"].asBool();
    mIterations = option["iterations"].asInt();
    assert(mIterations > 0);
    mLearningRate = static_cast<float>(option["learningRate"].asDouble());
    assert(mLearningRate > 0.0f);
    mUseRandomShuffle = option["useRandomShuffle"].asBool();
    mNumUserBlock = option["numUserBlock"].asInt();
    assert(mNumUserBlock > 0);
    mNumItemBlock = option["numItemBlock"].asInt();
    assert(mNumItemBlock > 0);
    mAccelerate = option["accelerate"].asBool();
    if (mAccelerate) {
        mNumThread = option["numThread"].asInt();
        if (mNumThread == 0) {
            mNumThread = SystemInfo::GetNumCPUCore();
        }
        assert(mNumUserBlock > mNumThread);
        assert(mNumItemBlock > mNumThread);
    } else {
        mNumThread = 1;
    }
    mMonitorIteration = option["monitorIteration"].asBool();
    if (mMonitorIteration) {
        mMonitorIterationStep = option["monitorIterationStep"].asInt();
        if (mMonitorIterationStep <= 0) mMonitorIterationStep = 1;
    } else {
        mMonitorIterationStep = 0;
    }
}

PredictOption::PredictOption(const Json::Value& option) {
    if (option["predictRankingMethod"].asString() == "predictRating") {
        mPredictRankingMethod = PredictRankingMethod_PredictRating;
    } else if (option["predictRankingMethod"].asString() == "latentItemNeighbor") {
        mPredictRankingMethod = PredictRankingMethod_LatentItemNeighbor;
        mNeighborSize = option["neighborSize"].asInt();
    } else if (option["predictRankingMethod"].asString() == "latentUserNeighbor") {
        mPredictRankingMethod = PredictRankingMethod_LatentUserNeighbor;
        mNeighborSize = option["neighborSize"].asInt();
    } else {
        throw LonganConfigError();
    }
    if (option["latentDistanceType"].asString() == "normL1") {
        assert(mPredictRankingMethod != PredictRankingMethod_PredictRating);
        mLatentDistanceType = LatentDistanceType_NormL1;
    } else if (option["latentDistanceType"].asString() == "normL2") {
        assert(mPredictRankingMethod != PredictRankingMethod_PredictRating);
        mLatentDistanceType = LatentDistanceType_NormL2;
    } else if (option["latentDistanceType"].asString() == "cosine") {
        assert(mPredictRankingMethod != PredictRankingMethod_PredictRating);
        mLatentDistanceType = LatentDistanceType_Cosine;
    } else if(option["latentDistanceType"].asString() == "correlation") {
        assert(mPredictRankingMethod != PredictRankingMethod_PredictRating);
        mLatentDistanceType = LatentDistanceType_Correlation;
    }
}

Matrix::Matrix() :
    mNumUser(0),
    mNumItem(0),
    mNumRating(0),
    mRatingAverage(0.0f) {}

Matrix::Matrix(Matrix&& orig) noexcept :
    mNumUser(orig.mNumUser),
    mNumItem(orig.mNumItem),
    mNumRating(orig.mNumRating),
    mRatingAverage(orig.mRatingAverage),
    mRatings(std::move(orig.mRatings)) { }

Matrix& Matrix::operator= (Matrix&& rhs) noexcept {
    if (this == &rhs) return *this;
    mNumUser = rhs.mNumUser;
    mNumItem = rhs.mNumItem;
    mNumRating = rhs.mNumRating;
    mRatingAverage = rhs.mRatingAverage;
    mRatings = std::move(rhs.mRatings);
    return *this;
}

Matrix Matrix::LoadFromBinaryFile(const std::string& ratingBinaryFilepath, bool isMetaOnly) {
    Matrix mat;
    BinaryInputStream bis(ratingBinaryFilepath);
    bis >> mat.mNumRating >> mat.mNumUser >> mat.mNumItem
        >> mat.mRatingAverage;
    if (isMetaOnly) {
        return std::move(mat);
    }
    mat.mRatings.reserve(mat.mNumRating);
    for (int i = 0; i < mat.mNumRating; ++i) {
        int uid, iid;
        float rating;
        bis >> uid >> iid >> rating;
        mat.mRatings.push_back(Node(uid, iid, rating));
    }
    return std::move(mat);
}

GriddedMatrix::GriddedMatrix() :
    mNumUser(0),
    mNumItem(0),
    mNumRating(0),
    mRatingAverage(0.0f),
    mNumUserBlock(0),
    mNumItemBlock(0) { }

GriddedMatrix::GriddedMatrix(GriddedMatrix&& orig) noexcept :
    mNumUser(orig.mNumUser),
    mNumItem(orig.mNumItem),
    mNumRating(orig.mNumRating),
    mRatingAverage(orig.mRatingAverage),
    mNumUserBlock(orig.mNumUserBlock),
    mNumItemBlock(orig.mNumItemBlock),
    mGrids(std::move(orig.mGrids)) {
}

GriddedMatrix& GriddedMatrix::operator= (GriddedMatrix&& rhs) noexcept {
    if (this == &rhs) return *this;
    mNumUser = rhs.mNumUser;
    mNumItem = rhs.mNumItem;
    mNumRating = rhs.mNumRating;
    mRatingAverage = rhs.mRatingAverage;
    mNumUserBlock = rhs.mNumUserBlock;
    mNumItemBlock = rhs.mNumItemBlock;
    mGrids = std::move(rhs.mGrids);
    return *this;
}

GriddedMatrix GriddedMatrix::LoadFromBinaryFile(const std::string& ratingBinaryFilepath,
    int numUserBlock, int numItemBlock,
    const std::vector<int>& userIdMapping, const std::vector<int>& itemIdMapping) {
    GriddedMatrix gmat;
    BinaryInputStream bis(ratingBinaryFilepath);
    bis >> gmat.mNumRating >> gmat.mNumUser >> gmat.mNumItem
        >> gmat.mRatingAverage;
    assert(gmat.mNumUser == userIdMapping.size());
    assert(gmat.mNumItem == itemIdMapping.size());
    gmat.mNumUserBlock = numUserBlock;
    gmat.mNumItemBlock = numItemBlock;
    gmat.mGrids.resize(numUserBlock * numItemBlock);
    int rowBlockSize = Math::Ceil(static_cast<double>(gmat.mNumUser) / numUserBlock);
    int colBlockSize = Math::Ceil(static_cast<double>(gmat.mNumItem) / numItemBlock);
    for (int i = 0; i < gmat.mNumRating; ++i) {
        int uid, iid;
        float rating;
        bis >> uid >> iid >> rating;
        uid = userIdMapping[uid];
        iid = itemIdMapping[iid];
        int gridRow = uid / rowBlockSize;
        int gridCol = iid / colBlockSize;
        int gridId = gridRow * numItemBlock + gridCol;
        gmat.mGrids[gridId].Add(Node(uid, iid, rating));
    }
    for(Matrix& grid : gmat.mGrids) {
        grid.Freeze();
    }
    return std::move(gmat);
}

}  // namespace SVD

} //~ namespace longan

