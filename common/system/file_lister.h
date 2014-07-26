/*
 * file_lister.h
 * Created on: Jul 27, 2014
 * Author: chenguangyu
 */

#ifndef COMMON_SYSTEM_FILE_LISTER_H
#define COMMON_SYSTEM_FILE_LISTER_H

#include <string>
#include <vector>

namespace longan {

class FileLister {
public:
	FileLister(const std::string& dir);
	std::vector<std::string> ListFilename();
protected:
	std::string mDir;
};

} //~ namespace longan

#endif /* COMMON_SYSTEM_FILE_LISTER_H */
