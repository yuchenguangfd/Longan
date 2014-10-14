/*
 * rating_list_unittest.cpp
 * Created on: Oct 7, 2014
 * Author: chenguangyu
 */

#include "rating_list.h"
#include <gtest/gtest.h>

using namespace longan;
using namespace std;

TEST(RatingListTest, Add_Rating_Record_OK) {
    int numRating = 3, numUser = 2, numItem = 4;
    RatingList list(numUser, numItem, numRating);
    list.Add(RatingRecord(0, 0, 3));
    list.Add(RatingRecord(0, 1, 4));
    list.Add(RatingRecord(1, 1, 2));
    ASSERT_EQ(3, list[0].Rating());
    ASSERT_EQ(0, list[1].UserId());
    ASSERT_EQ(1, list[2].ItemId());
    ASSERT_EQ(2, list.NumUser());
    ASSERT_EQ(4, list.NumItem());
    ASSERT_EQ(3, list.NumRating());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
