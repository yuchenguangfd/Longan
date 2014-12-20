/*
 * item_based_model.cpp
 * Created on: Nov 7, 2014
 * Author: chenguangyu
 */

#include "item_based_model.h"
#include "common/lang/binary_input_stream.h"
#include "common/lang/binary_output_stream.h"
#include <algorithm>
#include <cstdio>
#include <cassert>

namespace longan {

namespace ItemBased {

ModelTrain::ModelTrain(int numItem) :
    mNumItem(numItem) { }

ModelTrain::~ModelTrain() { }

/*
 * Model File (Binary) Format:
 * numItem|
 * numItem_0's_Neighbor|iid|sim|iid|sim|...
 * numItem_1's_neighbor|iid|sim|...
 * ...
 * numItem_n-1's_neighbor|iid|sim|...
 * all number are 32bit-int or 32bit-float
 */
void ModelTrain::Save(const std::string& filename) {
    BinaryOutputStream bos(filename);
    bos << mNumItem;
    for (int itemId = 0; itemId < mNumItem; ++itemId) {
        const NeighborItem* begin = NeighborBegin(itemId);
        int numNeighbor = NeighborSize(itemId);
        bos << numNeighbor;
        for (int i = 0; i < numNeighbor; ++i) {
            int iid = begin[i].ItemId();
            float sim = begin[i].Similarity();
            bos << iid << sim;
        }
    }
}

FixedNeighborSizeModel::FixedNeighborSizeModel(int numItem, int neighborSize) :
    ModelTrain(numItem) {
    assert(numItem > 0);
    assert(neighborSize > 0 && neighborSize < numItem);
    mNeighborItemTable.reserve(numItem);
    for (int i = 0; i < numItem; ++i) {
        mNeighborItemTable.push_back(RunningMaxK<NeighborItem>(neighborSize));
    }
}

void FixedNeighborSizeModel::AddPairSimilarity(int firstItemId, int secondItemId, float similarity) {
    assert(firstItemId >= 0 && firstItemId < mNeighborItemTable.size());
    assert(secondItemId >= 0 && secondItemId < mNeighborItemTable.size());
    assert(similarity >= -1.0f && similarity <= 1.0f);
    mNeighborItemTable[firstItemId].Add(NeighborItem(secondItemId, similarity));
    mNeighborItemTable[secondItemId].Add(NeighborItem(firstItemId, similarity));
}

const NeighborItem* FixedNeighborSizeModel::NeighborBegin(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemTable.size());
    return mNeighborItemTable[itemId].CurrentMaxKBegin();
}

const NeighborItem* FixedNeighborSizeModel::NeighborEnd(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemTable.size());
    return mNeighborItemTable[itemId].CurrentMaxKEnd();
}

int FixedNeighborSizeModel::NeighborSize(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemTable.size());
    return mNeighborItemTable[itemId].CurrentMaxKSize();
}

FixedSimilarityThresholdModel::FixedSimilarityThresholdModel(int numItem, float threshold) :
    ModelTrain(numItem),
    mThreshold(threshold) {
    assert(numItem > 0);
    assert(threshold >= -1.0f && threshold <= 1.0f);
    mNeighborItemTable.resize(numItem);
}

void FixedSimilarityThresholdModel::AddPairSimilarity(int firstItemId, int secondItemId, float similarity) {
    assert(firstItemId >= 0 && firstItemId < mNeighborItemTable.size());
    assert(secondItemId >= 0 && secondItemId < mNeighborItemTable.size());
    assert(similarity >= -1.0f && similarity <= 1.0f);
    if (similarity >= mThreshold) {
        mNeighborItemTable[firstItemId].push_back(NeighborItem(secondItemId, similarity));
        mNeighborItemTable[secondItemId].push_back(NeighborItem(firstItemId, similarity));
    }
}

const NeighborItem* FixedSimilarityThresholdModel::NeighborBegin(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemTable.size());
    return mNeighborItemTable[itemId].data();
}

const NeighborItem* FixedSimilarityThresholdModel::NeighborEnd(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemTable.size());
    return mNeighborItemTable[itemId].data() + mNeighborItemTable[itemId].size();
}

int FixedSimilarityThresholdModel::NeighborSize(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemTable.size());
    return mNeighborItemTable[itemId].size();
}

ModelPredict::ModelPredict() :
    mNumItem(0) { }

ModelPredict::~ModelPredict() { }

const NeighborItem* ModelPredict::NeighborBegin(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemTable.size());
    return mNeighborItemTable[itemId].data();
}

const NeighborItem* ModelPredict::NeighborEnd(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemTable.size());
    return mNeighborItemTable[itemId].data() + mNeighborItemTable[itemId].size();
}

int ModelPredict::NeighborSize(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemTable.size());
    return mNeighborItemTable[itemId].size();
}

const NeighborItem* ModelPredict::ReverseNeighborBegin(int itemId) const {
    assert(itemId >= 0 && itemId < mReverseNeighborItemTable.size());
    return mReverseNeighborItemTable[itemId].data();
}

const NeighborItem* ModelPredict::ReverseNeighborEnd(int itemId) const {
    assert(itemId >= 0 && itemId < mReverseNeighborItemTable.size());
    return mReverseNeighborItemTable[itemId].data()
            + mReverseNeighborItemTable[itemId].size();
}

int ModelPredict::ReverseNeighborSize(int itemId) const {
    assert(itemId >= 0 && itemId < mReverseNeighborItemTable.size());
    return mReverseNeighborItemTable[itemId].size();
}

void ModelPredict::Load(const std::string& filename) {
    BinaryInputStream bis(filename);
    bis >> mNumItem;
    mNeighborItemTable.resize(mNumItem);
    mReverseNeighborItemTable.resize(mNumItem);
    for (int itemId = 0; itemId < mNumItem; ++itemId) {
        int numNeighbor;
        bis >> numNeighbor;
        std::vector<NeighborItem> neighbors;
        neighbors.reserve(numNeighbor);
        for (int i = 0; i < numNeighbor; ++i) {
            int neighborId;
            float sim;
            bis >> neighborId >> sim;
            neighbors.push_back(NeighborItem(neighborId, sim));
            mReverseNeighborItemTable[neighborId].push_back(NeighborItem(itemId, sim));
        }
        std::sort(neighbors.begin(), neighbors.end(),
                [](const NeighborItem& lhs, const NeighborItem& rhs)->bool {
                    return lhs.ItemId() < rhs.ItemId();
        });
        mNeighborItemTable[itemId] = std::move(neighbors);
    }
}

} //~ namespace item_based

} //~ namespace longan
