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
    run_command("../../svd/svd_convert_longan "
              + "-ratingTextFilepath=./MovieLens/rating_train.txt "
              + "-ratingBinaryFilepath=./MovieLens/rating_train.bin")
    run_command("../../svd/svd_convert_longan "
              + "-ratingTextFilepath=./MovieLens/rating_test.txt "
              + "-ratingBinaryFilepath=./MovieLens/rating_test.bin")
    
    run_command("../../svd/svd_train_longan "
              + "-ratingTrainFilepath=./MovieLens/rating_train.bin "
              + "-ratingValidateFilepath=./MovieLens/rating_test.bin "
              + "-configFilepath=./svd_config.json "
              + "-modelFilepath=./svd_model.dat")
    run_command("../../svd/svd_evaluate_longan " 
              + "-ratingTrainFilepath=./MovieLens/rating_train.bin "
              + "-configFilepath=./svd_config.json "
              + "-modelFilepath=./svd_model.dat "
              + "-ratingTestFilepath=./MovieLens/rating_test.bin " 
              + "-resultFilepath=./svd_result.json")

if __name__=="__main__":
    run_svd()