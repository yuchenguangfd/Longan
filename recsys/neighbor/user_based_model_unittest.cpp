/*
 * user_based_model_unittest.cpp
 * Created on: Nov 17, 2014
 * Author: chenguangyu
 */

#include "user_based_model.h"
#include "common/util/array_helper.h"
#include "common/lang/binary_output_stream.h"
#include <gtest/gtest.h>
#include <cstdio>

using namespace longan;
using namespace longan::UserBased;

TEST(NeighborUserTest, ObjectCompareOk) {
    NeighborUser nu1(42, 0.42);
    NeighborUser nu2(43, 0.43);
    ASSERT_TRUE(nu1 < nu2);
}

TEST(FixedNeighborSizeModelTest, UpdateOK) {
    int numUser = 100;
    int neighborSize = 5;
    ModelTrain *model = new FixedNeighborSizeModel(numUser, neighborSize);
    int *array;
    ArrayHelper::CreateArray1D(&array, numUser);
    ArrayHelper::FillRange(array, numUser);
    ArrayHelper::RandomShuffle(array, numUser);
    for (int i = 0; i < numUser; ++i) {
        model->AddPairSimilarity(0, array[i], (float)array[i]);
        model->AddPairSimilarity(1, array[i], (float)(-array[i]));
    }
    for (const NeighborUser *begin = model->NeighborBegin(0), *end = model->NeighborEnd(0); begin != end; ++begin) {
        ASSERT_TRUE(begin->UserId() >= numUser - neighborSize && begin->UserId() < numUser);
    }
    for (const NeighborUser *begin = model->NeighborBegin(1), *end = model->NeighborEnd(1); begin != end; ++begin) {
        ASSERT_TRUE(begin->UserId() >= 0 && begin->UserId() < neighborSize);
    }
    delete model;
    ArrayHelper::ReleaseArray1D(&array, numUser);
}

TEST(FixedSimilarityThresholdModelTest, UpdateOK) {
    int numUser = 100;
    float threshold = 0.5f;
    ModelTrain *model = new FixedSimilarityThresholdModel(numUser, threshold);
    double *array;
    ArrayHelper::CreateArray1D(&array, numUser);
    ArrayHelper::FillRandom(array, numUser);
    for (int i = 0; i < numUser; ++i) {
        model->AddPairSimilarity(42, i, (float)array[i]);
    }
    for (const NeighborUser *begin = model->NeighborBegin(42), *end = model->NeighborEnd(42); begin != end; ++begin) {
        ASSERT_TRUE(begin->UserId() >= 0 && begin->UserId() < numUser);
        ASSERT_GT(begin->Similarity(), threshold);
    }
    delete model;
    ArrayHelper::ReleaseArray1D(&array, numUser);
}

TEST(ModelPredictTest, NeighborShouldSortedByUserId) {
    BinaryOutputStream bos("model.tmp");
    int numUser = 200;
    bos << numUser;
    for (int i = 0; i < numUser; ++i) {
        int numNeighbor = Random::Instance().Uniform(100, numUser);
        bos << numNeighbor;
        for (int j = 0; j < numNeighbor; ++j) {
            int uid = Random::Instance().Uniform(0, numUser);
            float sim = (float)Random::Instance().NextDouble();
            bos << uid << sim;
        }
    }
    bos.Close();

    ModelPredict model;
    model.Load("model.tmp");
    for (int i = 0; i < numUser; ++i) {
        auto* begin = model.NeighborBegin(i);
        auto* end = model.NeighborEnd(i);
        int size = end - begin;
        for (int j = 1; j < size; ++j) {
            ASSERT_LE(begin[j-1].UserId(), begin[j].UserId());
        }
    }

    int rtn = remove("model.tmp");
    ASSERT_TRUE(rtn == 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
