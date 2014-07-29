/*
 * string_helper_unittest.cpp
 * Created on: Jul 29, 2014
 * Author: chenguangyu
 */
#include "string_helper.h"
#include <gtest/gtest.h>
#include <vector>
#include <string>

using namespace longan;
using namespace std;

TEST(StringHelperTest, Split) {
    vector<string> substrings = StringHelper::Split("a b cd efg", " ");
    EXPECT_EQ(substrings.size(), 4);
    EXPECT_EQ(substrings[0], "a");
    EXPECT_EQ(substrings[1], "b");
    EXPECT_EQ(substrings[2], "cd");
    EXPECT_EQ(substrings[3], "efg");

    substrings = StringHelper::Split("a<br>b<br><br>c", "<br>");
    EXPECT_EQ(substrings.size(), 3);
    EXPECT_EQ(substrings[0], "a");
    EXPECT_EQ(substrings[1], "b");
    EXPECT_EQ(substrings[2], "c");

    substrings = StringHelper::Split("<br>", "<br>");
    EXPECT_EQ(substrings.size(), 0);

    substrings = StringHelper::Split("", "<br>");
    EXPECT_EQ(substrings.size(), 0);

    substrings = StringHelper::Split("<br>", "");
    EXPECT_EQ(substrings.size(), 1);
    EXPECT_EQ(substrings[0], "<br>");

    substrings = StringHelper::Split("a<br>", "<br>", false);
    EXPECT_EQ(substrings.size(), 2);
    EXPECT_EQ(substrings[0], "a");
    EXPECT_EQ(substrings[1], "");

    substrings = StringHelper::Split("a<br>", "<br>", true);
    EXPECT_EQ(substrings.size(), 1);
    EXPECT_EQ(substrings[0], "a");

    substrings = StringHelper::Split(",,,,", ",", false);
    EXPECT_EQ(substrings.size(), 5);
    for (int i = 0; i < substrings.size(); ++i)
        EXPECT_EQ(substrings[i], "");
}

TEST(StringHelperTest, CountWhitespace) {
    string text = "hello, \rstring.\tohh!\n";
    EXPECT_EQ(4, StringHelper::CountWhitespace(text));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
