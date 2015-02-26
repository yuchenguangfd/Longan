/*
 * file_lister.cpp
 * Created on: Jul 27, 2014
 * Author: chenguangyu
 */

#include "file_lister.h"
#include <boost/filesystem.hpp>
#include <algorithm>

namespace longan {

FileLister::FileLister(const std::string& dir):
    mDir(dir) {
}

std::vector<std::string> FileLister::ListFilename() {
    using namespace boost::filesystem;
    std::vector<std::string> filenames;
    for (directory_iterator iter(mDir), end; iter != end; ++iter) {
        path p = (*iter);
        filenames.push_back(p.filename().generic_string());
    }
    std::sort(filenames.begin(), filenames.end());
    return filenames;
}

} //~ namespace longan
