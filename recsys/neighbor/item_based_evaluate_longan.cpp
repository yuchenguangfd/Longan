/*
 * item_based_evaluate_longan.cpp
 * Created on: Nov 12, 2014
 * Author: chenguangyu
 */

#include "item_based_evaluate.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTrainFilepath, "./rating_train.txt", "input file path of rating data for training.");
DEFINE_string(configFilepath, "./item_based_config.json", "input file path of config file.");
DEFINE_string(modelFilepath, "./item_based_model.dat", "input file path of trained model.");
DEFINE_string(ratingTestFilepath, "./rating_test.txt", "input file path of rating data for testing.");
DEFINE_string(resultFilepath, "./item_based_result.txt", "output file path of result report.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::ItemBasedEvaluate itemBasedEvaluate(
            FLAGS_ratingTrainFilepath, FLAGS_configFilepath,
            FLAGS_modelFilepath, FLAGS_ratingTestFilepath,
            FLAGS_resultFilepath);
    itemBasedEvaluate.Evaluate();
    return 0;
}
