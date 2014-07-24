#!/bin/bash
set -o nounset
set -o errexit

scons -u -j8
#######/common##########
cd common

cd math
./math_unittest
./statistic_unittest

cd ../util
./random_unittest
cd ../..

#######/algorithm#######
cd algorithm
cd number_theory
./number_theory_unittest
cd ../sort
./bubble_sort_unittest
./insertion_sort_unittest
./merge_sort_unittest
./quick_sort_unittest

#######all test passed##
echo "Congratulations! All test passed."
