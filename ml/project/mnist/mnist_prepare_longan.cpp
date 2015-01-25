/*
 * mnist_prepare_longan.cpp
 * Created on: Jan 24, 2015
 * Author: chenguangyu
 */

#include "mnist_prepare.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(inputDirpath, "./Mnist", "home path of MNIST dataset");
DEFINE_string(configFilepath, "./prepare_config.json", "config file of prepare");
DEFINE_string(outputDirpath, "./Mnist", "output path of converted data");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::MnistPrepare prepare(FLAGS_inputDirpath, FLAGS_configFilepath, FLAGS_outputDirpath);
    prepare.Prepare();
    return 0;
}
