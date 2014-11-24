/*
 * matrix_unittest.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "matrix.h"
#include <iterator>
#include <gtest/gtest.h>

using namespace longan;
using namespace std;

TEST(MatrixTest, ConstructorOK) {
    Matrix<double> m0;
    ASSERT_EQ(0, m0.Rows());
    ASSERT_EQ(0, m0.Cols());
    ASSERT_EQ(nullptr, m0.Data());

    Matrix<double> m1(2, 3);
    ASSERT_EQ(2, m1.Rows());
    ASSERT_EQ(3, m1.Cols());
    ASSERT_NE(nullptr, m1.Data());

    Matrix<int> m2(2, 3, true);
    ASSERT_EQ(0, m2[1][2]);

    Matrix<int> m3(2, 3, true, 42);
    ASSERT_EQ(42, m3[0][1]);
    ASSERT_EQ(42, m3[1][1]);

    m3[1][0] = 24; m3[1][2] = 12;
    Matrix<int> m4(m3);
    for (int i = 0; i < m4.Rows(); ++i) {
        for (int j = 0; j < m4.Cols(); ++j) {
            ASSERT_EQ(m3[i][j], m4[i][j]);
        }
    }

    Matrix<int> m5 = std::move(m4);
    for (int i = 0; i < m5.Rows(); ++i) {
        for (int j = 0; j < m5.Cols(); ++j) {
            ASSERT_EQ(m3[i][j], m5[i][j]);
        }
    }
    ASSERT_EQ(0, m4.Rows());
    ASSERT_EQ(0, m4.Cols());
    ASSERT_EQ(nullptr, m4.Data());
}

TEST(MatrixTest, CopyAndMoveOK) {
    int data[] = {1, 2, 3, 4, 5, 6};
    Matrix<int> m0(3, 2, std::begin(data), std::end(data));
    Matrix<int> m1, m2;

    m1 = m0;
    for (int i = 0; i < m1.Rows(); ++i) {
       for (int j = 0; j < m1.Cols(); ++j) {
           ASSERT_EQ(m0[i][j], m1[i][j]);
       }
    }
    m2 = std::move(m0);
    for (int i = 0; i < m2.Rows(); ++i) {
       for (int j = 0; j < m2.Cols(); ++j) {
           ASSERT_EQ(m1[i][j], m2[i][j]);
       }
    }
    ASSERT_EQ(nullptr, m0.Data());
}

TEST(MatrixTest, BinaryMulMatrixOK) {
    int data1[] = {1, 2, 3,
                   4, 5, 6};
    int data2[] = {1, 2,
                   3, 4,
                   5, 6};
    int data3[] = {22, 28,
                   49, 64};
    Matrix<int> m1(2, 3, std::begin(data1), std::end(data1));
    Matrix<int> m2(3, 2, std::begin(data2), std::end(data2));
    Matrix<int> m3(2, 2, std::begin(data3), std::end(data3));
    ASSERT_EQ(m3, m1 * m2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
