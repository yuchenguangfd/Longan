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
    enum RatingType {
        RatingType_Numerical,
        RatingType_Binary
    };
    Parameter(const Json::Value& param);
    int RatingType() const { return mRatingType; }
    int Dim() const { return mDim; }
    float LambdaUserFeature() const { return mLambdaUserFeature; }
    float LambdaItemFeature() const { return mLambdaItemFeature; }
    float LambdaUserBias() const { return mLambdaUserBias; }
    float LambdaItemBias() const { return mLambdaItemBias; }
    bool UseRatingAverage() const { return mUseRatingAverage; }
    bool UseSigmoid() const { return mUseSigmoid; }
private:
    int mRatingType;
    int mDim;
    float mLambdaUserFeature;
    float mLambdaItemFeature;
    float mLambdaUserBias;
    float mLambdaItemBias;
    bool mUseRatingAverage;
    bool mUseSigmoid;
};

class TrainOption {
public:
    TrainOption(const Json::Value& option);
    int RandomInit() const { return mRandomInit; }
    int Iterations() const { return mIterations; }
    float LearningRate() const { return mLearningRate; }
    int NumThread() const { return mNumThread; }
    int NumUserBlock() const { return mNumUserBlock; }
    int NumItemBlock() const { return mNumItemBlock; }
    bool UseRandomShuffle() const { return mUseRandomShuffle; }
    bool Accelerate() const { return mAccelerate; }
    bool MonitorIteration() const { return mMonitorIteration; }
    int MonitorIterationStep() const { return mMonitorIterationStep; }
private:
    bool mRandomInit;
    int mIterations;
    float mLearningRate;
    int mNumThread;
    int mNumUserBlock;
    int mNumItemBlock;
    bool mUseRandomShuffle;
    bool mAccelerate;
    bool mMonitorIteration;
    int mMonitorIterationStep;
};

class PredictOption {
public:
   enum PredictRankingMethod {
        PredictRankingMethod_PredictRating,
        PredictRankingMethod_LatentItemNeighbor,
        PredictRankingMethod_LatentUserNeighbor
    };
   enum LatentDistanceType {
       LatentDistanceType_NormL1,
       LatentDistanceType_NormL2,
       LatentDistanceType_Cosine,
       LatentDistanceType_Correlation
   };
    PredictOption(const Json::Value& option);
    int PredictRankingMethod() const { return mPredictRankingMethod; }
    int NeighborSize() const { return mNeighborSize; }
    int LatentDistanceType() const { return mLatentDistanceType; }
private:
    int mPredictRankingMethod;
    int mNeighborSize;
    int mLatentDistanceType;
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
        mRatings.shrink_to_fit();
    }
public:
    static Matrix LoadFromBinaryFile(const std::string& ratingBinaryFilepath, bool isMetaOnly = false);
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
    const Matrix& Get(int gridId) const { return mGrids[gridId]; }
    int NumUser() const { return mNumUser; }
    int NumItem() const { return mNumItem; }
    float RatingAverage() const { return mRatingAverage; }
    int NumUserBlock() const { return mNumUserBlock; }
    int NumItemBlock() const { return mNumItemBlock; }
    int NumBlock() const { return mNumUserBlock * mNumItemBlock; }
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
