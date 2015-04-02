/*
 * pop_evaluate_longan.cpp
 * Created on: Sep 22, 2014
 * Author: chenguangyu
 */

#include "pop_evaluate.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTrainFilepath, "./rating_train.bin", "input file path of train rating data.");
DEFINE_string(configFilepath, "./pop_config.json", "input file path of config file.");
DEFINE_string(modelFilepath, "./pop_model.dat", "input file path of trained model.");
DEFINE_string(ratingTestFilepath, "./rating_test.bin", "input file path of test rating data.");
DEFINE_string(rankingResultFilepath, "./pop_ranking_result.bin", "output file path of ranking result.");
DEFINE_string(evaluateResultFilepath, "./pop_evaluate_result.json", "output file path of evaluate result.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::PopEvaluate popEvaluate(
            FLAGS_ratingTrainFilepath, FLAGS_configFilepath,
            FLAGS_modelFilepath, FLAGS_ratingTestFilepath,
            FLAGS_rankingResultFilepath, FLAGS_evaluateResultFilepath);
    popEvaluate.Evaluate();
    return 0;
}
