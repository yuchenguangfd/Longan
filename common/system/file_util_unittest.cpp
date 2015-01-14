/*
 * file_util_unittest.cpp
 * Created on: Sep 20, 2014
 * Author: chenguangyu
 */

#include "file_util.h"
#include "common/error.h"
#include <gtest/gtest.h>
#include <cstdio>

using namespace longan;

TEST(FileUtilTest, CopyOK) {
	std::string src = "file_util.h";
	std::string dst = "file_util.h.copy";
	FileUtil::Copy(src, dst);
	ASSERT_TRUE(FileUtil::IsContentSame(src, dst));
	ASSERT_NO_THROW(FileUtil::Delete(dst));
}

TEST(FileUtilTest, LoadSaveFileContentOK) {
    std::string content;
    ASSERT_NO_THROW(content = FileUtil::LoadFileContent("file_util_unittest.cpp"));
    ASSERT_NO_THROW(FileUtil::SaveFileContent("temp.txt", content));
    std::string content2;
    ASSERT_NO_THROW(content2 = FileUtil::LoadFileContent("temp.txt"));
    ASSERT_EQ(content, content2);
    ASSERT_NO_THROW(FileUtil::Delete("temp.txt"));
}

TEST(FileUtil, LineWordCountOK) {
	std::string tmpFile = "tmp.txt";

	std::string content1 = "This string is used to test worldcount function.";
	ASSERT_NO_THROW(FileUtil::SaveFileContent(tmpFile, content1));
    int cnt = FileUtil::WordCount(tmpFile);
    ASSERT_EQ(8, cnt);

    std::string content2 = "This\nstring\nis\nused\nto\ntest\nlinecount\nfunction.";
    ASSERT_NO_THROW(FileUtil::SaveFileContent(tmpFile, content2));
    cnt = FileUtil::LineCount(tmpFile);
	ASSERT_EQ(8, cnt);
	ASSERT_NO_THROW(FileUtil::Delete(tmpFile));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
