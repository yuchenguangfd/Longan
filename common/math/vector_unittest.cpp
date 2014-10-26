/*
 * vector_unittest.cpp
 * Created on: Sep 4, 2014
 * Author: chenguangyu
 */

#include "vector.h"
#include <gtest/gtest.h>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;
using namespace longan;

TEST(VectorTest, ConstructorOK) {
    Vector<int> v0;
    ASSERT_EQ(0, v0.Size());
    ASSERT_EQ(nullptr, v0.Data());

    Vector<int> v1(3);
    ASSERT_EQ(3, v1.Size());
    ASSERT_NE(nullptr, v1.Data());

    Vector<int> v2(1, true);
    ASSERT_EQ(0, v2[0]);

    Vector<int> v3(2, true, 42);
    ASSERT_EQ(42, v3[0]);
    ASSERT_EQ(42, v3[1]);

    int data[] = {1, 23, 456};
    Vector<int> v4(std::begin(data), std::end(data));
    ASSERT_EQ(1, v4[0]);
    ASSERT_EQ(23, v4[1]);
    ASSERT_EQ(456, v4[2]);

    Vector<int> v5(v4);
    ASSERT_EQ(v4[0], v5[0]);
    ASSERT_EQ(v4[1], v5[1]);
    ASSERT_EQ(v4[2], v5[2]);

    Vector<int> v6 = std::move(v5);
    ASSERT_EQ(1, v6[0]);
    ASSERT_EQ(23, v6[1]);
    ASSERT_EQ(456, v6[2]);
    ASSERT_EQ(0, v5.Size());
    ASSERT_EQ(nullptr, v5.Data());
}

TEST(VectorTest, CopyAndMoveOK) {
    int data[] = {1, 23, 456};
    Vector<int> v0(std::begin(data), std::end(data));
    Vector<int> v1, v2;
    v1 = v0;
    ASSERT_EQ(3, v0.Size());
    ASSERT_EQ(456, v1[2]);
    v2 = std::move(v0);
    ASSERT_EQ(0, v0.Size());
    ASSERT_EQ(nullptr, v0.Data());
    ASSERT_EQ(23, v2[1]);
}

TEST(VectorTest, EqualAndNotEqualOK) {
    int data[] = {42, 43, 44};
    Vector<int> v1(data, data + 3), v2(data, data + 3);
    ASSERT_TRUE(v1 == v2);
    v2[1] = -v2[1];
    ASSERT_TRUE(v1 != v2);
}

TEST(VectorTest, BinaryAddOK) {
    int data1[] = {1, 2, 3};
    int data2[] = {4, 5, 6};
    int data3[] = {5, 7, 9};
    Vector<int> v1(data1, data1+3), v2(data2, data2+3), v3(data3, data3+3);
    ASSERT_EQ(v3, v1 + v2);
}

TEST(VectorTest, BinarySubOK) {
    int data1[] = {1, 2, 3};
    int data2[] = {4, 5, 6};
    int data3[] = {-3, -3, -3};
    Vector<int> v1(data1, data1+3), v2(data2, data2+3), v3(data3, data3+3);
    ASSERT_EQ(v3, v1 - v2);
}

TEST(VectorTest, InnerProdOK) {
    double data1[] = {1.0, 2.0, 3.0};
    double data2[] = {4.0, 5.0, 6.0};
    Vector<double> v1(data1, data1+3), v2(data2, data2+3);
    ASSERT_DOUBLE_EQ(32.0, InnerProd(v1, v2));
}

TEST(VectorTest, StaticFactoryOK) {
    Vector<double> v1;
    v1 = Vector<double>::Zeros(2);
    ASSERT_EQ(2, v1.Size());
    v1 = Vector<double>::Rand(3);
    ASSERT_EQ(3, v1.Size());
    v1 = Vector64F::Randn(4);
    ASSERT_EQ(4, v1.Size());
}

TEST(VectorTest, OutputFormatOK) {
    Vector<int> vec(3);
    vec[0] = 12; vec[1] = 34, vec[2] = 5;
    std::ostringstream oss;
    oss << vec;
    ASSERT_EQ("(12,34,5)", oss.str());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
