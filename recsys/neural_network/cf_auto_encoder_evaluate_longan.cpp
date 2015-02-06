/*
 * cf_auto_encoder_evaluate_longan.cpp
 * Created on: Feb 4, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder_evaluate.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTrainFilepath, "./rating_train.bin", "input file path of rating data for training.");
DEFINE_string(configFilepath, "./cf_auto_encoder_config.json", "input file path of config file.");
DEFINE_string(modelFilepath, "./cf_auto_encoder_model.dat", "input file path of trained model.");
DEFINE_string(ratingTestFilepath, "./rating_test.bin", "input file path of rating data for testing.");
DEFINE_string(resultFilepath, "./cf_auto_encoder_result.json", "output file path of result report.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::CFAutoEncoderEvaluate cfAutoEncoderEvaluate(
            FLAGS_ratingTrainFilepath, FLAGS_configFilepath,
            FLAGS_modelFilepath, FLAGS_ratingTestFilepath,
            FLAGS_resultFilepath);
    cfAutoEncoderEvaluate.Evaluate();
    return 0;
}
