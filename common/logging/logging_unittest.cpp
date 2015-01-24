/*
 * logging_unittest.cpp
 * Created on: Jan 24, 2015
 * Author: chenguangyu
 */

#include "logging.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(LogTest, Demo) {
    const std::string tag = "log_test";
    Log::I(tag, "hello");
    Log::W(tag, "hello");
    Log::E(tag, "hello");
    Log::Console(tag, "hello");
    const std::string msg("world");
    Log::I(tag, msg);
    Log::W(tag, msg);
    Log::E(tag, msg);
    Log::Console(tag, msg);
    int i = 42;
    double d = 4.2;
    char c = '*';
    Log::I(tag, "%d,%lf,%c,%s", i, d, c, "woo~");
    Log::W(tag, "%d,%lf,%c,%s", i, d, c, "woo~");
    Log::E(tag, "%d,%lf,%c,%s", i, d, c, "woo~");
    Log::Console(tag, "%d,%lf,%c,%s", i, d, c, "woo~");
}

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
