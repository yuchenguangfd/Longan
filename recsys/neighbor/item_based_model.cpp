/*
 * item_based_model.cpp
 * Created on: Nov 7, 2014
 * Author: chenguangyu
 */

#include "item_based_model.h"
#include <cassert>

namespace longan {

namespace item_based {

Model::Model() { }

Model::~Model() { }

void Model::Load(const std::string& filename) {
    // TODO
}

void Model::Save(const std::string& filename) {
    // TODO
    // Model File Format:
    // numItem|
    // numItem_1_Neighbor|iid|sim|iid|sim|
    // numItem_2_neighbot|iid|sim....
    FILE* fp = fopen(filename.c_str(), "wb");
//    int numItem = mRatingMatrix.NumItem();
//    fwrite(&numItem, sizeof(numItem), 1, fp);
    fclose(fp);

}

FixedNeighborSizeModel::FixedNeighborSizeModel(int numItem, int neighborSize) :
    Model() {
    assert(numItem > 0);
    assert(neighborSize > 0 && neighborSize < numItem);
    mNeighborItemList.reserve(numItem);
    for (int i = 0; i < numItem; ++i) {
        mNeighborItemList.push_back(RunningMaxK<NeighborItem>(neighborSize));
    }
}

void FixedNeighborSizeModel::Update(int firstItemId, int secondItemId, float similarity) {
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
    Model(),
    mThreshold(threshold) {
    assert(numItem > 0);
    assert(threshold >= -1.0f && threshold <= 1.0f);
    mNeighborItemList.resize(numItem);
}

void FixedSimilarityThresholdModel::Update(int firstItemId, int secondItemId, float similarity) {
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

