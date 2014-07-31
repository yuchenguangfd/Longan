#!/bin/bash
set -o nounset
set -o errexit

scons -u -j8
#######/common##########
cd common

cd math
./math_unittest
./statistic_unittest
cd ../system
./file_lister_unittest
cd ../time
./stopwatch_unittest
cd ../util
./random_unittest
./string_helper_unittest
cd ../..

#######/algorithm#######
cd algorithm
cd number_theory
./number_theory_unittest
cd ../sort
./bubble_sort_unittest
./insertion_sort_unittest
./merge_sort_unittest
./counting_sort_unittest
./quick_sort_unittest
./quick_sort_mt_unittest
./heap_sort_unittest
./shell_sort_unittest
#######all test passed##
echo "Congratulations! All test passed."
