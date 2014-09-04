/*
 * movielens_preparation_longan.cpp
 * Created on: Aug 20, 2014
 * Author: chenguangyu
 */

#include "movielens_preparation.h"
#include "common/error.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(inputPath, "./MovieLens", "home path of movielens dataset");
DEFINE_string(outputPath, "./MovieLens", "output path of converted data");
DEFINE_string(datasetType, "100k", "one of 100k, 1m, 10m");
DEFINE_double(trainRatio, 0.9, "ratio of train to all ratings");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    FLAGS_alsologtostderr = true;
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);

    longan::MovielensPreparation preparation(FLAGS_inputPath, FLAGS_outputPath, FLAGS_trainRatio);
    if (FLAGS_datasetType == "100k") {
        preparation.PrepareDataset100K();
    } else if (FLAGS_datasetType == "1m") {
        preparation.PrepareDataset1M();
    } else if (FLAGS_datasetType == "10m") {
    	preparation.PrepareDataset10M();
    } else {
    	LOG(ERROR) << "unrecognized flag: datasetType=" << FLAGS_datasetType; 
    	return longan::LONGAN_FAIL;
    }
    return longan::LONGAN_FAIL;
}
