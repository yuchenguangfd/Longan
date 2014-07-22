/*
 * statistic_unittest.cpp
 * Created on: Jul 21, 2014
 * Author: chenguangyu
 */

#include "statistic.h"
#include <gtest/gtest.h>
#include <cmath>

using namespace longan;

TEST(RunningStatisticTest, All) {
    int numbers[10] = {1, 3, 5, 7, 9, 2, 4, 6, 8, 10};
    RunningStatistic stat;
    for(int x : numbers) {
        stat.Accumulate(x);
    }
    EXPECT_EQ(5.5, stat.GetMean());
    EXPECT_EQ(5.5, stat.GetAverage());
    EXPECT_EQ(8.25, stat.GetVariance());
    EXPECT_DOUBLE_EQ(sqrt(8.25), stat.GetStandardDeviation());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
