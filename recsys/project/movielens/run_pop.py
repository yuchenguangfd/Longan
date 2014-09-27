#! /usr/bin/python
#-*- coding: utf-8 -*-

import sys
import os

def run_command(cmd):
    print "run command:", cmd
    exit_status = os.system(cmd)
    if (exit_status != 0):
        print "command failed."
        sys.exit(-1)
        
def run_pop():
    run_command("../../pop/pop_train_longan "
              + "-ratingTrainFilePath=./MovieLens/rating_train.txt "
              + "-modelFilePath=./model.dat") 
        
    run_command("../../pop/pop_evaluate_longan " 
              + "-modelFilePath=./model.dat "
              + "-ratingTrainFilePath=./MovieLens/rating_train.txt "
              + "-ratingTestFilePath=./MovieLens/rating_test.txt " 
              + "-resultFilePath=./pop_result.txt "
              + "-configFilePath=./pop_evaluate.cfg")
    run_command("cat ./pop_result.txt")

if __name__=="__main__":
    run_pop()
