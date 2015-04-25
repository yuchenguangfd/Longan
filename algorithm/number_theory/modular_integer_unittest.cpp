/*
 * modular_integer_unittest.cpp
 * Created on: Apr 25, 2015
 * Author: chenguangyu
 */

#include "modular_integer.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(ModularIntegerTest, AddOK) {
    ModularInteger::SetModular(100);
	ModularInteger mi1(1234), mi2(4321);
    EXPECT_EQ(ModularInteger(355), mi1 + mi2);
    mi1 += mi2;
    EXPECT_EQ(ModularInteger(455), mi1);
}

TEST(ModularIntegerTest, MultiplyOK) {
	ModularInteger::SetModular(100);
	ModularInteger mi1(1234), mi2(4321);
    EXPECT_EQ(ModularInteger(1014), mi1 * mi2);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
