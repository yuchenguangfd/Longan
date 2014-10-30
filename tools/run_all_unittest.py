#! /usr/bin/python
#-*- coding: utf-8 -*-

import os
import sys

num_test = 0

def run_unittest(file):
    rtn = os.system("./"+file)
    if (rtn != 0):
        print "test failed, stop."
        sys.exit(-1)
        
def run_unittest_under(dir):
    for (path, dir, files) in os.walk(dir):
        os.chdir(path)
        for file in files:
            if (file.endswith("_unittest")):
                run_unittest(file)
                global num_test
                num_test += 1

def run_all_unittest(dir_list):
    for dir in dir_list:
        dir = os.path.abspath(dir)
        run_unittest_under(dir)

if __name__ == "__main__":
    if len(sys.argv) == 1:
        dir_list = ["./"]
    else:
        dir_list = sys.argv[1:]
    run_all_unittest(dir_list)
    print ("All %d tests have passed, Congratulations!")%(num_test)
