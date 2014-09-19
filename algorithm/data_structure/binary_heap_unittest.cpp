/*
 * binary_heap_unittest.h
 * Created on: Aug 20, 2014
 * Author: chenguangyu
 */

#include "binary_heap.h"
#include "common/util/array_helper.h"
#include "common/util/random.h"
#include <gtest/gtest.h>
#include <cstdio>

using namespace longan;

class BinaryHeapTest : public ::testing::Test {
protected:
	void SetUp() {
		mSize = 2014;
		ArrayHelper::AllocateArray1D(&mArray, mSize);
	}
	void TearDown() {
		ArrayHelper::FreeArray1D(&mArray, mSize);
	}
	int* mArray;
	int mSize;
	const static int REPEAT_TIME = 100;
};

TEST_F(BinaryHeapTest, MinBinaryHeap) {
	for (int rep = 0; rep < REPEAT_TIME; ++rep) {
		ArrayHelper::FillRange(mArray, mSize);
		ArrayHelper::RandomShuffle(mArray, mSize);
		MinBinaryHeap<int> heap1;
		for (int i = 0; i < mSize; ++i) {
			heap1.Add(mArray[i]);
		}
		EXPECT_EQ(heap1.Size(), mSize);
		EXPECT_EQ(heap1.Min(), 0);
		EXPECT_EQ(heap1.Top(), 0);
		for (int i = 0; i < mSize; ++i) {
			EXPECT_EQ(i, heap1.Extract());
		}
		EXPECT_EQ(heap1.Size(), 0);
		MinBinaryHeap<double> heap2(mArray, mSize);
		EXPECT_EQ(heap2.Min(), 0);
		for (int i = 0; i < mSize; ++i) {
			EXPECT_EQ(i, heap2.Extract());
			heap2.Add(mSize);
		}
		EXPECT_EQ(heap2.Top(), mSize);
	}
}

TEST_F(BinaryHeapTest, MaxBinaryHeap) {
	for (int rep = 0; rep < REPEAT_TIME; ++rep) {
		ArrayHelper::FillRange(mArray, mSize);
		ArrayHelper::RandomShuffle(mArray, mSize);
		MaxBinaryHeap<int> heap1(mSize);
		for (int i = 0; i < mSize; ++i) {
			heap1.Add(mArray[i]);
		}
		EXPECT_EQ(heap1.Size(), mSize);
		EXPECT_EQ(heap1.Capacity(), mSize);
		EXPECT_EQ(heap1.Max(), mSize-1);
		EXPECT_EQ(heap1.Top(), mSize-1);
		for (int i = mSize - 1; i >= 0; --i) {
			EXPECT_EQ(i, heap1.Extract());
		}
		EXPECT_EQ(heap1.Size(), 0);
		MaxBinaryHeap<float> heap2(mArray, mSize, 2*mSize);
		EXPECT_EQ(heap2.Max(), mSize - 1);
		for (int i = mSize - 1; i >= 0 ; --i) {
			EXPECT_EQ(i, heap2.Extract());
			heap2.Add(0);
		}
		EXPECT_EQ(heap2.Top(), 0);
	}
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
