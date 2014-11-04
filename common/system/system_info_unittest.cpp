/*
 * system_info_unittest.cpp
 * Created on: Nov 4, 2014
 * Author: chenguangyu
 */

#include "system_info.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(SystemInfoTest, NumCpuCoreOK) {
    int numCore = SystemInfo::GetNumCPUCore();
	ASSERT_LE(1, numCore);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
