/*
 * random_evaluate_longan.cpp
 * Created on: Nov 24, 2014
 * Author: chenguangyu
 */

#include "random_evaluate.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTrainFilepath, "./rating_train.bin", "input file path of rating data for training.");
DEFINE_string(configFilepath, "./random_config.json", "input file path of config file.");
DEFINE_string(modelFilepath, "./random_model.dat", "input file path of trained model.");
DEFINE_string(ratingTestFilepath, "./rating_test.bin", "input file path of rating data for testing.");
DEFINE_string(rankingResultFilepath, "./random_ranking_result.bin", "output file path of ranking result.");
DEFINE_string(evaluateResultFilepath, "./random_evaluate_result.json", "output file path of evaluate result.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::RandomEvaluate randomEvaluate(
            FLAGS_ratingTrainFilepath, FLAGS_configFilepath,
            FLAGS_modelFilepath, FLAGS_ratingTestFilepath,
            FLAGS_rankingResultFilepath, FLAGS_evaluateResultFilepath);
    randomEvaluate.Evaluate();
    return 0;
}
