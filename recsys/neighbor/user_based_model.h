/*
 * user_based_model.h
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_NEIGHBOR_USER_BASED_MODEL_H
#define RECSYS_NEIGHBOR_USER_BASED_MODEL_H

#include "common/util/running_statistic.h"
#include <vector>
#include <string>

namespace longan {

namespace user_based {

class NeighborUser {
public:
    NeighborUser(int userId, float similarity) : mUserId(userId), mSimilarity(similarity) { }
    int UserId() const {
        return mUserId;
    }
    float Similarity() const {
        return mSimilarity;
    }
private:
    int mUserId;
    float mSimilarity;
};

inline bool operator < (const NeighborUser& lhs, const NeighborUser& rhs) {
    return lhs.Similarity() < rhs.Similarity();
}

class ModelTrain {
public:
    ModelTrain(int numUser);
    virtual ~ModelTrain();
    virtual void AddPairSimilarity(int firstUserId, int secondUserId, float similarity) = 0;
    virtual const NeighborUser* NeighborBegin(int userId) const = 0;
    virtual const NeighborUser* NeighborEnd(int userId) const = 0;
    virtual int NeighborSize(int userId) const = 0;
    void Save(const std::string& filename);
protected:
    int mNumUser;
};

class FixedNeighborSizeModel : public ModelTrain {
public:
    FixedNeighborSizeModel(int numUser, int neighborSize);
    virtual void AddPairSimilarity(int firstUserId, int secondUserId, float similarity) override;
    virtual const NeighborUser* NeighborBegin(int userId) const override;
    virtual const NeighborUser* NeighborEnd(int userId) const override;
    virtual int NeighborSize(int userId) const override;
private:
    std::vector<RunningMaxK<NeighborUser>> mNeighborUserList;
};

class FixedSimilarityThresholdModel : public ModelTrain {
public:
    FixedSimilarityThresholdModel(int numUser, float threshold);
    virtual void AddPairSimilarity(int firstUserId, int secondUserId, float similarity) override;
    virtual const NeighborUser* NeighborBegin(int userId) const override;
    virtual const NeighborUser* NeighborEnd(int userId) const override;
    virtual int NeighborSize(int userId) const override;
private:
    float mThreshold;
    std::vector<std::vector<NeighborUser>> mNeighborUserList;
};

class ModelPredict {
public:
    ModelPredict();
    ~ModelPredict();
    const NeighborUser* NeighborBegin(int userId) const;
    const NeighborUser* NeighborEnd(int userId) const;
    int NeighborSize(int userId) const;
    void Load(const std::string& filename);
private:
    int mNumUser;
    std::vector<std::vector<NeighborUser>> mNeighborUserList;
};

} //~ namespace user_based

} //~ namespace longan

#endif /* RECSYS_NEIGHBOR_USER_BASED_MODEL_H */
