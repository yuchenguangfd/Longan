/*
 * insertion_sort_unittest.cpp
 * Created on: Jul 23, 2014
 * Author: chenguangyu
 */

#include "insertion_sort.h"
#include "common/util/array_helper.h"
#include <gtest/gtest.h>

using namespace longan;

class InsertionSortTest : public ::testing::Test {
protected:
	void SetUp() {
		size = 5;
		ArrayHelper::AllocateArray1D(&array, size);
		ArrayHelper::FillRange(array, size);
		ArrayHelper::RandomShuffle(array, size);
	}
	void TearDown() {
		ArrayHelper::FreeArray1D(&array, size);
	}
	int *array;
	int size;
    InsertionSort sort;
};

TEST_F(InsertionSortTest, SortAscent) {
    sort(array, size);
    for (int i = 1; i < size; ++i) {
        EXPECT_LT(array[i-1], array[i]);
    }
}

TEST_F(InsertionSortTest, SortDescent) {
    sort(array, size,
    		[](int a, int b) -> int {
    			return (a < b)?1:-1;
    		});
    for (int i = 1; i < size; ++i) {
        EXPECT_GT(array[i-1], array[i]);
    }
}

TEST_F(InsertionSortTest, SortPointerVector) {
    std::vector<int*> pArray(size);
    for (int i = 0; i < size; ++i) {
        pArray[i] = new int(array[i]);
    }
    sort(&pArray[0], pArray.size(), [](int *a, int *b) -> int {
        return (*a) - (*b);
        });
    for (int i = 1; i < size; ++i) {
        EXPECT_LT(*pArray[i-1], *pArray[i]);
    }
    for (int i = 0; i < size; ++i) {
        delete pArray[i];
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
