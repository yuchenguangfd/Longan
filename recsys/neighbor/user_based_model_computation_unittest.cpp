/*
 * user_based_model_computation_unittest.cpp
 * Created on: Nov 18, 2014
 * Author: chenguangyu
 */

#include "user_based_model_computation.h"
#include "recsys/util/recsys_util.h"
#include <gtest/gtest.h>

using namespace longan;
using namespace longan::UserBased;

TEST(ModelComputationTest, ComputeSimilarityOK) {
    RatingRecord data1[] = {RatingRecord(1,2,4.0), RatingRecord(1,3,2.0), RatingRecord(1,4,6.0), RatingRecord(1,5,9.0)};
    RatingRecord data2[] = {RatingRecord(2,1,3.0), RatingRecord(2,3,5.0), RatingRecord(2,4,7.0)};
    UserVec uv1(1, data1, sizeof(data1) / sizeof(data1[0]));
    UserVec uv2(2, data2, sizeof(data2) / sizeof(data2[0]));
    class ModelComputationMock : public ModelComputation {
    public:
        virtual void ComputeModel(const TrainOption *option, RatingMatUsers *trainData,
                Model *model) { }
        float ComputeCosineSimilarityMock(UserVec& uv1, UserVec& uv2) {
            return ComputeCosineSimilarity(uv1, uv2);
        }
        float ComputeBinaryCosineSimilarityMock(UserVec& uv1, UserVec& uv2) {
            return ComputeBinaryCosineSimilarity(uv1, uv2);
        }
        float ComputeBinaryJaccardSimilarityMock(UserVec& uv1, UserVec& uv2) {
            return ComputeBinaryJaccardSimilarity(uv1, uv2);
        }
    };
    ASSERT_FLOAT_EQ(0.9557790f, ModelComputationMock().ComputeCosineSimilarityMock(uv1, uv2));
    ASSERT_FLOAT_EQ(0.5773502f, ModelComputationMock().ComputeBinaryCosineSimilarityMock(uv1, uv2));
    ASSERT_FLOAT_EQ(0.4f, ModelComputationMock().ComputeBinaryJaccardSimilarityMock(uv1, uv2));
}

TEST(ModelComputationTest, ComputeModelSTAndMTResultSame) {
    Json::Value config;
    config["parameter"]["ratingType"] = "numerical";
    config["parameter"]["simType"] = "cosine";
    config["trainOption"]["accelerate"] = true;
    TrainOption option(config["trainOption"]);
    Parameter parameter(config["parameter"]);
    int numUser = 6000;
    int numItem = 50;
    int numRating = 30000;
    RatingMatUsers trainData = RecsysUtil::RandomRatingMatUsers(numUser, numItem, numRating);
    Model model1(&parameter, numUser);
    Model model2(&parameter, numUser);
    ModelComputationST comp1;
    ModelComputationMT comp2;
    comp1.ComputeModel(&option, &trainData, &model1);
    comp2.ComputeModel(&option, &trainData, &model2);
    for (int uid1 = 0; uid1 < numUser; ++uid1) {
        for (int uid2 = uid1 + 1; uid2 < numUser; ++uid2) {
            ASSERT_FLOAT_EQ(model1.Get(uid1, uid2), model2.Get(uid1, uid2));
        }
    }
}

TEST(ModelComputationTest, ComputeModelMTAndMTStaticScheduleResultSame) {
    Json::Value config;
    config["parameter"]["ratingType"] = "binary";
    config["parameter"]["simType"] = "binaryJaccard";
    config["trainOption"]["accelerate"] = true;
    TrainOption option(config["trainOption"]);
    Parameter parameter(config["parameter"]);
    int numUser = 6000;
    int numItem = 50;
    int numRating = 30000;
    RatingMatUsers trainData = RecsysUtil::RandomRatingMatUsers(numUser, numItem, numRating);
    Model model1(&parameter, numUser);
    Model model2(&parameter, numUser);
    ModelComputationMT comp1;
    ModelComputationMTStaticSchedule comp2;
    comp1.ComputeModel(&option, &trainData, &model1);
    comp2.ComputeModel(&option, &trainData, &model2);
    for (int uid1 = 0; uid1 < numUser; ++uid1) {
        for (int uid2 = 0; uid2 < numUser; ++uid2) {
            ASSERT_FLOAT_EQ(model1.Get(uid1, uid2), model2.Get(uid1, uid2));
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
