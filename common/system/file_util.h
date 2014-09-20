/*
 * file_util.h
 * Created on: Sep 20, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_SYSTEM_FILE_UTIL_H
#define COMMON_SYSTEM_FILE_UTIL_H

#include <string>

namespace longan {

namespace FileUtil {

void Copy(const std::string& srcFilename, const std::string& dstFilename);
void Delete(const std::string& filename);
bool IsContentSame(const std::string& filename1, const std::string& filename2);
std::string LoadFileContent(const std::string& filename);
void SaveFileContent(const std::string& filename, const std::string& content);
int LineCount(const std::string& filename);
int WordCount(const std::string& filename);

};

} //~ namespace longan

#endif // COMMON_SYSTEM_FILE_UTIL_H
