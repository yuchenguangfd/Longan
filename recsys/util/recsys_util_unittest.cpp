/*
 * recsys_util_unittest.cpp
 * Created on: Mar 8, 2015
 * Author: chenguangyu
 */

#include "recsys_util.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(RecsysUtilTest, CreateRandomDataOK) {
    int numUser = 600;
    int numItem = 500;
    int numRating = 200000;
    RatingList rlist1 = RecsysUtil::RandomRatingList(numUser, numItem, numRating, false, 1.0f, 5.0f);
    ASSERT_EQ(numUser, rlist1.NumUser());
    ASSERT_EQ(numItem, rlist1.NumItem());
    ASSERT_EQ(numRating, rlist1.NumRating());
    RatingList rlist2 = RecsysUtil::RandomRatingList(numUser, numItem, numRating, true);
    for (int i = 0; i < rlist2.Size(); ++i) {
        ASSERT_TRUE(rlist2[i].Rating() == 0.0f || rlist2[i].Rating() == 1.0f);
    }
    RatingMatUsers rmat1 = RecsysUtil::RandomRatingMatUsers(numUser, numItem, numRating);
    RatingMatItems rmat2 = RecsysUtil::RandomRatingMatItems(numUser, numItem, numRating);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
