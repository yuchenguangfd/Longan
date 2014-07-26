/*
 * file_lister_unittest.cpp
 *
 *  Created on: Jul 7, 2014
 *      Author: chenguangyu
 */

#include "file_lister.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace longan;
using namespace std;

TEST(FileListerTest, ListFilename) {
    FileLister fileLister(".");
    vector<std::string> filenames = fileLister.ListFilename();
    auto pos = find(filenames.begin(), filenames.end(), "file_lister_unittest.cpp");
    EXPECT_NE(pos, filenames.end());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
