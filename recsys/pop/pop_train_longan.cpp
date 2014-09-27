/*
 * pop_train_longan.cpp
 * Created on: Sep 19, 2014
 * Author: chenguangyu
 */

#include "pop_train.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTrainFilePath, "./rating_train.txt", "input file path of rating data file for training.");
DEFINE_string(modelFilePath, "./model.dat", "output file path of model file.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::PopTrain popTrain(FLAGS_ratingTrainFilePath, FLAGS_modelFilePath);
    popTrain.Train();
    return 0;
}
