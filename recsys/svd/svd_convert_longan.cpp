/*
 * svd_convert_longan.cpp
 * Created on: Dec 11, 2014
 * Author: chenguangyu
 */

#include "svd_convert.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTextFilepath, "./rating.txt", "input file path of text rating data.");
DEFINE_string(ratingBinaryFilepath, "./rating.bin", "output file path of converted binary rating data.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::SVDConvert svdConvert(FLAGS_ratingTextFilepath, FLAGS_ratingBinaryFilepath);
    svdConvert.Convert();
    return 0;
}
