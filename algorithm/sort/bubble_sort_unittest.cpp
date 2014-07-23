/*
 * bubble_sort_unittest.cpp
 * Created on: Jul 23, 2014
 * Author: chenguangyu
 */

#include "bubble_sort.h"
#include "common/util/array_helper.h"
#include <gtest/gtest.h>
#include <vector>

using namespace longan;

class BubbleSortTest : public ::testing::Test {
protected:
	void SetUp() {
		size = 4096;
		ArrayHelper::AllocateArray1D(&array, size);
		ArrayHelper::FillRange(array, size);
		ArrayHelper::RandomShuffle(array, size);
	}
	void TearDown() {
		ArrayHelper::FreeArray1D(&array, size);
	}
	int *array;
	int size;
    BubbleSort sort;
};

TEST_F(BubbleSortTest, SortAscent) {
    sort(&array[0], size);
    for (int i = 1; i < size; ++i) {
        EXPECT_LE(array[i-1], array[i]);
    }
}

TEST_F(BubbleSortTest, SortDescent) {
    sort(&array[0], size,
    		[](int a, int b) -> int {
    			return (a < b)?1:-1;
    		});
    for (int i = 1; i < size; ++i) {
        EXPECT_GE(array[i-1], array[i]);
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
