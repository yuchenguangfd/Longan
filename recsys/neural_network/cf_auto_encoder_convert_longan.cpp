/*
 * cf_auto_encoder_convert_longan.cpp
 * Created on: Feb 3, 2015
 * Author: chenguangyu
 */

#include "cf_auto_encoder_convert.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTextFilepath, "./rating.txt", "input file path of text rating data.");
DEFINE_bool(needNormalize, true, "normalize rating into [0, 1].");
DEFINE_string(ratingBinaryFilepath, "./rating.bin", "output file path of converted binary rating data.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::CFAutoEncoderConvert cfAutoEncoderConvert(FLAGS_ratingTextFilepath, FLAGS_needNormalize,
            FLAGS_ratingBinaryFilepath);
    cfAutoEncoderConvert.Convert();
    return 0;
}
