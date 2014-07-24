/*
 * counting_sort_unittest.cpp
 * Created on: Jul 24, 2014
 * Author: chenguangyu
 */

#include "counting_sort.h"
#include "common/util/array_helper.h"
#include <gtest/gtest.h>
#include <utility>

using namespace longan;

class CountingSortTest : public ::testing::Test {
protected:
    void SetUp() {
        size = 600000;
        ArrayHelper::AllocateArray1D(&array, size);
        ArrayHelper::AllocateArray1D(&pairArray, size);
    }
    void TearDown() {
        ArrayHelper::FreeArray1D(&array, size);
    }
    int *array;
    std::pair<double, int> *pairArray;
    int size;
    CountingSort sort;
};

TEST_F(CountingSortTest, SortInt) {
    ArrayHelper::FillRange(array, size);
    ArrayHelper::RandomShuffle(array, size);
    sort(array, size, size, [](int val)->int { return val; });
    for (int i = 1; i < size; ++i) {
        EXPECT_LE(array[i-1], array[i]);
    }
}

TEST_F(CountingSortTest, SortPair) {
    for (int i = 0; i < size; ++i) {
        pairArray[i].first = Random::Instance().NextDouble();
        pairArray[i].second = i;
    }
    ArrayHelper::RandomShuffle(array, size);
    sort(pairArray, size, size, [](std::pair<double, int>& p)->int { return p.second;});
    for (int i = 1; i < size; ++i) {
        EXPECT_LE(pairArray[i-1].second, pairArray[i].second);
    }
}

TEST_F(CountingSortTest, SortStable) {
    for (int i = 0; i < size; ++i) {
       pairArray[i].first = i / size;
       pairArray[i].second = i;
    }
    sort(pairArray, size, size, [](std::pair<double, int>& p)->int { return p.first > 0.5 ? 1 : 0;});
    for (int i = 0; i < size; ++i) {
        EXPECT_LE(pairArray[i].second, i);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
