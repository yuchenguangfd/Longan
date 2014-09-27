/*
 * pop_evaluate_longan.cpp
 * Created on: Sep 22, 2014
 * Author: chenguangyu
 */

#include "pop_evaluate.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(modelFilePath, "./model.dat", "input file path of trained model.");
DEFINE_string(ratingTrainFilePath, "./rating_train.txt", "input file path of train rating data.");
DEFINE_string(ratingTestFilePath, "./rating_test.txt", "input file path of test rating data.");
DEFINE_string(resultFilePath, "./pop_result.txt", "output file path of result report.");
DEFINE_string(configFilePath, "./pop_evaluate.cfg", "input file path of evaluation config, see pop_evaluate as sample");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;

    longan::PopEvaluate popEvaluate(
            FLAGS_modelFilePath,
            FLAGS_ratingTrainFilePath,
            FLAGS_ratingTestFilePath,
            FLAGS_resultFilePath,
            FLAGS_configFilePath);
    popEvaluate.Evaluate();
    return 0;
}
