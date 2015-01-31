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

    int data4[] = {1, 3, 5};
    int data5[] = {22, 49};
    Vector<int> v1(std::begin(data4), std::end(data4));
    Vector<int> v2(std::begin(data5), std::end(data5));
    ASSERT_EQ(v2, m1 * v1);

    int data6[] = {1, 2, 3, 4, 3, 8, 9, 64};
    Matrix<int> m4(1, 2, data6, data6+2);
    Matrix<int> m5(1, 2, data6+2, data6+4);
    Matrix<int> m6(1, 2, data6+4, data6+6);
    Matrix<int> m7(1, 2, data6+6, data6+8);
    ASSERT_EQ(m6, MultiplyElementWise(m4, m5));
    ASSERT_EQ(m7, MultiplyElementWise(m4, m5, m6));
}

TEST(MatrixTest, OutterProdOK) {
    int data1[] = {1, 2, 3};
    int data2[] = {4, 5, 6};
    int data3[] = {4, 5, 6,
                   8,10,12,
                  12,15,18};
    Vector<int> v1(data1, data1+3), v2(data2, data2+3);
    Matrix<int> m3(3, 3, data3, data3+9);
    ASSERT_EQ(m3, OutterProd(v1, v2));
}

TEST(MatrixTest, NormOK) {
    Matrix<double> m1(2, 2);
    m1[0][0] = -3; m1[0][1] = 4;
    m1[1][0] = 5; m1[1][1] = 6;
    ASSERT_EQ(86.0, NormFSqr(m1));
    ASSERT_DOUBLE_EQ(9.273618495495704, NormF(m1));
}

TEST(MatrixTest, TransposeOK) {
    int data1[] = {1, 2, 3, 4, 5, 6};
    int data2[] = {1, 4, 2, 5, 3, 6};
    Matrix<int> m1(2, 3, std::begin(data1), std::end(data1));
    Matrix<int> m2(3, 2, std::begin(data2), std::end(data2));
    ASSERT_EQ(m2, m1.Transpose());
}

TEST(MatrixTest, OutputFormatOK) {
    Matrix<int> mat(2,2);
    mat[0][0] = 1; mat[0][1] = 2;
    mat[1][0] = 3; mat[1][1] = 5;
    std::ostringstream oss;
    oss << mat;
    ASSERT_EQ("1,2\n3,5\n", oss.str());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
