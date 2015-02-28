/*
 * svd_train_longan.cpp
 * Created on: Dec 13, 2014
 * Author: chenguangyu
 */

#include "svd_train.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTrainFilepath, "./rating_train.bin", "input filepath of rating data file for training.");
DEFINE_string(ratingValidateFilepath, "./rating_validate.bin", "input filepath of rating data file for validating.");
DEFINE_string(configFilepath, "./svd_config.json", "input config filepath");
DEFINE_string(modelFilepath, "./svd_model.dat", "output filepath of model file.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::SVDTrain svdTrain(FLAGS_ratingTrainFilepath, FLAGS_ratingValidateFilepath,
            FLAGS_configFilepath, FLAGS_modelFilepath);
    svdTrain.Train();
    return 0;
}
