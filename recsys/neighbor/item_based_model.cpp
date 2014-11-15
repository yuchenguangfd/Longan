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

namespace item_based {

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
        const NeighborItem* end = NeighborEnd(itemId);
        int numNeighbor = end - begin;
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
    mNeighborItemList.reserve(numItem);
    for (int i = 0; i < numItem; ++i) {
        mNeighborItemList.push_back(RunningMaxK<NeighborItem>(neighborSize));
    }
}

void FixedNeighborSizeModel::AddPairSimilarity(int firstItemId, int secondItemId, float similarity) {
    assert(firstItemId >= 0 && firstItemId < mNeighborItemList.size());
    assert(secondItemId >= 0 && secondItemId < mNeighborItemList.size());
    mNeighborItemList[firstItemId].Add(NeighborItem(secondItemId, similarity));
    mNeighborItemList[secondItemId].Add(NeighborItem(firstItemId, similarity));
}

const NeighborItem* FixedNeighborSizeModel::NeighborBegin(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemList.size());
    return mNeighborItemList[itemId].CurrentMaxKBegin();
}

const NeighborItem* FixedNeighborSizeModel::NeighborEnd(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemList.size());
    return mNeighborItemList[itemId].CurrentMaxKEnd();
}

FixedSimilarityThresholdModel::FixedSimilarityThresholdModel(int numItem, float threshold) :
    ModelTrain(numItem),
    mThreshold(threshold) {
    assert(numItem > 0);
    assert(threshold >= -1.0f && threshold <= 1.0f);
    mNeighborItemList.resize(numItem);
}

void FixedSimilarityThresholdModel::AddPairSimilarity(int firstItemId, int secondItemId, float similarity) {
    assert(firstItemId >= 0 && firstItemId < mNeighborItemList.size());
    assert(secondItemId >= 0 && secondItemId < mNeighborItemList.size());
    assert(similarity >= -1.0f && similarity <= 1.0f);
    if (similarity >= mThreshold) {
        mNeighborItemList[firstItemId].push_back(NeighborItem(secondItemId, similarity));
        mNeighborItemList[secondItemId].push_back(NeighborItem(firstItemId, similarity));
    }
}

const NeighborItem* FixedSimilarityThresholdModel::NeighborBegin(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemList.size());
    return mNeighborItemList[itemId].data();
}

const NeighborItem* FixedSimilarityThresholdModel::NeighborEnd(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemList.size());
    return mNeighborItemList[itemId].data() + mNeighborItemList[itemId].size();
}

ModelPredict::ModelPredict() :
    mNumItem(0) { }

ModelPredict::~ModelPredict() { }

const NeighborItem* ModelPredict::NeighborBegin(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemList.size());
    return mNeighborItemList[itemId].data();
}

const NeighborItem* ModelPredict::NeighborEnd(int itemId) const {
    assert(itemId >= 0 && itemId < mNeighborItemList.size());
    return mNeighborItemList[itemId].data() + mNeighborItemList[itemId].size();
}

void ModelPredict::Load(const std::string& filename) {
    BinaryInputStream bis(filename);
    bis >> mNumItem;
    mNeighborItemList.reserve(mNumItem);
    for (int itemId = 0; itemId < mNumItem; ++itemId) {
        int numNeighbor;
        bis >> numNeighbor;
        std::vector<NeighborItem> neighbors;
        neighbors.reserve(numNeighbor);
        for (int i = 0; i < numNeighbor; ++i) {
            int iid;
            float sim;
            bis >> iid >> sim;
            neighbors.push_back(NeighborItem(iid, sim));
        }
        std::sort(neighbors.begin(), neighbors.end(),
                [](const NeighborItem& lhs, const NeighborItem& rhs)->bool {
                    return lhs.ItemId() < rhs.ItemId();
        });
        mNeighborItemList.push_back(std::move(neighbors));
    }
}

} //~ namespace item_based

} //~ namespace longan
