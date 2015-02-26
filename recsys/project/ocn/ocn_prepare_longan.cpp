/*
 * ocn_prepare_longan.cpp
 * Created on: Feb 26, 2015
 * Author: chenguangyu
 */

#include "ocn_prepare.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(inputDirpath, "./Ocn", "home path of Ocn dataset");
DEFINE_string(configFilepath, "./prepare_config.json", "config file of prepare");
DEFINE_string(outputDirpath, "./Ocn", "output path of converted data");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::OcnPrepare ocnPrepare(FLAGS_inputDirpath, FLAGS_configFilepath, FLAGS_outputDirpath);
    ocnPrepare.Prepare();
    return 0;
}
