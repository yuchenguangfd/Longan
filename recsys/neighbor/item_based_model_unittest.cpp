/*
 * item_based_model_unittest.cpp
 * Created on: Nov 7, 2014
 * Author: chenguangyu
 */

#include "item_based_model.h"
#include <gtest/gtest.h>
#include <cstdio>

using namespace longan;
using namespace longan::ItemBased;

TEST(NeighborItemTest, ObjectCompareOK) {
    NeighborItem ni1(0.42f, 0.2f);
    NeighborItem ni2(0.43f, 0.1f);
    ASSERT_TRUE(ni1 < ni2);
}

TEST(ModelTest, SaveAndLoadOK) {
    int numItem = 10;
    Json::Value config;
    config["ratingType"] = "numerical";
    config["simType"] = "adjustedCosine";
    Parameter param(config);
    ModelTrain modelTrain(&param, numItem);
    for (int i = 0; i < numItem; ++i) {
        for (int j = i + 1; j < numItem; ++j) {
            modelTrain.PutSimilarity(i, j, i+j);
        }
    }
    modelTrain.Save("model.tmp");
    ModelPredict modelPredict;
    modelPredict.Load("model.tmp");
    for (int i = 0; i < modelPredict.NumItem(); ++i) {
        for (int j = modelPredict.NumItem()-1; j > i; --j) {
            ASSERT_FLOAT_EQ(i+j, modelPredict.GetSimilarity(i, j));
        }
    }
    int rtn = remove("model.tmp");
    ASSERT_TRUE(rtn == 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
