/*
 * count_inversion_unittest.cpp
 * Created on: Aug 15, 2014
 * Author: chenguangyu
 */

#include "count_inversion.h"
#include "common/util/array_helper.h"
#include <gtest/gtest.h>

using namespace longan;

class CountInversionTest : public ::testing::Test {
protected:
	void SetUp() {
		size = 1997;
		ArrayHelper::CreateArray1D(&array, size);
		ArrayHelper::CreateArray1D(&arrayCopy, size);
	}
	void TearDown() {
		ArrayHelper::ReleaseArray1D(&array, size);
		ArrayHelper::ReleaseArray1D(&arrayCopy, size);
	}
	int *array;
	int *arrayCopy;
	int size;
    CountInversion CntInv;
};

uint64 CountInversionSimple(int *array, int size) {
	int count = 0;
	for (int i = 0; i < size; ++i)
		for (int j = i + 1; j < size; ++j)
			if (array[j] < array[i]) ++count;
	return count;
}


TEST_F(CountInversionTest, IncreaseCase) {
	for (int i = 0; i < size; ++i) {
		array[i] = i;
		arrayCopy[i] = i;
	}
	uint64 result1 = CntInv(array, size);
	uint64 result2 = CountInversionSimple(arrayCopy, size);
	ASSERT_EQ(result2, result1);
}

TEST_F(CountInversionTest, DecreaseCase) {
	for (int i = 0; i < size; ++i) {
		array[i] = size - i;
		arrayCopy[i] = size - i;
	}
	uint64 result1 = CntInv(array, size);
	uint64 result2 = CountInversionSimple(arrayCopy, size);
	ASSERT_EQ(result2, result1);
}

TEST_F(CountInversionTest, RandomCase) {
	ArrayHelper::FillRange(array, size);
	for (int rep = 0; rep < 10; ++rep) {
		ArrayHelper::RandomShuffle(array, size);
		ArrayHelper::CopyArray1D(array, arrayCopy, size);
		uint64 result1 = CntInv(array, size);
		uint64 result2 = CountInversionSimple(arrayCopy, size);
		ASSERT_EQ(result2, result1);
	}
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
