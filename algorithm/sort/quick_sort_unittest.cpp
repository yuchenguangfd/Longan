/*
 * quick_sort_unittest.cpp
 * Created on: Jul 24, 2014
 * Author: chenguangyu
 */

#include "quick_sort.h"
#include "common/util/array_helper.h"
#include "common/time/stopwatch.h"
#include <gtest/gtest.h>

using namespace longan;

class QuickSortTest : public ::testing::Test {
protected:
    void SetUp() {
        size = 1000000;
        ArrayHelper::CreateArray1D(&array, size);
    }
    void TearDown() {
        ArrayHelper::ReleaseArray1D(&array, size);
    }
    int *array;
    int size;
    QuickSort qsort;
};

TEST_F(QuickSortTest, SortAscent) {
    ArrayHelper::FillRange(array, size);
    ArrayHelper::RandomShuffle(array, size);
    qsort(array, size);
    for (int i = 1; i < size; ++i) {
        EXPECT_LT(array[i-1], array[i]);
    }
}

TEST_F(QuickSortTest, SortDescent) {
    ArrayHelper::FillRange(array, size);
    ArrayHelper::RandomShuffle(array, size);
    qsort(array, size,
            [](int a, int b) -> int {
                return b - a;
            });
    for (int i = 1; i < size; ++i) {
        EXPECT_GT(array[i-1], array[i]);
    }
}

TEST_F(QuickSortTest, AlreadySorted) {
    ArrayHelper::FillRange(array, size);
    Stopwatch sw;
    sw.Start();
        qsort(array, size);
    EXPECT_LE(sw.ElapsedMilliseconds(), 1000);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

