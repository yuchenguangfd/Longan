/*
 * item_based_model.h
 * Created on: Nov 7, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_ITEM_BASED_MODEL_H
#define RECSYS_NEIGHBOR_ITEM_BASED_MODEL_H

#include "common/util/running_statistic.h"
#include <vector>
#include <string>

namespace longan {

namespace item_based {

class NeighborItem {
public:
    NeighborItem(int itemId, float similarity): mItemId(itemId), mSimilarity(similarity) { }
    int ItemId() const {
        return mItemId;
    }
    float Similarity() const {
        return mSimilarity;
    }
private:
    int mItemId;
    float mSimilarity;
};

inline bool operator < (const NeighborItem& lhs, const NeighborItem& rhs) {
    return lhs.Similarity() < rhs.Similarity();
}

class Model {
public:
    Model();
    virtual ~Model();
    virtual void Update(int firstItemId, int secondItemId, float similarity) = 0;
    virtual const NeighborItem* NeighborBegin(int itemId) const = 0;
    virtual const NeighborItem* NeighborEnd(int itemId) const = 0;
    void Load(const std::string& filename);
    void Save(const std::string& filename);
};

class FixedNeighborSizeModel : public Model {
public:
    FixedNeighborSizeModel(int numItem, int neighborSize);
    virtual void Update(int firstItemId, int secondItemId, float similarity);
    virtual const NeighborItem* NeighborBegin(int itemId) const;
    virtual const NeighborItem* NeighborEnd(int itemId) const;
private:
    std::vector<RunningMaxK<NeighborItem> > mNeighborItemList;
};

class FixedSimilarityThresholdModel : public Model {
public:
    FixedSimilarityThresholdModel(int numItem, float threshold);
    virtual void Update(int firstItemId, int secondItemId, float similarity);
    virtual const NeighborItem* NeighborBegin(int itemId) const;
    virtual const NeighborItem* NeighborEnd(int itemId) const;
private:
    float mThreshold;
    std::vector<std::vector<NeighborItem> > mNeighborItemList;
};

} //~ namespace item_based

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_MODEL_H */
