/*
 * object_buffer_unittest.cpp
 * Created on: Oct 30, 2014
 * Author: chenguangyu
 */

#include "object_buffer.h"
#include <thread>
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace longan;

typedef ObjectBuffer<int> IntBuffer;
const int MAX_PUSH_SIZE = 1000;

class Producer {
public:
    Producer(IntBuffer* pBuffer, int pushCount = 100000, bool closeOrFail = true):
        mpBuffer(pBuffer),
        mPushCount(pushCount),
        mCloseOrFail(closeOrFail) { }
    void operator() () {
        int rtn = 0;
        int64 id;
        for (int i = 0; i < mPushCount; i++) {
            rtn = mpBuffer->Push(i, id);
            if (LONGAN_SUCC != rtn) {
                break;
            }
            // check id integrity if single producer
            if (mpBuffer->NumProducer() == 1) {
                EXPECT_EQ(i, id);
            }
        }
        if (mCloseOrFail) {
            rtn = mpBuffer->ProducerQuit();
        } else {
            rtn = mpBuffer->ForceFail();
        }
        if (rtn != LONGAN_SUCC) {
        }
    }

private:
    IntBuffer *mpBuffer;
    int mPushCount;
    bool mCloseOrFail;
};

class Consumer {
public:
    Consumer(IntBuffer* pBuffer, int popCount = 100000, bool closeOrFail = true):
        mpBuffer(pBuffer),
        mPopCount(popCount),
        mCloseOrFail(closeOrFail) {}
    void operator() () {
        int i = 0, j = 0;
        int64 id = 0;
        int rtn = 0;
        while (LONGAN_SUCC == rtn) {
            i++;
            if (i > mPopCount) {
                if (rtn != LONGAN_SUCC) {
                    FAIL();
                }
                break;
            }
            rtn = mpBuffer->Pop(j, id);
            if (rtn != LONGAN_SUCC) {
                break;
            }
            // check id integrity if single producer
            if (mpBuffer->NumConsumer() == 1)
                EXPECT_EQ(j, id);
        }
        if (mCloseOrFail) {
            rtn = mpBuffer->ConsumerQuit();
        } else {
             rtn = mpBuffer->ForceFail();
        }
    }
private:
    IntBuffer* mpBuffer;
    int mPopCount;
    bool mCloseOrFail;
};

TEST(ObjectBufferTest, ProducerCloseOk) {
    IntBuffer intBuffer("producer_close_buffer", 100, 4, 4);
    Producer producer0(&intBuffer, MAX_PUSH_SIZE);
    Producer producer1(&intBuffer, MAX_PUSH_SIZE);
    Producer producer2(&intBuffer, MAX_PUSH_SIZE);
    Producer producer3(&intBuffer, MAX_PUSH_SIZE);
    Consumer consumer0(&intBuffer, MAX_PUSH_SIZE);
    Consumer consumer1(&intBuffer, MAX_PUSH_SIZE);
    Consumer consumer2(&intBuffer, MAX_PUSH_SIZE);
    Consumer consumer3(&intBuffer, MAX_PUSH_SIZE);
    std::thread producerThread0(producer0);
    std::thread producerThread1(producer1);
    std::thread producerThread2(producer2);
    std::thread producerThread3(producer3);
    sleep(2);
    std::thread consumerThread0(consumer0);
    std::thread consumerThread1(consumer1);
    std::thread consumerThread2(consumer2);
    std::thread consumerThread3(consumer3);
    producerThread0.join();
    producerThread1.join();
    producerThread2.join();
    producerThread3.join();
    consumerThread0.join();
    consumerThread1.join();
    consumerThread2.join();
    consumerThread3.join();
    int rtn = 0;
    rtn = intBuffer.Push(123);
    EXPECT_EQ(rtn, LONGAN_OBJECT_BUFFER_NO_ALIVE_CONSUMER);
    int t;
    rtn = intBuffer.Pop(t);
    EXPECT_EQ(rtn, LONGAN_OBJECT_BUFFER_NO_MORE_OBJECT);
}

TEST(ObjectBufferTest, ConsumerCloseOK) {
    IntBuffer intBuffer("consumer_close_buffer", 100, 4, 4);
    Producer producer0(&intBuffer, MAX_PUSH_SIZE);
    Producer producer1(&intBuffer, MAX_PUSH_SIZE);
    Producer producer2(&intBuffer, MAX_PUSH_SIZE);
    Producer producer3(&intBuffer, MAX_PUSH_SIZE);
    Consumer consumer0(&intBuffer, 10);
    Consumer consumer1(&intBuffer, 10);
    Consumer consumer2(&intBuffer, 10);
    Consumer consumer3(&intBuffer, 10);
    std::thread producerThread0(producer0);
    std::thread producerThread1(producer1);
    std::thread producerThread2(producer2);
    std::thread producerThread3(producer3);
    usleep(100000);
    std::thread consumerThread0(consumer0);
    std::thread consumerThread1(consumer1);
    std::thread consumerThread2(consumer2);
    std::thread consumerThread3(consumer3);
    producerThread0.join();
    producerThread1.join();
    producerThread2.join();
    producerThread3.join();
    consumerThread0.join();
    consumerThread1.join();
    consumerThread2.join();
    consumerThread3.join();
    int rtn = 0;
    rtn = intBuffer.Push(123);
    EXPECT_EQ(rtn, LONGAN_OBJECT_BUFFER_NO_ALIVE_CONSUMER);
    int t;
    rtn = intBuffer.Pop(t);
    EXPECT_EQ(rtn, LONGAN_OBJECT_BUFFER_NO_MORE_OBJECT);
}

TEST(ObjectBufferTest, IndexIntegrityOK) {
    IntBuffer buffer("index_integrity_buffer", 100, 1, 1);
    Producer producer0(&buffer, MAX_PUSH_SIZE);
    Consumer consumer0(&buffer);
    std::thread producerThread0(producer0);
    std::thread consumerThread0(consumer0);
    producerThread0.join();
    consumerThread0.join();
}

TEST(ObjectBufferTest, ForceFailOK) {
    int rtn = 0;
    IntBuffer buffer("forcefail_buffer", 100, 3, 2);
    Producer producer0(&buffer, MAX_PUSH_SIZE, false);
    Producer producer1(&buffer, MAX_PUSH_SIZE, true);
    Producer producer2(&buffer, MAX_PUSH_SIZE, true);
    Consumer consumer0(&buffer);
    Consumer consumer1(&buffer);
    std::thread producerThread0(producer0);
    std::thread producerThread1(producer1);
    std::thread producerThread2(producer2);
    std::thread consumerThread0(consumer0);
    std::thread consumerThread1(consumer1);
    IntBuffer buffer2("forcefail_buffer2", 10, 2, 1);
    Producer producer3(&buffer2, MAX_PUSH_SIZE);
    std::thread producerThread3(producer3);
    usleep(10000);
    rtn = buffer2.ForceFail();
    EXPECT_EQ(rtn, LONGAN_SUCC);
    producerThread3.join();
    rtn = buffer2.ForceFail();
    EXPECT_EQ(rtn, LONGAN_SUCC);
    producerThread0.join();
    producerThread1.join();
    producerThread2.join();
    consumerThread0.join();
    consumerThread1.join();
}

TEST(ObjectBufferTest, Somke) {
    int bufSize = 100;
    for (int i = 0; i < 5; i++) {
        bufSize *= 5;
        IntBuffer intBuffer("buffer", 1000, 1, 1);
        Producer producer0(&intBuffer, bufSize);
        Consumer consumer0(&intBuffer, bufSize);
        std::thread producerThread0(producer0);
        std::thread consumerThread0(consumer0);
        producerThread0.join();
        consumerThread0.join();
    }
    bufSize = 100;
    for (int i = 0; i < 5; i++) {
        bufSize *= 5;
        IntBuffer intBuffer("buffer", 1000, 4, 4);
        Producer producer0(&intBuffer, bufSize);
        Producer producer1(&intBuffer, bufSize);
        Producer producer2(&intBuffer, bufSize);
        Producer producer3(&intBuffer, bufSize);
        Consumer consumer0(&intBuffer, bufSize);
        Consumer consumer1(&intBuffer, bufSize);
        Consumer consumer2(&intBuffer, bufSize);
        Consumer consumer3(&intBuffer, bufSize);
        std::thread producerThread0(producer0);
        std::thread producerThread1(producer1);
        std::thread producerThread2(producer2);
        std::thread producerThread3(producer3);
        std::thread consumerThread0(consumer0);
        std::thread consumerThread1(consumer1);
        std::thread consumerThread2(consumer2);
        std::thread consumerThread3(consumer3);
        producerThread0.join();
        producerThread1.join();
        producerThread2.join();
        producerThread3.join();
        consumerThread0.join();
        consumerThread1.join();
        consumerThread2.join();
        consumerThread3.join();
    }
}

TEST(ObjectBufferTest, TryPushPopOK) {
    int rtn;
    IntBuffer buffer("buffer", 100, 1, 1);
    for (int i = 0; i < 10; ++i) {
        rtn = buffer.Push(i);
        ASSERT_EQ(rtn, LONGAN_SUCC);
    }
    rtn = buffer.TryPush(10);
    ASSERT_EQ(rtn, LONGAN_SUCC);
    for (int i = 0; i < 10; ++i) {
        int data;
        int64 id;
        rtn = buffer.TryPop(data, id);
        ASSERT_EQ(rtn, LONGAN_SUCC);
        ASSERT_EQ(data, i);
        ASSERT_EQ(data, id);
    }
    int data;
    rtn = buffer.Pop(data);
    ASSERT_EQ(rtn, LONGAN_SUCC);
    ASSERT_EQ(data, 10);
    rtn = buffer.TryPop(data);
    ASSERT_EQ(rtn, LONGAN_OBJECT_BUFFER_TRY_POP_FAILED);
    for (int i = 0; i < 100; ++i) {
        rtn = buffer.Push(i);
        ASSERT_EQ(rtn, LONGAN_SUCC);
        ASSERT_EQ(buffer.Size(), i + 1);
    }
    rtn = buffer.TryPush(100);
    ASSERT_EQ(rtn, LONGAN_OBJECT_BUFFER_TRY_PUSH_FAILED);
    buffer.Pop(data);
    ASSERT_EQ(buffer.Size(), 99);
    rtn = buffer.ProducerQuit();
    ASSERT_EQ(rtn, LONGAN_SUCC);
    for (int i = 0; i < 99; ++i) {
        ASSERT_EQ(buffer.Size(), 99 - i);
        rtn = buffer.TryPop(data);
        ASSERT_EQ(rtn, LONGAN_SUCC);
    }
    rtn = buffer.TryPop(data);
    ASSERT_EQ(rtn, LONGAN_OBJECT_BUFFER_NO_MORE_OBJECT);
    rtn = buffer.ConsumerQuit();
    ASSERT_EQ(rtn, LONGAN_SUCC);
    rtn = buffer.TryPush(0);
    ASSERT_EQ(rtn, LONGAN_OBJECT_BUFFER_NO_ALIVE_CONSUMER);
    rtn = buffer.ForceFail();
    ASSERT_EQ(rtn, LONGAN_SUCC);
    rtn = buffer.TryPush(data);
    ASSERT_EQ(rtn, LONGAN_OBJECT_BUFFER_FAILED);
    rtn = buffer.TryPop(data);
    ASSERT_EQ(rtn, LONGAN_OBJECT_BUFFER_FAILED);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
