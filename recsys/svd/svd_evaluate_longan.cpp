/*
 * svd_evaluate_longan.cpp
 * Created on: Dec 12, 2014
 * Author: chenguangyu
 */

#include "svd_evaluate.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTrainFilepath, "./rating_train.bin", "input file path of train rating data.");
DEFINE_string(configFilepath, "./svd_config.json", "input file path of config file.");
DEFINE_string(modelFilepath, "./pop_model.dat", "input file path of trained model.");
DEFINE_string(ratingTestFilepath, "./rating_test.bin", "input file path of test rating data.");
DEFINE_string(resultFilepath, "./pop_result.json", "output file path of result report.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::SVDEvaluate svdEvaluate(
        FLAGS_ratingTrainFilepath, FLAGS_configFilepath,
        FLAGS_modelFilepath, FLAGS_ratingTestFilepath,
        FLAGS_resultFilepath);
    svdEvaluate.Evaluate();
    return 0;
}
