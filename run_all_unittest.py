#! /usr/bin/python
#-*- coding: utf-8 -*-

import os
import sys

PROJECT_ROOT = os.path.abspath("./")

def run_unittest(dir, file):
    os.chdir(dir)
    rtn = os.system("./"+file)
    if (rtn != 0):
        print "unittest failed, stop."
        sys.exit(-1)
    os.chdir(PROJECT_ROOT)
        
def run_all_unittest():
    for (root, dir, files) in os.walk(PROJECT_ROOT):
        for file in files:
            if (file.endswith("_unittest")):
                run_unittest(root, file)

            
if __name__=="__main__":
    run_all_unittest()
    
# echo "Congratulations! All test passed."
