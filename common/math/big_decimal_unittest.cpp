/*
 * big_decimal_unittest.cpp
 * Created on: Dec 9, 2014
 * Author: chenguangyu
 */

#include "big_decimal.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(BigDecimalTest, UnitaryMultiplyOK) {
    BigDecimal res0("1");
    BigDecimal num0("95.123");
    for (int i = 0; i < 12; ++i) {
        res0 *= num0;
    }
    ASSERT_EQ("548815620517731830194541.899025343415715973535967221869852721", res0.ToString());

    BigDecimal res1("1");
    BigDecimal num1("0.4321");
    for (int i = 0; i < 20; ++i) {
        res1 *= num1;
    }
    ASSERT_EQ(".00000005148554641076956121994511276767154838481760200726351203835429763013462401", res1.ToString());

    BigDecimal res2("1");
    BigDecimal num2("5.1234");
    for (int i = 0; i < 15; ++i) {
        res2 *= num2;
    }
    ASSERT_EQ("43992025569.928573701266488041146654993318703707511666295476720493953024", res2.ToString());

    BigDecimal res3("1");
    BigDecimal num3("6.7592");
    for (int i = 0; i < 9; ++i) {
        res3 *= num3;
    }
    ASSERT_EQ("29448126.764121021618164430206909037173276672", res3.ToString());

    BigDecimal res4("1");
    BigDecimal num4("98.999");
    for (int i = 0; i < 10; ++i) {
        res4 *= num4;
    }
    ASSERT_EQ("90429072743629540498.107596019456651774561044010001", res4.ToString());


}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
