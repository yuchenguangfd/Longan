/*
 * queue_unittest.cpp
 * Created on: Aug 20, 2014
 * Author: chenguangyu
 */

#include "queue.h"
#include <gtest/gtest.h>

using namespace longan;

class QueueTest : public ::testing::Test {
protected:
	virtual void SetUp() {
		q1.Enqueue(1);
		q2.Enqueue(2);
		q2.Enqueue(3);
	}
	Queue<int> q0;
	Queue<int> q1;
	Queue<int> q2;
};

TEST_F(QueueTest, Empty) {
  EXPECT_TRUE(q0.Empty());
  q0.Enqueue(123);
  q0.Enqueue(456);
  EXPECT_FALSE(q0.Empty());
}

TEST_F(QueueTest, DequeueWorks) {
	EXPECT_THROW(q0.Dequeue(), RuntimeError);
	EXPECT_EQ(1, q1.Dequeue());
	EXPECT_EQ(0, q1.Size());

	EXPECT_EQ(2, q2.Dequeue());
	q2.Enqueue(10);
	EXPECT_EQ(3, q2.Dequeue());
	EXPECT_EQ(1, q2.Size());
	EXPECT_EQ(0, q0.Size());
	for (int i = 0; i < 1024; ++i) {
		EXPECT_NO_THROW(q0.Enqueue(i));
	}
	for (int i = 0; i < 512; ++i) {
		EXPECT_EQ(i, q0.Dequeue());
	}
	EXPECT_EQ(512, q0.Size());
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
