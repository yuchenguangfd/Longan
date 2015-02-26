#! /usr/bin/python
#-*- coding: utf-8 -*-

import sys
import os

def run_command(cmd):
    print "--run command:", cmd
    exit_status = os.system(cmd)
    if (exit_status != 0):
        print "command failed."
        sys.exit(-1)
        
def run_pop():
    run_command("../../pop/random_convert_longan "
              + "-ratingTextFilepath=./MovieLens/rating_train.txt "
              + "-ratingBinaryFilepath=./MovieLens/rating_train.bin")
    run_command("../../pop/random_convert_longan "
              + "-ratingTextFilepath=./MovieLens/rating_test.txt "
              + "-ratingBinaryFilepath=./MovieLens/rating_test.bin")
  
    run_command("../../pop/random_train_longan "
              + "-ratingTrainFilepath=./MovieLens/rating_train.bin "
              + "-configFilepath=./random_config.json "
              + "-modelFilepath=./random_model.dat") 
        
    run_command("../../pop/random_evaluate_longan " 
               + "-ratingTrainFilepath=./MovieLens/rating_train.bin "
               + "-configFilepath=./random_config.json "
               + "-modelFilepath=./random_model.dat "
               + "-ratingTestFilepath=./MovieLens/rating_test.bin " 
               + "-resultFilepath=./random_result.json")

if __name__=="__main__":
    run_pop()
