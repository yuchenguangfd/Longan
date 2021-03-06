/*
 * merge_sort_unittest.cpp
 * Created on: Jul 23, 2014
 * Author: chenguangyu
 */

#include "merge_sort.h"
#include "common/util/array_helper.h"
#include <gtest/gtest.h>

using namespace longan;

class MergeSortTest : public ::testing::Test {
protected:
	void SetUp() {
		size = 65536;
		ArrayHelper::CreateArray1D(&array, size);
		ArrayHelper::FillRandom(array, size);
	}
	void TearDown() {
		ArrayHelper::ReleaseArray1D(&array, size);
	}
	double *array;
	int size;
    MergeSort sort;
};

TEST_F(MergeSortTest, SortAscent) {
    sort(&array[0], size);
    for (int i = 1; i < size; ++i) {
        EXPECT_LE(array[i-1], array[i]);
    }
}

TEST_F(MergeSortTest, SortDescent) {
    sort(&array[0], size,
    		[](double a, double b) -> int {
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
