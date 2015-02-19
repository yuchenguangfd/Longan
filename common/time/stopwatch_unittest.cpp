/*
 * stopwatch_unittest.cpp
 * Created on: Jul 24, 2014
 * Author: chenguangyu
 */

#include "stopwatch.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(StopwatchTest, All) {
    Stopwatch sw;
    double t1 = 0.0, t2;
    sw.Tic();
    for (int i = 0; i < 10; ++i) {
        for (int k = 0; k < 1000000; ++k) { int b = 0; }
        t2 = sw.Toc();
        EXPECT_GE(t2, t1);
        t1 = t2;
    }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
