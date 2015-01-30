/*
 * json_config_helper.cpp
 * Created on: Jan 27, 2015
 * Author: chenguangyu
 */

#include "json_config_helper.h"
#include "common/system/file_util.h"
#include "common/error.h"

namespace longan {

namespace JsonConfigHelper {

void LoadFromFile(const std::string& filename, Json::Value& config) {
    std::string content = FileUtil::LoadFileContent(filename);
    Json::Reader reader;
    if (!reader.parse(content, config)) {
        throw LonganFileFormatError();
    }
}

}  //~ namespace JsonConfigHelper

} //~ namespace longan
