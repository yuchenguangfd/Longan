/*
 * item_based_model_unittest.h
 * Created on: Nov 7, 2014
 * Author: chenguangyu
 */

#include "item_based_model.h"
#include "common/util/array_helper.h"
#include "common/lang/binary_output_stream.h"
#include <gtest/gtest.h>
#include <cstdio>

using namespace longan;
using namespace longan::ItemBased;

TEST(NeighborItemTest, ObjectCompareOK) {
    NeighborItem ni1(42, 0.42);
    NeighborItem ni2(43, 0.43);
    ASSERT_TRUE(ni1 < ni2);
}

TEST(FixedNeighborSizeModelTest, UpdateOK) {
    int numItem = 100;
    int neighborSize = 5;
    ModelTrain *model = new FixedNeighborSizeModel(numItem, neighborSize);
    int *array;
    ArrayHelper::CreateArray1D(&array, numItem);
    ArrayHelper::FillRange(array, numItem);
    ArrayHelper::RandomShuffle(array, numItem);
    for (int i = 0; i < numItem; ++i) {
        model->AddPairSimilarity(0, array[i], (float)array[i]/numItem);
        model->AddPairSimilarity(1, array[i], (float)(-array[i])/numItem);
    }
    for (const NeighborItem *begin = model->NeighborBegin(0), *end = model->NeighborEnd(0); begin != end; ++begin) {
        ASSERT_TRUE(begin->ItemId() >= numItem - neighborSize && begin->ItemId() < numItem);
    }
    for (const NeighborItem *begin = model->NeighborBegin(1), *end = model->NeighborEnd(1); begin != end; ++begin) {
        ASSERT_TRUE(begin->ItemId() >= 0 && begin->ItemId() < neighborSize);
    }
    delete model;
    ArrayHelper::ReleaseArray1D(&array, numItem);
}

TEST(FixedSimilarityThresholdModelTest, UpdateOK) {
    int numItem = 100;
    float threshold = 0.5f;
    ModelTrain *model = new FixedSimilarityThresholdModel(numItem, threshold);
    double *array;
    ArrayHelper::CreateArray1D(&array, numItem);
    ArrayHelper::FillRandom(array, numItem);
    for (int i = 0; i < numItem; ++i) {
        model->AddPairSimilarity(42, i, (float)array[i]);
    }
    for (const NeighborItem *begin = model->NeighborBegin(42), *end = model->NeighborEnd(42); begin != end; ++begin) {
        ASSERT_TRUE(begin->ItemId() >= 0 && begin->ItemId() < numItem);
        ASSERT_GT(begin->Similarity(), threshold);
    }
    delete model;
    ArrayHelper::ReleaseArray1D(&array, numItem);
}

TEST(ModelPredictTest, NeighborShouldSortedByItemId) {
    BinaryOutputStream bos("model.tmp");
    int numItem = 200;
    bos << numItem;
    for (int i = 0; i < numItem; ++i) {
        int numNeighbor = Random::Instance().Uniform(100, numItem);
        bos << numNeighbor;
        for (int j = 0; j < numNeighbor; ++j) {
            int iid = Random::Instance().Uniform(0, numItem);
            float sim = (float)Random::Instance().NextDouble();
            bos << iid << sim;
        }
    }
    bos.Close();

    ModelPredict model;
    model.Load("model.tmp");
    for (int i = 0; i < numItem; ++i) {
        auto* begin = model.NeighborBegin(i);
        auto* end = model.NeighborEnd(i);
        int size = end - begin;
        for (int j = 1; j < size; ++j) {
            ASSERT_LE(begin[j-1].ItemId(), begin[j].ItemId());
        }
    }

    int rtn = remove("model.tmp");
    ASSERT_TRUE(rtn == 0);
}

TEST(ModelPredictTest, NeighborAndReverseNeighborShouldMatch) {
    BinaryOutputStream bos("model.tmp");
    int numItem = 10;
    bos << numItem;
    for (int i = 0; i < numItem; ++i) {
        int numNeighbor = i;
        bos << numNeighbor;
        for (int j = 0; j < numNeighbor; ++j) {
            int iid = j;
            float sim = (float)Random::Instance().NextDouble();
            bos << iid << sim;
        }
    }
    bos.Close();

    ModelPredict model;
    model.Load("model.tmp");
    for (int i = 0; i < numItem; ++i) {
        ASSERT_EQ(numItem-i-1, model.ReverseNeighborSize(i));
    }

    int rtn = remove("model.tmp");
    ASSERT_TRUE(rtn == 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
