/*
 * running_statistic_unittest.cpp
 * Created on: Jul 21, 2014
 * Author: chenguangyu
 */

#include "running_statistic.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(RunningMaxMin, All) {
    RunningMin<int> rmin;
    RunningMax<double> rmax;
    int numbers[] = {-1, -2, -3, -4, -5};
    for (int n : numbers) {
        rmin.Add(n);
        rmax.Add(n);
    }
    ASSERT_EQ(-5, rmin.CurrentMin());
    ASSERT_EQ(-1, rmax.CurrentMax());
    for (int n : numbers) {
        rmin.Add(-n);
        rmax.Add(-n);
    }
    ASSERT_EQ(-5, rmin.CurrentMin());
    ASSERT_EQ(5, rmax.CurrentMax());
}

TEST(RunningAverage, All) {
    RunningAverage<double> ravg;
    for (int i = 0; i < 10; ++i) {
        ravg.Add(i);
    }
    ASSERT_DOUBLE_EQ(4.5, ravg.CurrentAverage());
    ravg.Add(10.0);
    ASSERT_DOUBLE_EQ(5, ravg.CurrentMean());
}

TEST(RunningStd, All) {
    int numbers[10] = {1, 3, 5, 7, 9, 2, 4, 6, 8, 10};
    RunningStd<float> rstd;
    for(int x : numbers) {
        rstd.Add(x);
    }
    ASSERT_FLOAT_EQ(5.5, rstd.CurrentMean());
    ASSERT_FLOAT_EQ(8.25, rstd.CurrentVar());
    ASSERT_FLOAT_EQ(sqrt(8.25), rstd.CurrentStd());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
