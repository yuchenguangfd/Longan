/*
 * bp_network_evaluate_longan.cpp
 * Created on: Jan 25, 2015
 * Author: chenguangyu
 */

#include "bp_network_evaluate.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(dataTrainFilepath, "./data_train.bin ", "training data file");
DEFINE_string(configFilepath, "./bp_network_config.json", "config file of bp_network");
DEFINE_string(modelFilepath, "./bp_network_model.dat", "output model file");
DEFINE_string(dataTestFilepath, "./data_test.bin", "testing data file");
DEFINE_string(resultFilepath, "/bp_network_result.json", "output result file");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::BpNetworkEvaluate evaluate(FLAGS_dataTrainFilepath, FLAGS_configFilepath, FLAGS_modelFilepath,
            FLAGS_dataTestFilepath, FLAGS_resultFilepath);
    evaluate.Evaluate();
    return 0;
}
