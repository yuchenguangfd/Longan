/*
 * json_config_helper_unittest.cpp
 * Created on: Feb 6, 2015
 * Author: chenguangyu
 */

#include "json_config_helper.h"
#include "common/system/file_util.h"
#include <json/json.h>
#include <gtest/gtest.h>

using namespace longan;

TEST(JsonConfigHelperTest, LoadAndWriteOK) {
    Json::Value config;
    config["field1"] = "value1";
    config["field2"] = 42;
    config["field3"] = true;
    JsonConfigHelper::WriteToFile("tmp.json", config);
    JsonConfigHelper::LoadFromFile("tmp.json", config);
    FileUtil::Delete("tmp.json");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

