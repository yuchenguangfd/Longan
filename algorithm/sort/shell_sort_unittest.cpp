/*
 * shell_sort_unittest.cpp
 * Created on: Jul 24, 2014
 * Author: chenguangyu
 */

#include "shell_sort.h"
#include "common/util/array_helper.h"
#include <gtest/gtest.h>

using namespace longan;

class ShellSortTest : public ::testing::Test {
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
    ShellSort sort;
};

TEST_F(ShellSortTest, SortAscent) {
    ArrayHelper::FillRange(array, size);
    ArrayHelper::RandomShuffle(array, size);
    sort(array, size);
    for (int i = 1; i < size; ++i) {
        EXPECT_LT(array[i-1], array[i]);
    }
}

TEST_F(ShellSortTest, SortDescent) {
    ArrayHelper::FillRange(array, size);
    ArrayHelper::RandomShuffle(array, size);
    sort(array, size,
            [](int a, int b) -> int {
                return b - a;
            });
    for (int i = 1; i < size; ++i) {
        EXPECT_GT(array[i-1], array[i]);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

