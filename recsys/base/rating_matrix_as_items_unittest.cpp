/*
 * rating_matrix_as_items_unittest.cpp
 * Created on: Oct 4, 2014
 * Author: chenguangyu
 */

#include "rating_matrix_as_items.h"
#include "recsys/util/recsys_test_helper.h"
#include <gtest/gtest.h>
#include <memory>

using namespace longan;
using namespace std;

class RatingMatrixAsItemsTest : public ::testing::Test {
protected:
protected:
    int mNumRating;
    int mNumUser;
    int mNumItem;
};

TEST_F(RatingMatrixAsItemsTest, Init_From_RatingList_OK) {
    RatingList rlist = RecsysTestHelper::CreateRandomRatingList(50, 60, 1000);
    double checkResult1 = 0.0;
    for (int i = 0; i < rlist.Size(); ++i) {
        checkResult1 += rlist[i].UserId() - rlist[i].ItemId() + rlist[i].Rating();
    }

    RatingMatrixAsItems<> rmat;
    ASSERT_EQ(0, rmat.NumUser());
    ASSERT_EQ(0, rmat.NumItem());

    rmat.Init(rlist);
    ASSERT_EQ(rlist.NumUser(), rmat.NumUser());
    ASSERT_EQ(rlist.NumItem(), rmat.NumItem());
    double checkResult2 = 0.0;
    for (int itemId = 0; itemId < rmat.NumItem(); ++itemId) {
        auto& ivec = rmat.GetItemVector(itemId);
        for (auto iter = ivec.Data(), end = ivec.Data()+ivec.Size(); iter != end; ++iter) {
            checkResult2 += iter->UserId() - itemId + iter->Rating();
        }
    }
    ASSERT_DOUBLE_EQ(checkResult1, checkResult2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
