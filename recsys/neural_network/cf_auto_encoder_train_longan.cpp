/*
 * cf_auto_encoder_train_longan.cpp
 * Created on: Feb 3, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder_train.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTrainFilepath, "./rating_train.bin", "input filepath of rating data file for training.");
DEFINE_string(configFilepath, "./cf_auto_encoder_config.json", "input config filepath");
DEFINE_string(modelFilepath, "./cd_auto_encoder_model.dat", "output filepath of model file.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::CFAutoEncoderTrain cfAutoEncoderTrain(FLAGS_ratingTrainFilepath,
            FLAGS_configFilepath, FLAGS_modelFilepath);
    cfAutoEncoderTrain.Train();
    return 0;
}
