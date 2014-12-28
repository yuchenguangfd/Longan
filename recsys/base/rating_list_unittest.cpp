/*
 * rating_list_unittest.cpp
 * Created on: Oct 7, 2014
 * Author: chenguangyu
 */

#include "rating_list.h"
#include "common/system/file_util.h"
#include <gtest/gtest.h>

using namespace longan;
using namespace std;

TEST(RatingListTest, AddRatingRecordOK) {
    int numRating = 3, numUser = 2, numItem = 4;
    RatingList rlist(numUser, numItem, numRating);
    rlist.Add(RatingRecord(0, 0, 3));
    rlist.Add(RatingRecord(0, 1, 4));
    rlist.Add(RatingRecord(1, 1, 2));
    ASSERT_EQ(3, rlist[0].Rating());
    ASSERT_EQ(0, rlist[1].UserId());
    ASSERT_EQ(1, rlist[2].ItemId());
    ASSERT_EQ(2, rlist.NumUser());
    ASSERT_EQ(4, rlist.NumItem());
    ASSERT_EQ(3, rlist.NumRating());
}

TEST(RatingListTest, LoadAndWriteOK) {
    int numRating = 3, numUser = 2, numItem = 4;
    RatingList rlist(numUser, numItem, numRating);
    rlist.Add(RatingRecord(0, 0, 3));
    rlist.Add(RatingRecord(0, 1, 4));
    rlist.Add(RatingRecord(1, 1, 2));
    RatingList::WriteToBinaryFile(rlist, "tmp.bin");
    RatingList::WriteToTextFile(rlist, "tmp.txt");
    RatingList rlist2 = RatingList::LoadFromBinaryFile("tmp.bin");
    RatingList rlist3 = RatingList::LoadFromTextFile("tmp.txt");
    FileUtil::Delete("tmp.bin");
    FileUtil::Delete("tmp.txt");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
