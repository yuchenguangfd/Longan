/*
 * small_memory_allocator_unittest.cpp
 * Created on: Sep 1, 2014
 * Author: chenguangyu
 */

#include "small_memory_allocator.h"
#include "common/time/stopwatch.h"
#include <gtest/gtest.h>
#include <vector>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace longan;

TEST(SmallMemoryAllocatorTest, TimeCompare) {
    SmallMemoryAllocator<double> alloc;
    int allocTimes = 10000000;
    vector<int> sizeArray(allocTimes);
    vector<double*> pArray(allocTimes);
    for (int i = 0; i < allocTimes; ++i) {
        sizeArray[i] = (i % 1000 == 0)?(rand() % (1024 * 32)):(rand() % (32));
    }

    Stopwatch sw;
    cout << "allocate using SmallMemoryAllocator<double>...";
    sw.Reset();sw.Start();
    for (int i = 0; i < allocTimes; ++i) {
        pArray[i] = alloc.Allocate(sizeArray[i]);
        ASSERT_TRUE(pArray[i] != nullptr);
    }
    for (int i = 0; i < allocTimes; ++i) {
        alloc.Deallocate(pArray[i], sizeArray[i]);
    }
    double time1 = sw.ElapsedMilliseconds();
    cout << "total time = " << time1 << endl;

    cout << "allocate using new & delete...";
    sw.Reset();sw.Start();
    for (int i = 0; i < allocTimes; i++) {
        pArray[i] = new double[sizeArray[i]];
        ASSERT_TRUE(pArray[i] != nullptr);
    }
    for (int i = 0; i < allocTimes; ++i) {
        delete []pArray[i];
    }
    double time2 = sw.ElapsedMilliseconds();
    cout << "total time = " << time2 << endl;

    ASSERT_TRUE(time1 < time2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



