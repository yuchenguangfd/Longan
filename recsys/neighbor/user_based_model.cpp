/*
 * user_based_model.cpp
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#include "user_based_model.h"
#include "common/lang/binary_input_stream.h"
#include "common/lang/binary_output_stream.h"
#include <algorithm>
#include <cstdio>
#include <cassert>

namespace longan {

namespace user_based {

ModelTrain::ModelTrain(int numUser) :
    mNumUser(numUser) { }

ModelTrain::~ModelTrain() { }

/*
 * Model File (Binary) Format:
 * numUser|
 * numUser_0's_Neighbor|uid|sim|uid|sim|...
 * numUser_1's_neighbor|uid|sim|...
 * ...
 * numUser_n-1's_neighbor|uid|sim|...
 * all number are 32bit-int or 32bit-float
 */
void ModelTrain::Save(const std::string& filename) {
    BinaryOutputStream bos(filename);
    bos << mNumUser;
    for (int userId = 0; userId < mNumUser; ++userId) {
        const NeighborUser* begin = NeighborBegin(userId);
        int numNeighbor = NeighborSize(userId);
        bos << numNeighbor;
        for (int i = 0; i < numNeighbor; ++i) {
            int uid = begin[i].UserId();
            float sim = begin[i].Similarity();
            bos << uid << sim;
        }
    }
}

FixedNeighborSizeModel::FixedNeighborSizeModel(int numUser, int neighborSize) :
    ModelTrain(numUser) {
    assert(numUser > 0);
    assert(neighborSize > 0 && neighborSize < numUser);
    mNeighborUserList.reserve(numUser);
    for (int i = 0; i < numUser; ++i) {
        mNeighborUserList.push_back(RunningMaxK<NeighborUser>(neighborSize));
    }
}

void FixedNeighborSizeModel::AddPairSimilarity(int firstUserId, int secondUserId, float similarity) {
    assert(firstUserId >= 0 && firstUserId < mNeighborUserList.size());
    assert(secondUserId >= 0 && secondUserId < mNeighborUserList.size());
    mNeighborUserList[firstUserId].Add(NeighborUser(secondUserId, similarity));
    mNeighborUserList[secondUserId].Add(NeighborUser(firstUserId, similarity));
}

const NeighborUser* FixedNeighborSizeModel::NeighborBegin(int userId) const {
    assert(userId >= 0 && userId < mNeighborUserList.size());
    return mNeighborUserList[userId].CurrentMaxKBegin();
}

const NeighborUser* FixedNeighborSizeModel::NeighborEnd(int userId) const {
    assert(userId >= 0 && userId < mNeighborUserList.size());
    return mNeighborUserList[userId].CurrentMaxKEnd();
}

int FixedNeighborSizeModel::NeighborSize(int userId) const {
    assert(userId >= 0 && userId < mNeighborUserList.size());
    return mNeighborUserList[userId].CurrentMaxKSize();
}

FixedSimilarityThresholdModel::FixedSimilarityThresholdModel(int numUser, float threshold) :
    ModelTrain(numUser),
    mThreshold(threshold) {
    assert(numUser > 0);
    assert(threshold >= -1.0f && threshold <= 1.0f);
    mNeighborUserList.resize(numUser);
}

void FixedSimilarityThresholdModel::AddPairSimilarity(int firstUserId, int secondUserId, float similarity) {
    assert(firstUserId >= 0 && firstUserId < mNeighborUserList.size());
    assert(secondUserId >= 0 && secondUserId < mNeighborUserList.size());
    assert(similarity >= -1.0f && similarity <= 1.0f);
    if (similarity >= mThreshold) {
        mNeighborUserList[firstUserId].push_back(NeighborUser(secondUserId, similarity));
        mNeighborUserList[secondUserId].push_back(NeighborUser(firstUserId, similarity));
    }
}

const NeighborUser* FixedSimilarityThresholdModel::NeighborBegin(int userId) const {
    assert(userId >= 0 && userId < mNeighborUserList.size());
    return mNeighborUserList[userId].data();
}

const NeighborUser* FixedSimilarityThresholdModel::NeighborEnd(int userId) const {
    assert(userId >= 0 && userId < mNeighborUserList.size());
    return mNeighborUserList[userId].data() + mNeighborUserList[userId].size();
}

int FixedSimilarityThresholdModel::NeighborSize(int userId) const {
    assert(userId >= 0 && userId < mNeighborUserList.size());
    return mNeighborUserList[userId].size();
}

ModelPredict::ModelPredict() :
    mNumUser(0) { }

ModelPredict::~ModelPredict() { }

const NeighborUser* ModelPredict::NeighborBegin(int userId) const {
    assert(userId >= 0 && userId < mNeighborUserList.size());
    return mNeighborUserList[userId].data();
}

const NeighborUser* ModelPredict::NeighborEnd(int userId) const {
    assert(userId >= 0 && userId < mNeighborUserList.size());
    return mNeighborUserList[userId].data() + mNeighborUserList[userId].size();
}

int ModelPredict::NeighborSize(int userId) const {
    assert(userId >= 0 && userId < mNeighborUserList.size());
    return mNeighborUserList[userId].size();
}

void ModelPredict::Load(const std::string& filename) {
    BinaryInputStream bis(filename);
    bis >> mNumUser;
    mNeighborUserList.resize(mNumUser);
    for (int userId = 0; userId < mNumUser; ++userId) {
        int numNeighbor;
        bis >> numNeighbor;
        std::vector<NeighborUser> neighbors;
        neighbors.reserve(numNeighbor);
        for (int i = 0; i < numNeighbor; ++i) {
            int neighborId;
            float sim;
            bis >> neighborId >> sim;
            neighbors.push_back(NeighborUser(neighborId, sim));
        }
        std::sort(neighbors.begin(), neighbors.end(),
                [](const NeighborUser& lhs, const NeighborUser& rhs)->bool {
                    return lhs.UserId() < rhs.UserId();
        });
        mNeighborUserList[userId] = std::move(neighbors);
    }
}

} //~ namespace user_based

} //~ namespace longan
