/*
 * item_based_model_unittest.h
 * Created on: Nov 7, 2014
 * Author: chenguangyu
 */

#include "item_based_model.h"
#include "common/util/array_helper.h"
#include <gtest/gtest.h>

using namespace longan;
using namespace longan::item_based;

TEST (NeighborItemTest, ObjectCompareOK) {
    NeighborItem ni1(42, 0.42);
    NeighborItem ni2(43, 0.43);
    ASSERT_TRUE(ni1 < ni2);
}

TEST(FixedNeighborSizeModelTest, UpdateOK) {
    int numItem = 100;
    int neighborSize = 5;
    Model *model = new FixedNeighborSizeModel(numItem, neighborSize);
    int *array;
    ArrayHelper::CreateArray1D(&array, numItem);
    ArrayHelper::FillRange(array, numItem);
    ArrayHelper::RandomShuffle(array, numItem);
    for (int i = 0; i < numItem; ++i) {
        model->Update(0, array[i], (float)array[i]);
        model->Update(1, array[i], (float)(-array[i]));
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
    Model *model = new FixedSimilarityThresholdModel(numItem, threshold);
    double *array;
    ArrayHelper::CreateArray1D(&array, numItem);
    ArrayHelper::FillRandom(array, numItem);
    for (int i = 0; i < numItem; ++i) {
        model->Update(42, i, (float)array[i]);
    }
    for (const NeighborItem *begin = model->NeighborBegin(42), *end = model->NeighborEnd(42); begin != end; ++begin) {
        ASSERT_TRUE(begin->ItemId() >= 0 && begin->ItemId() < numItem);
        ASSERT_GT(begin->Similarity(), threshold);
    }
    delete model;
    ArrayHelper::ReleaseArray1D(&array, numItem);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
