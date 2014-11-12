/*
 * item_based_train_longan.cpp
 * Created on: Oct 18, 2014
 * Author: chenguangyu
 */

#include "item_based_train.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTrainFilepath, "./rating_train.txt", "input filepath of rating data file for training.");
DEFINE_string(trainConfigFilepath, "./item_based_train_config.json", "input config filepath for training");
DEFINE_string(modelFilepath, "./item_based_model.dat", "output filepath of model file.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = true;
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    longan::ItemBasedTrain itemBasedTrain(FLAGS_ratingTrainFilepath,
            FLAGS_trainConfigFilepath, FLAGS_modelFilepath);
    itemBasedTrain.Train();
    return 0;
}
