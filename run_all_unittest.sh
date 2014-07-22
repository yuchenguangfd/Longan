#!/bin/bash
set -o nounset
set -o errexit

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

cd sort


#######all test passed##
echo "Congratulations! All test passed."
