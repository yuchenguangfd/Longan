/*
 * item_based_model.cpp
 * Created on: Nov 7, 2014
 * Author: chenguangyu
 */

#include "item_based_model.h"
#include <cstdio>
#include <cassert>

namespace longan {

namespace item_based {

Model::Model(int numItem) :
    mNumItem(numItem) { }

Model::~Model() { }

void Model::Load(const std::string& filename) {
    // TODO
}

void Model::Save(const std::string& filename) {
    /*
     * Model File (Binary) Format:
     * numItem|
     * numItem_0's_Neighbor|iid|sim|iid|sim|...
     * numItem_1's_neighbor|iid|sim|...
     * ...
     * numItem_n-1's_neighbor|iid|sim|...
     * all number are 32bit-int or 32bit-float
     */
    int rtn;
    FILE* fp = fopen(filename.c_str(), "wb");
    assert(fp != nullptr);
    rtn = fwrite(&mNumItem, sizeof(mNumItem), 1, fp);
    assert(rtn == 1);
    for (int itemId = 0; itemId < mNumItem; ++itemId) {
        const NeighborItem* begin = NeighborBegin(itemId);
        const NeighborItem* end = NeighborEnd(itemId);
        int numNeighbor = end - begin;
        rtn = fwrite(&numNeighbor, sizeof(numNeighbor), 1, fp);
        assert(rtn == 1);
        for (int i = 0; i < numNeighbor; ++i) {
            int iid = begin[i].ItemId();
            float sim = begin[i].Similarity();
            rtn = fwrite(&iid, sizeof(iid), 1, fp);
            assert(rtn == 1);
            rtn = fwrite(&sim, sizeof(sim), 1, fp);
            assert(rtn == 1);
        }
    }
    rtn = fclose(fp);
    assert(rtn == 0);
}

FixedNeighborSizeModel::FixedNeighborSizeModel(int numItem, int neighborSize) :
    Model(numItem) {
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
    Model(numItem),
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

} //~ namespace item_based

} //~ namespace longan

