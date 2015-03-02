/*
 * user_based_model_unittest.cpp
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#include "user_based_model.h"
#include <gtest/gtest.h>
#include <cstdio>

using namespace longan;
using namespace longan::UserBased;

TEST(NeighborUserTest, ObjectCompareOk) {
    NeighborUser nu1(42, 0.42f, 0.2f);
    NeighborUser nu2(43, 0.43f, 0.1f);
    ASSERT_TRUE(nu1 < nu2);
}

TEST(ModelTest, SaveAndLoadOK) {
    int numUser = 10;
    Json::Value config;
    Parameter param(config);
    ModelTrain modelTrain(&param, numUser);
    for (int i = 0; i < numUser; ++i) {
        for (int j = i + 1; j < numUser; ++j) {
            modelTrain.PutSimilarity(i, j, i + j);
        }
    }
    modelTrain.Save("model.tmp");
    ModelPredict modelPredict;
    modelPredict.Load("model.tmp");
    for (int i = 0; i < modelPredict.NumUser(); ++i) {
        for (int j = modelPredict.NumUser()-1; j > i; --j) {
            ASSERT_FLOAT_EQ(i + j, modelPredict.GetSimilarity(i, j));
        }
    }
    int rtn = remove("model.tmp");
    ASSERT_TRUE(rtn == 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
