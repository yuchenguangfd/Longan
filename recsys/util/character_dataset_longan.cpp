/*
 * character_dataset_longan.cpp
 * Created on: Mar 11, 2015
 * Author: chenguangyu
 */

#include "character_dataset.h"
#include <gflags/gflags.h>
#include <glog/logging.h>

DEFINE_string(ratingTextFilepath, "./rating.txt", "input text file of rating data.");
DEFINE_string(resultFilepath, "./character_result.json", "output file path of result report.");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = true;
    longan::CharacterDataset characterDataset(FLAGS_ratingTextFilepath, FLAGS_resultFilepath);
    characterDataset.Character();
    return 0;
}
