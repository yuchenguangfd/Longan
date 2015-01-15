/*
 * svd_util.h
 * Created on: Jan 5, 2015
 * Author: chenguangyu
 */

#ifndef RECSYS_SVD_SVD_UTIL_H
#define RECSYS_SVD_SVD_UTIL_H

#include "common/lang/defines.h"
#include <json/json.h>
#include <vector>
#include <string>

namespace longan {

namespace SVD {

class Parameter {
public:
    Parameter(const Json::Value& config);
    int Dim() const { return mDim; }
    float LambdaUserFeature() const { return mLambdaUserFeature; }
    float LambdaItemFeature() const { return mLambdaItemFeature; }
    float LambdaUserBias() const { return mLambdaUserBias; }
    float LambdaItemBias() const { return mLambdaItemBias; }
private:
    int mDim;
    float mLambdaUserFeature;
    float mLambdaItemFeature;
    float mLambdaUserBias;
    float mLambdaItemBias;
};

class TrainOption {
public:
    TrainOption(const Json::Value& config);
    int Iterations() const { return mIterations; }
    float LearningRate() const { return mLearningRate; }
    int NumThread() const { return mNumThread; }
    int NumUserBlock() const { return mNumUserBlock; }
    int NumItemBlock() const { return mNumItemBlock; }
    bool UseRandomShuffle() const { return mUseRandomShuffle; }
    bool UseRatingAverage() const { return mUseRatingAverage; }
    bool Accelerate() const { return mAccelerate; }
private:
    int mIterations;
    float mLearningRate;
    int mNumThread;
    int mNumUserBlock;
    int mNumItemBlock;
    bool mUseRandomShuffle;
    bool mUseRatingAverage;
    bool mAccelerate;
};

class Node {
public:
    Node() : mUserId(0), mItemId(0), mRating(0) { }
    Node(int userId, int itemId, float rating) : mUserId(userId), mItemId(itemId), mRating(rating) { }
    int UserId() const { return mUserId; }
    int ItemId() const { return mItemId; }
    float Rating() const { return mRating; }
private:
    int mUserId;
    int mItemId;
    float mRating;
};

class Matrix {
public:
    Matrix();
    Matrix(Matrix&& orig) noexcept;
    Matrix& operator= (Matrix&& rhs) noexcept;
    int NumUser() const { return mNumUser; }
    int NumItem() const { return mNumItem; }
    int NumRating() const { return mNumRating; }
    float RatingAverage() const { return mRatingAverage; }
    const Node& Get(int i) const { return mRatings[i]; }
    void Add(const Node& node) {
        ++mNumRating;
        mRatings.push_back(node);
    }
    void Freeze() {
        mNumRating = mRatings.size();
        mRatings.shrink_to_fit();
    }
public:
    static Matrix LoadMetaOnlyFromBinaryFile(const std::string& ratingBinaryFilepath);
    static Matrix LoadFromBinaryFile(const std::string& ratingBinaryFilepath);
private:
    int mNumUser;
    int mNumItem;
    int mNumRating;
    float mRatingAverage;
    std::vector<Node> mRatings;
    DISALLOW_COPY_AND_ASSIGN(Matrix);
};

class GriddedMatrix {
public:
    GriddedMatrix();
    GriddedMatrix(GriddedMatrix&& orig) noexcept;
    GriddedMatrix& operator= (GriddedMatrix&& rhs) noexcept;
    const Matrix& Grid(int gridId) const {
        return mGrids[gridId];
    }
public:
    static GriddedMatrix LoadFromBinaryFile(const std::string& ratingBinaryFilepath,
        int numUserBlock, int numItemBlock,
        const std::vector<int>& userIdMapping, const std::vector<int>& itemIdMapping);
private:
    int mNumUser;
    int mNumItem;
    int mNumRating;
    float mRatingAverage;
    int mNumUserBlock;
    int mNumItemBlock;
    std::vector<Matrix> mGrids;
    DISALLOW_COPY_AND_ASSIGN(GriddedMatrix);
};

} //~ namespace SVD

} //~ namespace longan

#endif /* RECSYS_SVD_SVD_UTIL_H */
