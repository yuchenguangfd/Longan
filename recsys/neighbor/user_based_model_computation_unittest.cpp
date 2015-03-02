/*
 * user_based_model_computation_unittest.cpp
 * Created on: Nov 18, 2014
 * Author: chenguangyu
 */

#include "user_based_model_computation.h"
#include "recsys/util/recsys_test_helper.h"
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
                ModelTrain *model) { }
        float ComputeSimilarityMock(UserVec& uv1, UserVec& uv2) {
            return ComputeSimilarity(uv1, uv2);
        }
    };
    ASSERT_FLOAT_EQ(0.9557790f, ModelComputationMock().ComputeSimilarityMock(uv1, uv2));
}

TEST(ModelComputationTest, ComputeModelSTAndMTResultSame) {
    Json::Value config;
    config["trainOption"]["accelerate"] = true;
    TrainOption option(config["trainOption"]);
    Parameter parameter(config);
    int numUser = 6000;
    int numItem = 50;
    int numRating = 30000;
    RatingMatUsers trainData = RecsysTestHelper::CreateRandomRatingMatUsers(numUser, numItem, numRating);
    ModelTrain model1(&parameter, numUser);
    ModelTrain model2(&parameter, numUser);
    ModelComputationST comp1;
    ModelComputationMT comp2;
    comp1.ComputeModel(&option, &trainData, &model1);
    comp2.ComputeModel(&option, &trainData, &model2);
    for (int uid1 = 0; uid1 < numUser; ++uid1) {
        for (int uid2 = uid1 + 1; uid2 < numUser; ++uid2) {
            ASSERT_FLOAT_EQ(model1.GetSimilarity(uid1, uid2), model2.GetSimilarity(uid1, uid2));
        }
    }
}

TEST(ModelComputationTest, ComputeModelMTAndMTStaticScheduleResultSame) {
    Json::Value config;
    config["trainOption"]["accelerate"] = true;
    TrainOption option(config["trainOption"]);
    Parameter parameter(config);
    int numUser = 6000;
    int numItem = 50;
    int numRating = 30000;
    RatingMatUsers trainData = RecsysTestHelper::CreateRandomRatingMatUsers(numUser, numItem, numRating);
    ModelTrain model1(&parameter, numUser);
    ModelTrain model2(&parameter, numUser);
    ModelComputationMT comp1;
    ModelComputationMTStaticSchedule comp2;
    comp1.ComputeModel(&option, &trainData, &model1);
    comp2.ComputeModel(&option, &trainData, &model2);
    for (int uid1 = 0; uid1 < numUser; ++uid1) {
        for (int uid2 = 0; uid2 < numUser; ++uid2) {
            ASSERT_FLOAT_EQ(model1.GetSimilarity(uid1, uid2), model2.GetSimilarity(uid1, uid2));
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
