/*
 * preferred_size_allocator_unittest.cpp
 * Created on: Sep 1, 2014
 * Author: chenguangyu
 */

#include "preferred_size_allocator.h"
#include "common/time/stopwatch.h"
#include <gtest/gtest.h>
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace longan;

TEST(PreferredSizeAllocatorTest, TimeCompare) {
    int length = 256;
    PreferredSizeAllocator<double> alloc(length);
    int allocTimes = 1000000;
    vector<int> sizeArray(allocTimes);
    vector<double*> pArray(allocTimes);
    vector<bool> deletetagArray(allocTimes);
    for (int i = 0; i < allocTimes; ++i) {
        sizeArray[i] = (i % 1000 == 0)?(rand() % (1024 * 32)) : length;
        deletetagArray[i] = (rand() % 3 == 0);
    }

    Stopwatch sw;
    cout << "allocate using PreferredSizeAllocator<int> for preferredSize = " << length << "...";
    sw.Reset(); sw.Start();
    for (int i = 0; i < allocTimes; ++i) {
        pArray[i] = alloc.Allocate(sizeArray[i]);
        ASSERT_TRUE(pArray[i] != nullptr);
        if (deletetagArray[i]) {
            alloc.Deallocate(pArray[i], sizeArray[i]);
        }
    }
    double time1 = sw.ElapsedMilliseconds();
    cout << "total time = " << time1 << endl;

    cout << "allocate using new & delete...";
    sw.Reset(); sw.Start();
    for (int i = 0; i < allocTimes; ++i) {
        pArray[i] = new double[sizeArray[i]];
        ASSERT_TRUE(pArray[i] != nullptr);
        if (deletetagArray[i]) {
            delete [] pArray[i];
        }
    }
    for (int i = 0; i < allocTimes; ++i) {
        if (!deletetagArray[i]) {
            delete [] pArray[i];
        }
    }
    double time2 = sw.ElapsedMilliseconds();
    cout << "total time = " << time2 << endl;

    ASSERT_TRUE(time1 < time2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
