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
    run_command("../../pop/pop_convert_longan "
            + "-ratingTextFilepath=./Netflix/rating_train.txt "
            + "-ratingBinaryFilepath=./Netflix/rating_train.bin")
    run_command("../../pop/pop_convert_longan "
            + "-ratingTextFilepath=./Netflix/rating_test.txt "
            + "-ratingBinaryFilepath=./Netflix/rating_test.bin")
     
    run_command("../../pop/pop_train_longan "
              + "-ratingTrainFilepath=./Netflix/rating_train.bin "
              + "-configFilepath=./pop_config.json "
              + "-modelFilepath=./pop_model.dat") 
        
    run_command("../../pop/pop_evaluate_longan " 
              + "-ratingTrainFilepath=./Netflix/rating_train.bin "
              + "-configFilepath=./pop_config.json "
              + "-modelFilepath=./pop_model.dat "
              + "-ratingTestFilepath=./Netflix/rating_test.bin "
              + "-rankingResultFilepath=./pop_ranking_result.bin "
              + "-evaluateResultFilepath=./pop_evaluate_result.json")

if __name__=="__main__":
    run_pop()
