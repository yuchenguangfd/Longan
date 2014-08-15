/*
 * randomized_select_unittest.cpp
 * Created on: Aug 10, 2014
 * Author: chenguangyu
 */

#include "randomized_select.h"
#include "common/util/array_helper.h"
#include <gtest/gtest.h>

using namespace longan;

class RandomizedSelectTest : public ::testing::Test {
protected:
    void SetUp() {
        size = 1000000;
        ArrayHelper::AllocateArray1D(&array, size);
        ArrayHelper::FillRange(array, size);
        ArrayHelper::RandomShuffle(array, size);
    }
    void TearDown() {
        ArrayHelper::FreeArray1D(&array, size);
    }
    int *array;
    int size;
    RandomizedSelect select;
};

TEST_F(RandomizedSelectTest, SelectMin) {
    int res = select(array, size, 0);
    EXPECT_EQ(0, res);
}

TEST_F(RandomizedSelectTest, SelectMax) {
    int res = select(array, size, size - 1);
    EXPECT_EQ(size - 1, res);
}

TEST_F(RandomizedSelectTest, SelectKth) {
    for (int i = 0; i < 10; ++i) {
        int k = size / 10 * i;
        int res = select(array, size, k);
        EXPECT_EQ(k, res);
        for (int j = 0; j < k; ++j) {
            EXPECT_LT(array[j], res);
        }
        ArrayHelper::RandomShuffle(array, size);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
