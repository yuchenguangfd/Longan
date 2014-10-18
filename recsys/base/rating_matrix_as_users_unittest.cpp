/*
 * rating_matrix_as_users_unittest.cpp
 * Created on: Oct 4, 2014
 * Author: chenguangyu
 */

#include "rating_matrix_as_users.h"
#include "common/util/array_helper.h"
#include <gtest/gtest.h>
#include <memory>

using namespace longan;
using namespace std;

class RatingMatrixAsUsersTest : public ::testing::Test {
protected:
    RatingList GenerateRatingList() {
        mNumRating = 1000;
        mNumUser = 50;
        mNumItem = 60;
        int **mat;
        ArrayHelper::CreateArray2D(&mat, mNumUser, mNumItem);
        ArrayHelper::InitArray2D(mat, mNumUser, mNumItem, -1);
        for (int i = 0; i < mNumRating; ++i) {
            while (true) {
                int r = Random::Instance().Uniform(0, mNumUser);
                int c = Random::Instance().Uniform(0, mNumItem);
                if (mat[r][c] == -1) {
                    mat[r][c] = Random::Instance().Uniform(1, 6);
                    break;
                }
            }
        }
        RatingList rlist(mNumUser, mNumItem, mNumRating);
        for (int i = 0; i < mNumUser; ++i) {
            for (int j = 0; j < mNumItem; ++j) {
                if (mat[i][j] != -1) {
                    rlist.Add(RatingRecord(i, j, mat[i][j]));
                }
            }
        }
        return std::move(rlist);
    }
protected:
    int mNumRating;
    int mNumUser;
    int mNumItem;
};

TEST_F(RatingMatrixAsUsersTest, Init_From_RatingList_OK) {
    RatingList rlist = GenerateRatingList();
    double checkResult1 = 0.0;
    for (int i = 0; i < rlist.Size(); ++i) {
        checkResult1 += rlist[i].ItemId() - rlist[i].UserId() + rlist[i].Rating();
    }

    RatingMatrixAsUsers<> rmat;
    ASSERT_EQ(0, rmat.NumUser());
    ASSERT_EQ(0, rmat.NumItem());

    rmat.Init(rlist);
    ASSERT_EQ(rlist.NumUser(), rmat.NumUser());
    ASSERT_EQ(rlist.NumItem(), rmat.NumItem());
    double checkResult2 = 0.0;
    for (int userId = 0; userId < rmat.NumUser(); ++userId) {
        auto& uvec = rmat.GetUserVector(userId);
        for (auto iter = uvec.Begin(), end = uvec.End(); iter != end; ++iter) {
            checkResult2 += iter->ItemId() - userId + iter->Rating();
        }
    }
    ASSERT_DOUBLE_EQ(checkResult1, checkResult2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
