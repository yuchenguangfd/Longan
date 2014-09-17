/*
 * queue_unittest.cpp
 * Created on: Aug 20, 2014
 * Author: chenguangyu
 */

#include "queue.h"
#include <gtest/gtest.h>
#include <string>

using namespace longan;
using namespace std;

TEST(QueueTest, SmallCase) {
    Queue<string> queue;
    ASSERT_TRUE(queue.Empty());
    queue.Enqueue("111");
    queue.Enqueue("22");
    queue.Enqueue("3333");
    ASSERT_FALSE(queue.Empty());
    ASSERT_EQ("111", queue.Front());
    ASSERT_EQ("3333", queue.Rear());
    ASSERT_EQ("111", queue.Dequeue());
    ASSERT_EQ(2, queue.Size());
}

TEST(QueueTest, LargeCase) {
	Queue<double> queue;
	for (int i = 0 ; i < 2048; ++i) {
	    queue.Enqueue(i / 100);
	    ASSERT_EQ(i+1, queue.Size());
	}
	ASSERT_EQ(2048, queue.Size());
	for (int i = 0; i < 2048; ++i) {
	    queue.Enqueue(i * 0.618);
	    queue.Dequeue();
	    queue.Dequeue();
	}
	ASSERT_EQ(0, queue.Size());
	ASSERT_THROW(queue.Dequeue(), RuntimeError);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
