/*
 * user_based_train_longan.cpp
 * Created on: Oct 27, 2014
 * Author: chenguangyu
 */

#include "user_based_train.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTrainFilepath, "./rating_train.bin", "input file path of rating data file for training.");
DEFINE_string(configFilepath, "./user_based_config.json", "input config file for training");
DEFINE_string(modelFilepath, "./user_based_model.dat", "output filepath of model file.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::UserBasedTrain userBasedTrain(FLAGS_ratingTrainFilepath,
            FLAGS_configFilepath, FLAGS_modelFilepath);
    userBasedTrain.Train();
    return 0;
}
