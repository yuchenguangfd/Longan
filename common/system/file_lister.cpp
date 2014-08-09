/*
 * file_lister.cpp
 * Created on: Jul 27, 2014
 * Author: chenguangyu
 */

#include "file_lister.h"
#include <boost/filesystem.hpp>

namespace longan {

FileLister::FileLister(const std::string& dir):
    mDir(dir) {
}

std::vector<std::string> FileLister::ListFilename() {
    using namespace boost::filesystem;
    using namespace std;
    vector<string> filenames;
    for (directory_iterator iter(mDir), end; iter != end; ++iter) {
        path p = (*iter);
        filenames.push_back(p.filename().generic_string());
    }
    return filenames;
}

} //~ namespace longan
