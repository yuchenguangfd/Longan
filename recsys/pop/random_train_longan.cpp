/*
 * random_train_unittest.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "random_train.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTrainFilepath, "./rating_train.bin", "input filepath of rating data file for training.");
DEFINE_string(configFilepath, "./random_config.json", "input filepath of config file");
DEFINE_string(modelFilepath, "./random_model.dat", "output filepath of model file.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::RandomTrain randomTrain(FLAGS_ratingTrainFilepath, FLAGS_configFilepath, FLAGS_modelFilepath);
    randomTrain.Train();
    return 0;
}
