/*
 * implict_convert_longan.cpp
 * Created on: Feb 27, 2015
 * Author: chenguangyu
 */

#include "implict_convert.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(explictRatingTextFilepath, "./explict_rating.txt", "input text file of explict rating data.");
DEFINE_int32(negativeToPositiveRatio, 5, "negative to positive sample ratio");
DEFINE_string(implictRatingTextFilepath, "./implict_rating.txt", "output text file of implict rating data.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::ImplictConvert implictConvert(FLAGS_explictRatingTextFilepath,
            FLAGS_negativeToPositiveRatio, FLAGS_implictRatingTextFilepath);
    implictConvert.Convert();
    return 0;
}
