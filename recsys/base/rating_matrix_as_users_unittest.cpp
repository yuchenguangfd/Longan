/*
 * rating_matrix_as_users_unittest.cpp
 * Created on: Oct 4, 2014
 * Author: chenguangyu
 */

#include "rating_matrix_as_users.h"
#include "recsys/util/recsys_test_helper.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(RatingMatrixAsUsersTest, InitFromRatingListOK) {
    RatingList rlist = RecsysTestHelper::CreateRandomRatingList(50, 60, 1000);
    double checkResult1 = 0.0;
    for (int i = 0; i < rlist.Size(); ++i) {
        checkResult1 += rlist[i].ItemId() - rlist[i].UserId() + rlist[i].Rating();
    }
    RatingMatUsers rmat;
    rmat.Init(rlist);
    ASSERT_EQ(rlist.NumRating(), rmat.NumRating());
    ASSERT_EQ(rlist.NumUser(), rmat.NumUser());
    ASSERT_EQ(rlist.NumItem(), rmat.NumItem());
    double checkResult2 = 0.0;
    for (int userId = 0; userId < rmat.NumUser(); ++userId) {
        const UserVec& uvec = rmat.GetUserVector(userId);
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
