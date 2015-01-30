/*
 * bp_network_train_longan.cpp
 * Created on: Jan 25, 2015
 * Author: chenguangyu
 */

#include "bp_network_train.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(dataTrainFilepath, "./data_train.bin ", "training data file");
DEFINE_string(configFilepath, "./bp_network_config.json", "config file of bp_network");
DEFINE_string(modelFilepath, "./bp_network_model.dat", "output model file");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::BpNetworkTrain train(FLAGS_dataTrainFilepath, FLAGS_configFilepath, FLAGS_modelFilepath);
    train.Train();
    return 0;
}
