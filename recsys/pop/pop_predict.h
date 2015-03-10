/*
 * pop_predict.h
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#ifndef RECSYS_POP_POP_PREDICT_H
#define RECSYS_POP_POP_PREDICT_H

#include "recsys/base/basic_predict.h"
#include "recsys/base/rating_matrix_as_users.h"
#include <vector>

namespace longan {

namespace Pop {

class PredictOption {
public:
    enum PredictRatingMethod {
        PredictRatingMethod_ItemAverage,
        PredictRatingMethod_UserAverage
    };
    enum PredictRankingMethod {
        PredictRankingMethod_ItemPopularity,
        PredictRankingMethod_ItemAverage
    };
    PredictOption(const Json::Value& option);
    bool PredictRatingMethod() const { return mPredictRatingMethod; }
    bool PredictRankingMethod() const { return mPredictRankingMethod; }
private:
    int mPredictRatingMethod;
    int mPredictRankingMethod;
};

}  // namespace Pop

class PopPredict : public BasicPredict {
public:
    using BasicPredict::BasicPredict;
    virtual void Cleanup() override;
    virtual float PredictRating(int userId, int itemId) const override;
protected:
    virtual void CreatePredictOption() override;
    virtual void LoadModel() override;
    virtual float ComputeTopNItemScore(int userId, int itemId) const override;
private:
    const Pop::PredictOption *mPredictOption = nullptr;
    std::vector<float> mUserAverages;
    std::vector<float> mItemAverages;
    std::vector<float> mItemPopularities;
};

} //~ namespace longan

#endif /* RECSYS_POP_POP_PREDICT_H */
