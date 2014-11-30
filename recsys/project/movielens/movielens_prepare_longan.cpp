/*
 * movielens_prepare_longan.cpp
 * Created on: Aug 20, 2014
 * Author: chenguangyu
 */

#include "movielens_prepare.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(inputDirpath, "./MovieLens", "home path of movielens dataset");
DEFINE_string(configFilepath, "./prepare_config.json", "config file of prepare");
DEFINE_string(outputDirpath, "./MovieLens", "output path of converted data");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::MovielensPrepare prepare(FLAGS_inputDirpath, FLAGS_configFilepath, FLAGS_outputDirpath);
    prepare.Prepare();
    return 0;
}
