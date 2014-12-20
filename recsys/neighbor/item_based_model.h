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

namespace ItemBased {

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

class ModelTrain {
public:
    ModelTrain(int numItem);
    virtual ~ModelTrain();
    virtual void AddPairSimilarity(int firstItemId, int secondItemId, float similarity) = 0;
    virtual const NeighborItem* NeighborBegin(int itemId) const = 0;
    virtual const NeighborItem* NeighborEnd(int itemId) const = 0;
    virtual int NeighborSize(int itemId) const = 0;
    void Save(const std::string& filename);
protected:
    int mNumItem;
};

class FixedNeighborSizeModel : public ModelTrain {
public:
    FixedNeighborSizeModel(int numItem, int neighborSize);
    virtual void AddPairSimilarity(int firstItemId, int secondItemId, float similarity) override;
    virtual const NeighborItem* NeighborBegin(int itemId) const override;
    virtual const NeighborItem* NeighborEnd(int itemId) const override;
    virtual int NeighborSize(int itemId) const override;
private:
    std::vector<RunningMaxK<NeighborItem>> mNeighborItemTable;
};

class FixedSimilarityThresholdModel : public ModelTrain {
public:
    FixedSimilarityThresholdModel(int numItem, float threshold);
    virtual void AddPairSimilarity(int firstItemId, int secondItemId, float similarity) override;
    virtual const NeighborItem* NeighborBegin(int itemId) const override;
    virtual const NeighborItem* NeighborEnd(int itemId) const override;
    virtual int NeighborSize(int itemId) const override;
private:
    float mThreshold;
    std::vector<std::vector<NeighborItem>> mNeighborItemTable;
};

class ModelPredict {
public:
    ModelPredict();
    ~ModelPredict();
    const NeighborItem* NeighborBegin(int itemId) const;
    const NeighborItem* NeighborEnd(int itemId) const;
    int NeighborSize(int itemId) const;
    const NeighborItem* ReverseNeighborBegin(int itemId) const;
    const NeighborItem* ReverseNeighborEnd(int itemId) const;
    int ReverseNeighborSize(int itemId) const;
    void Load(const std::string& filename);
private:
    int mNumItem;
    std::vector<std::vector<NeighborItem>> mNeighborItemTable;
    std::vector<std::vector<NeighborItem>> mReverseNeighborItemTable;
};

} //~ namespace ItemBased

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_ITEM_BASED_MODEL_H */
