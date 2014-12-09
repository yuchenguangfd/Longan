#! /usr/bin/python
#-*- coding: utf-8 -*-

import os
import sys

def run_command(cmd):
    print "--run command:", cmd
    exit_status = os.system(cmd)
    if (exit_status != 0):
        print "command failed."
        sys.exit(-1)
        
def run_user_based():
    run_command("../../neighbor/user_based_train_longan "
            + "-ratingTrainFilepath=./MovieLens/rating_train.txt "
            + "-configFilepath=./user_based_config.json "
            + "-modelFilepath=./user_based_model.dat") 
        
    run_command("../../neighbor/user_based_evaluate_longan " 
              + "-ratingTrainFilepath=./MovieLens/rating_train.txt "
              + "-configFilepath=./user_based_config.json "
              + "-modelFilepath=./user_based_model.dat "
              + "-ratingTestFilepath=./MovieLens/rating_test.txt " 
              + "-resultFilepath=./user_based_result.json")

if __name__=="__main__":
    run_user_based()
