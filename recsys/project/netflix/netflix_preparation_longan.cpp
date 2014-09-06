/*
 * netflix_preparation_longan.cpp
 * Created on: Aug 5, 2014
 * Author: chenguangyu
 */

#include "netflix_preparation.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(inputPath, "./Netflix", "home path of netflix dataset");
DEFINE_string(outputPath, "./Netflix", "output path of converted data");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::NetflixPreparation preparation(FLAGS_inputPath, FLAGS_outputPath);
    preparation.PrepareDataset();
    return 0;
}
