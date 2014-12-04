/*
 * user_based_predict.cpp
 * Created on: Nov 22, 2014
 * Author: chenguangyu
 */

#include "user_based_predict.h"
#include "recsys/base/rating_list_loader.h"
#include "common/logging/logging.h"
#include "common/base/algorithm.h"
#include "common/math/math.h"
#include "common/lang/double.h"
#include "common/error.h"
#include <cassert>

namespace longan {

void UserBasedPredict::Init() {
    LoadConfig();
    LoadRatings();
    LoadModel();
}

void UserBasedPredict::Cleanup() {
    delete mModel;
    delete mRatingMatrix;
}

void UserBasedPredict::LoadRatings() {
    Log::I("recsys", "UserBasedPredict::LoadRatings()");
    Log::I("recsys", "rating file = " + mRatingTrainFilepath);
    RatingList rlist = RatingListLoader::Load(mRatingTrainFilepath);
    Log::I("recsys", "create rating matrix");
    mRatingMatrix = new RatingMatrixAsItems<>();
    mRatingMatrix->Init(rlist);
}

void UserBasedPredict::LoadModel() {
    Log::I("recsys", "UserBasedPredict::LoadModel()");
    Log::I("recsys", "model file = " + mModelFilepath);
    mModel = new user_based::ModelPredict();
    mModel->Load(mModelFilepath);
}

float UserBasedPredict::PredictRating(int userId, int itemId) const {
    assert(userId >= 0 && userId < mRatingMatrix->NumUser());
    assert(itemId >= 0 && itemId < mRatingMatrix->NumItem());
    const auto& iv = mRatingMatrix->GetItemVector(itemId);
    const UserRating *data1 = iv.Data();
    int size1 = iv.Size();
    const user_based::NeighborUser* data2 = mModel->NeighborBegin(userId);
    int size2 = mModel->NeighborSize(userId);
    double numerator = 0.0;
    double denominator = 0.0;
    if (size1 < size2) {
        for (int i = 0; i < size1; ++i) {
            const UserRating& ur = data1[i];
            int pos = BSearch(ur.UserId(), data2, size2,
                    [](int lhs, const user_based::NeighborUser& rhs)->int {
                return lhs - rhs.UserId();
            });
            if (pos >= 0) {
                const user_based::NeighborUser& nu = data2[pos];
                numerator += nu.Similarity() * ur.Rating();
                denominator += Math::Abs(nu.Similarity());
            }
        }
    } else {
        for (int i = 0; i < size2; ++i) {
            const user_based::NeighborUser& nu = data2[i];
            int pos = BSearch(nu.UserId(), data1, size1,
                    [](int lhs, const UserRating& rhs)->int {
                return lhs - rhs.UserId();
            });
            if (pos >= 0) {
                const UserRating& ur = data1[pos];
                numerator += nu.Similarity() * ur.Rating();
                denominator += Math::Abs(nu.Similarity());
            }
        }
    }
    double predictedRating = numerator / (denominator + Double::EPS);
    return (float)predictedRating;
}

ItemIdList UserBasedPredict::PredictTopNItem(int userId, int listSize) const {
    return ItemIdList(listSize);
}

} //~ namespace longan
