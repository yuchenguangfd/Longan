/*
 * rating_list_loader_unittest.cpp
 * Created on: Oct 14, 2014
 * Author: chenguangyu
 */

#include "rating_list_loader.h"
#include <gtest/gtest.h>

using namespace longan;
using namespace std;

TEST(RatingListLoaderTest, Load_MovieLens_Dataset) {
    RatingListLoader loader;
    loader.Load("../project/movielens/MovieLens/rating_test.txt");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
