/*
 * demo_glog.cpp
 * Created on: Jul 27, 2014
 * Author: chenguangyu
 */

#include <iostream>
#include <glog/logging.h>

using namespace std;

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    LOG(INFO) << "info log";
    LOG(WARNING) << "warning log";
    LOG(ERROR) << "error log";
    LOG(FATAL) << "fatal error occured";
    return 0;
}
