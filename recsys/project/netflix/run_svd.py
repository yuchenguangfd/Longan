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
        
def run_svd():
#     run_command("../../svd/svd_convert_longan "
#               + "-ratingTextFilepath=./Netflix/rating_train.txt "
#               + "-ratingBinaryFilepath=./Netflix/rating_train.bin")
#     run_command("../../svd/svd_convert_longan "
#               + "-ratingTextFilepath=./Netflix/rating_test.txt "
#               + "-ratingBinaryFilepath=./Netflix/rating_test.bin")
    
    run_command("../../svd/svd_train_longan "
              + "-ratingTrainFilepath=./Netflix/rating_train.bin "
              + "-configFilepath=./svd_config.json "
              + "-modelFilepath=./svd_model.dat")
    run_command("../../svd/svd_evaluate_longan " 
              + "-ratingTrainFilepath=./Netflix/rating_train.bin "
              + "-configFilepath=./svd_config.json "
              + "-modelFilepath=./svd_model.dat "
              + "-ratingTestFilepath=./Netflix/rating_test.bin " 
              + "-resultFilepath=./svd_result.json")

if __name__=="__main__":
    run_svd()
