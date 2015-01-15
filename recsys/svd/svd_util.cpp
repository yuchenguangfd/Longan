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

Parameter::Parameter(const Json::Value& config) {
    const Json::Value& parameter = config["parameter"];
    mDim = parameter["dim"].asInt();
    assert(mDim > 0);
    mLambdaUserFeature = static_cast<float>(parameter["lambdaUserFeature"].asDouble());
    mLambdaItemFeature = static_cast<float>(parameter["lambdaItemFeature"].asDouble());
    mLambdaUserBias = static_cast<float>(parameter["lambdaUserBias"].asDouble());
    mLambdaItemBias = static_cast<float>(parameter["lambdaItemBias"].asDouble());
}

TrainOption::TrainOption(const Json::Value& config) {
    const Json::Value& trainOption = config["trainOption"];
    mIterations = trainOption["iterations"].asInt();
    assert(mIterations > 0);
    mLearningRate = static_cast<float>(trainOption["learningRate"].asDouble());
    assert(mLearningRate > 0.0f);
    mNumThread = trainOption["numThread"].asInt();
    if (mNumThread == 0) {
        mNumThread = SystemInfo::GetNumCPUCore();
    }
    mNumUserBlock = trainOption["numUserBlock"].asInt();
    if (mNumUserBlock == 0) {
        mNumUserBlock = mNumThread * 2;
    }
    assert(mNumUserBlock > mNumThread);
    mNumItemBlock = trainOption["numItemBlock"].asInt();
    if (mNumItemBlock == 0) {
        mNumItemBlock = mNumThread * 2;
    }
    assert(mNumItemBlock > mNumThread);
    mUseRandomShuffle = trainOption["useRandomShuffle"].asBool();
    mUseRatingAverage = trainOption["useRatingAverage"].asBool();
    mAccelerate = trainOption["accelerate"].asBool();
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

Matrix Matrix::LoadMetaOnlyFromBinaryFile(const std::string& ratingBinaryFilepath) {
    Matrix matrix;
    BinaryInputStream bis(ratingBinaryFilepath);
    bis >> matrix.mNumRating >> matrix.mNumUser >> matrix.mNumItem
        >> matrix.mRatingAverage;
    return std::move(matrix);
}

Matrix Matrix::LoadFromBinaryFile(const std::string& ratingBinaryFilepath) {
    Matrix matrix;
    BinaryInputStream bis(ratingBinaryFilepath);
    bis >> matrix.mNumRating >> matrix.mNumUser >> matrix.mNumItem
        >> matrix.mRatingAverage;
    matrix.mRatings.reserve(matrix.mNumRating);
    for (int i = 0; i < matrix.mNumRating; ++i) {
        int uid, iid;
        float rating;
        bis >> uid >> iid >> rating;
        matrix.mRatings.push_back(Node(uid, iid, rating));
    }
    return std::move(matrix);
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
    mNumUserBlock(0),
    mNumItemBlock(0),
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
    GriddedMatrix matrix;
    BinaryInputStream bis(ratingBinaryFilepath);
    bis >> matrix.mNumRating >> matrix.mNumUser >> matrix.mNumItem
        >> matrix.mRatingAverage;
    matrix.mNumUserBlock = numUserBlock;
    matrix.mNumItemBlock = numItemBlock;
    matrix.mGrids.resize(numUserBlock * numItemBlock);
    int rowBlockSize = Math::Ceil(static_cast<double>(matrix.mNumUser) / numUserBlock);
    int colBlockSize = Math::Ceil(static_cast<double>(matrix.mNumItem) / numItemBlock);
    for (int i = 0; i < matrix.mNumRating; ++i) {
        int uid, iid;
        float rating;
        bis >> uid >> iid >> rating;
        uid = userIdMapping[uid];
        iid = itemIdMapping[iid];
        int gridRow = uid / rowBlockSize;
        int gridCol = iid / colBlockSize;
        int gridId = gridRow * numItemBlock + gridCol;
        matrix.mGrids[gridId].Add(Node(uid, iid, rating));
    }
    for(Matrix& grid : matrix.mGrids) {
        grid.Freeze();
    }
    return std::move(matrix);
}

}  // namespace SVD

} //~ namespace longan

