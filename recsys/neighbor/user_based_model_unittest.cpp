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
    NeighborUser nu1(0.42f, 0.2f);
    NeighborUser nu2(0.43f, 0.1f);
    ASSERT_TRUE(nu1 < nu2);
}

TEST(ModelTest, SaveAndLoadOK) {
    int numUser = 10;
    Json::Value config;
    config["ratingType"] = "numerical";
    config["simType"] = "adjustedCosine";
    Parameter param(config);
    Model model1(&param, numUser);
    for (int i = 0; i < numUser; ++i) {
        for (int j = i + 1; j < numUser; ++j) {
            model1.Put(i, j, i + j);
        }
    }
    model1.Save("model.tmp");
    Model model2(&param, numUser);
    model2.Load("model.tmp");
    for (int i = 0; i < model2.NumUser(); ++i) {
        for (int j = model2.NumUser()-1; j > i; --j) {
            ASSERT_FLOAT_EQ(i + j, model2.Get(i, j));
        }
    }
    int rtn = remove("model.tmp");
    ASSERT_TRUE(rtn == 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
