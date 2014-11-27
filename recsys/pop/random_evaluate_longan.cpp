/*
 * random_evaluate_longan.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "random_evaluate.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTrainFilepath, "./rating_train.txt", "input file path of rating data for training.");
DEFINE_string(configFilepath, "./random_config.json", "input file path of config file.");
DEFINE_string(modelFilepath, "./random_model.dat", "input file path of trained model.");
DEFINE_string(ratingTestFilepath, "./rating_test.txt", "input file path of rating data for testing.");
DEFINE_string(resultFilepath, "./random_result.txt", "output file path of result report.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::RandomEvaluate randomEvaluate(
            FLAGS_ratingTrainFilepath, FLAGS_configFilepath,
            FLAGS_modelFilepath, FLAGS_ratingTestFilepath,
            FLAGS_resultFilepath);
    randomEvaluate.Evaluate();
    return 0;
}
