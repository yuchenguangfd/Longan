/*
 * rating_trait_unittest.cpp
 * Created on: Oct 5, 2014
 * Author: chenguangyu
 */

#include "rating_trait.h"
#include "common/math/math.h"
#include <gtest/gtest.h>
#include <cmath>

using namespace longan;

class RatingTraitTest : public ::testing::Test {
protected:
    RatingList CreateRatingList() {
        RatingList rlist(3, 3);
        rlist.Add(RatingRecord(0, 0, 1.0f));
        rlist.Add(RatingRecord(0, 1, 2.0f));
        rlist.Add(RatingRecord(0, 2, 3.0f));
        rlist.Add(RatingRecord(1, 0, 4.0f));
        rlist.Add(RatingRecord(1, 1, 5.0f));
        rlist.Add(RatingRecord(1, 2, 6.0f));
        rlist.Add(RatingRecord(2, 0, 7.0f));
        rlist.Add(RatingRecord(2, 1, 8.0f));
        rlist.Add(RatingRecord(2, 2, 9.0f));
        return std::move(rlist);
    }
};

TEST_F(RatingTraitTest, ComputeMinAndMaxRatingOK) {
    RatingTrait rtrait;
    rtrait.Init(CreateRatingList());
    ASSERT_FLOAT_EQ(1.0f, rtrait.MinRating());
    ASSERT_FLOAT_EQ(9.0f, rtrait.MaxRating());
}

TEST_F(RatingTraitTest, ComputeMinAndMaxRatingEveryUserOK) {
    RatingTrait rtrait;
    rtrait.Init(CreateRatingList());
    ASSERT_FLOAT_EQ(1.0f, rtrait.UserMinRating(0));
    ASSERT_FLOAT_EQ(4.0f, rtrait.UserMinRating(1));
    ASSERT_FLOAT_EQ(7.0f, rtrait.UserMinRating(2));
    ASSERT_FLOAT_EQ(3.0f, rtrait.UserMaxRating(0));
    ASSERT_FLOAT_EQ(6.0f, rtrait.UserMaxRating(1));
    ASSERT_FLOAT_EQ(9.0f, rtrait.UserMaxRating(2));
}

TEST_F(RatingTraitTest, ComputeMinAndMaxRatingEveryItemOK) {
    RatingTrait rtrait;
    rtrait.Init(CreateRatingList());
    ASSERT_FLOAT_EQ(1.0f, rtrait.ItemMinRating(0));
    ASSERT_FLOAT_EQ(2.0f, rtrait.ItemMinRating(1));
    ASSERT_FLOAT_EQ(3.0f, rtrait.ItemMinRating(2));
    ASSERT_FLOAT_EQ(7.0f, rtrait.ItemMaxRating(0));
    ASSERT_FLOAT_EQ(8.0f, rtrait.ItemMaxRating(1));
    ASSERT_FLOAT_EQ(9.0f, rtrait.ItemMaxRating(2));
}

TEST_F(RatingTraitTest, ComputeAverageAndStdRatingOK) {
    RatingTrait rtrait;
    rtrait.Init(CreateRatingList());
    ASSERT_FLOAT_EQ(5.0f, rtrait.AverageRating());
    ASSERT_FLOAT_EQ(sqrt(60.0/9), rtrait.StdRating());
}

TEST_F(RatingTraitTest, ComputeAverageAndStdRatingEveryUserOK) {
    RatingTrait rtrait;
    rtrait.Init(CreateRatingList());
    ASSERT_FLOAT_EQ(2.0f, rtrait.UserAverageRating(0));
    ASSERT_FLOAT_EQ(5.0f, rtrait.UserAverageRating(1));
    ASSERT_FLOAT_EQ(8.0f, rtrait.UserAverageRating(2));
    ASSERT_TRUE(Math::Abs(sqrt(2.0f/3) - rtrait.UserStdRating(0)) < 1e-5);
    ASSERT_TRUE(Math::Abs(sqrt(2.0f/3) - rtrait.UserStdRating(1)) < 1e-5);
    ASSERT_TRUE(Math::Abs(sqrt(2.0f/3) - rtrait.UserStdRating(2)) < 1e-5);
}

TEST_F(RatingTraitTest, ComputeAverageAndStdRatingEveryItemOK) {
    RatingTrait rtrait;
    rtrait.Init(CreateRatingList());
    ASSERT_FLOAT_EQ(4.0f, rtrait.ItemAverageRating(0));
    ASSERT_FLOAT_EQ(5.0f, rtrait.ItemAverageRating(1));
    ASSERT_FLOAT_EQ(6.0f, rtrait.ItemAverageRating(2));
    ASSERT_TRUE(Math::Abs(sqrt(6.0f) - rtrait.ItemStdRating(0)) < 1e-5);
    ASSERT_TRUE(Math::Abs(sqrt(6.0f) - rtrait.ItemStdRating(1)) < 1e-5);
    ASSERT_TRUE(Math::Abs(sqrt(6.0f) - rtrait.ItemStdRating(2)) < 1e-5);

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
