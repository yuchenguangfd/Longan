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
        
def run_item_based():
    run_command("../../neighbor/item_based_train_longan "
            + "-ratingTrainFilepath=./MovieLens/rating_train.txt "
            + "-configFilepath=./item_based_config.json "
            + "-modelFilepath=./item_based_model.dat") 
        
    run_command("../../neighbor/item_based_evaluate_longan " 
              + "-ratingTrainFilepath=./MovieLens/rating_train.txt "
              + "-configFilepath=./item_based_config.json "
              + "-modelFilepath=./item_based_model.dat "
              + "-ratingTestFilepath=./MovieLens/rating_test.txt " 
              + "-resultFilepath=./item_based_result.json")

if __name__=="__main__":
    run_item_based()
