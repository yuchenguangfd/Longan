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
        
def run_random():
    run_command("../../pop/random_convert_longan "
              + "-ratingTextFilepath=./Ocn/rating_train.txt "
              + "-ratingBinaryFilepath=./Ocn/rating_train.bin")
    run_command("../../pop/random_convert_longan "
              + "-ratingTextFilepath=./Ocn/rating_test.txt "
              + "-ratingBinaryFilepath=./Ocn/rating_test.bin")
    
    run_command("../../pop/random_train_longan "
              + "-ratingTrainFilepath=./Ocn/rating_train.bin "
              + "-configFilepath=./random_config.json "
              + "-modelFilepath=./random_model.dat") 
        
    run_command("../../pop/random_evaluate_longan " 
               + "-ratingTrainFilepath=./Ocn/rating_train.bin "
               + "-configFilepath=./random_config.json "
               + "-modelFilepath=./random_model.dat "
               + "-ratingTestFilepath=./Ocn/rating_test.bin " 
               + "-rankingResultFilepath=./random_ranking_result.bin " 
               + "-evaluateResultFilepath=./random_evaluate_result.json")

if __name__=="__main__":
    run_random()
