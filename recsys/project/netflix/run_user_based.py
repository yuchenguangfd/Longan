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
#     run_command("../../neighbor/user_based_convert_longan "
#               + "-ratingTextFilepath=./Netflix/rating_train.txt "
#               + "-ratingBinaryFilepath=./Netflix/rating_train.bin")
#     run_command("../../neighbor/user_based_convert_longan "
#               + "-ratingTextFilepath=./Netflix/rating_test.txt "
#               + "-ratingBinaryFilepath=./Netflix/rating_test.bin")
    
    
    run_command("../../neighbor/user_based_train_longan "
            + "-ratingTrainFilepath=./Netflix/rating_train.bin "
            + "-configFilepath=./user_based_config.json "
            + "-modelFilepath=./user_based_model.dat") 
        
    run_command("../../neighbor/user_based_evaluate_longan " 
              + "-ratingTrainFilepath=./Netflix/rating_train.bin "
              + "-configFilepath=./user_based_config.json "
              + "-modelFilepath=./user_based_model.dat "
              + "-ratingTestFilepath=./Netflix/rating_test.bin " 
              + "-resultFilepath=./user_based_result.json")

if __name__=="__main__":
    run_user_based()
