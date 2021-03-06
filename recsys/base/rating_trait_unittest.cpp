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
    ASSERT_FLOAT_EQ(1.0f, rtrait.Min());
    ASSERT_FLOAT_EQ(9.0f, rtrait.Max());
}

TEST_F(RatingTraitTest, ComputeMinAndMaxRatingEveryUserOK) {
    RatingTrait rtrait;
    rtrait.Init(CreateRatingList());
    ASSERT_FLOAT_EQ(1.0f, rtrait.UserMin(0));
    ASSERT_FLOAT_EQ(4.0f, rtrait.UserMin(1));
    ASSERT_FLOAT_EQ(7.0f, rtrait.UserMin(2));
    ASSERT_FLOAT_EQ(3.0f, rtrait.UserMax(0));
    ASSERT_FLOAT_EQ(6.0f, rtrait.UserMax(1));
    ASSERT_FLOAT_EQ(9.0f, rtrait.UserMax(2));
}

TEST_F(RatingTraitTest, ComputeMinAndMaxRatingEveryItemOK) {
    RatingTrait rtrait;
    rtrait.Init(CreateRatingList());
    ASSERT_FLOAT_EQ(1.0f, rtrait.ItemMin(0));
    ASSERT_FLOAT_EQ(2.0f, rtrait.ItemMin(1));
    ASSERT_FLOAT_EQ(3.0f, rtrait.ItemMin(2));
    ASSERT_FLOAT_EQ(7.0f, rtrait.ItemMax(0));
    ASSERT_FLOAT_EQ(8.0f, rtrait.ItemMax(1));
    ASSERT_FLOAT_EQ(9.0f, rtrait.ItemMax(2));
}

TEST_F(RatingTraitTest, ComputeAverageAndStdRatingOK) {
    RatingTrait rtrait;
    rtrait.Init(CreateRatingList());
    ASSERT_FLOAT_EQ(5.0f, rtrait.Average());
    ASSERT_FLOAT_EQ(sqrt(60.0/9), rtrait.Std());
}

TEST_F(RatingTraitTest, ComputePopularityAverageAndStdRatingEveryUserOK) {
    RatingTrait rtrait;
    rtrait.Init(CreateRatingList());
    ASSERT_FLOAT_EQ(3.0f, rtrait.UserActivity(0));
    ASSERT_FLOAT_EQ(3.0f, rtrait.UserActivity(1));
    ASSERT_FLOAT_EQ(3.0f, rtrait.UserActivity(2));
    ASSERT_FLOAT_EQ(2.0f, rtrait.UserAverage(0));
    ASSERT_FLOAT_EQ(5.0f, rtrait.UserAverage(1));
    ASSERT_FLOAT_EQ(8.0f, rtrait.UserAverage(2));
    ASSERT_TRUE(Math::Abs(sqrt(2.0f/3) - rtrait.UserStd(0)) < 1e-5);
    ASSERT_TRUE(Math::Abs(sqrt(2.0f/3) - rtrait.UserStd(1)) < 1e-5);
    ASSERT_TRUE(Math::Abs(sqrt(2.0f/3) - rtrait.UserStd(2)) < 1e-5);
}

TEST_F(RatingTraitTest, ComputePopularityAverageAndStdRatingEveryItemOK) {
    RatingTrait rtrait;
    rtrait.Init(CreateRatingList());
    ASSERT_FLOAT_EQ(3.0f, rtrait.ItemPopularity(0));
    ASSERT_FLOAT_EQ(3.0f, rtrait.ItemPopularity(1));
    ASSERT_FLOAT_EQ(3.0f, rtrait.ItemPopularity(2));
    ASSERT_FLOAT_EQ(4.0f, rtrait.ItemAverage(0));
    ASSERT_FLOAT_EQ(5.0f, rtrait.ItemAverage(1));
    ASSERT_FLOAT_EQ(6.0f, rtrait.ItemAverage(2));
    ASSERT_TRUE(Math::Abs(sqrt(6.0f) - rtrait.ItemStd(0)) < 1e-5);
    ASSERT_TRUE(Math::Abs(sqrt(6.0f) - rtrait.ItemStd(1)) < 1e-5);
    ASSERT_TRUE(Math::Abs(sqrt(6.0f) - rtrait.ItemStd(2)) < 1e-5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
