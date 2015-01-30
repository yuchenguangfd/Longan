/*
 * json_config_helper.h
 * Created on: Jan 27, 2015
 * Author: chenguangyu
 */

#ifndef COMMON_CONFIG_JSON_CONFIG_HELPER_H
#define COMMON_CONFIG_JSON_CONFIG_HELPER_H

#include <json/json.h>
#include <string>

namespace longan {

namespace JsonConfigHelper {

void LoadFromFile(const std::string& filename, Json::Value& mConfig);

} //~ namespace JsonConfigHelper

} //~ namespace longan

#endif /* COMMON_CONFIG_JSON_CONFIG_HELPER_H */
