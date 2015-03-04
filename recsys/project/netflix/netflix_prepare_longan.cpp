/*
 * netflix_prepare_longan.cpp
 * Created on: Aug 5, 2014
 * Author: chenguangyu
 */

#include "netflix_prepare.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(inputDirpath, "./Netflix", "home path of netflixs dataset");
DEFINE_string(configFilepath, "./prepare_config.json", "config file of prepare");
DEFINE_string(outputDirpath, "./Netflix", "output path of converted data");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::NetflixPrepare netflixPrepare(FLAGS_inputDirpath, FLAGS_configFilepath, FLAGS_outputDirpath);
    netflixPrepare.Prepare();
    return 0;
}
