/*
 * quick_sort_mt_unittest.cpp
 * Created on: Jul 24, 2014
 * Author: chenguangyu
 */

#include "quick_sort_mt.h"
#include "common/util/array_helper.h"
#include "common/time/stopwatch.h"
#include <gtest/gtest.h>

using namespace longan;

class QuickSortMTTest : public ::testing::Test {
protected:
    void SetUp() {
        size = 5000000;
        ArrayHelper::CreateArray1D(&array, size);
    }
    void TearDown() {
        ArrayHelper::ReleaseArray1D(&array, size);
    }
    int *array;
    int size;
    QuickSortMT qsort;
};

TEST_F(QuickSortMTTest, SortAscent) {
    ArrayHelper::FillRange(array, size);
    ArrayHelper::RandomShuffle(array, size);
    qsort(array, size);
    for (int i = 1; i < size; ++i) {
        EXPECT_LT(array[i-1], array[i]);
    }
}

TEST_F(QuickSortMTTest, SortDescent) {
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

TEST_F(QuickSortMTTest, SpeedUp) {
    ArrayHelper::FillRange(array, size);
    ArrayHelper::RandomShuffle(array, size);
    QuickSort sort1;
    QuickSortMT sort2;
    Stopwatch sw;
    sw.Start();
        sort1(array, size);
    double t1 = sw.ElapsedMilliseconds();
    ArrayHelper::RandomShuffle(array, size);
    sw.Reset();
    sw.Start();
        sort2(array, size);
    double t2 = sw.ElapsedMilliseconds();
    EXPECT_LT(t2, t1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

