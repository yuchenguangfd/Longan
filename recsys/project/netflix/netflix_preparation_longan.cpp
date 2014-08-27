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
DEFINE_bool(prepareMovie, true, "to prepare movie data");
DEFINE_bool(prepareRating, true, "to prepare rating data");

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    longan::NetflixPreparation preparation(FLAGS_inputPath, FLAGS_outputPath);
    if (FLAGS_prepareMovie) {
        preparation.PrepareMovieData();
    }
    if (FLAGS_prepareRating) {
        preparation.PrepareRatingData();
    }
    return 0;
}
