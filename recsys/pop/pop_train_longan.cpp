/*
 * pop_train_longan.cpp
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#include "pop_train.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTrainFilepath, "./rating_train.bin", "input filepath of rating data file for training.");
DEFINE_string(configFilepath, "./pop_config.json", "input filepath of config file");
DEFINE_string(modelFilepath, "./pop_model.dat", "output filepath of model file.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::PopTrain popTrain(FLAGS_ratingTrainFilepath, FLAGS_configFilepath, FLAGS_modelFilepath);
    popTrain.Train();
    return 0;
}
