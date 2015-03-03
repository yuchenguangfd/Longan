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
        
def run_pop_implict():
    run_command("../../util/implict_convert_longan "
              + "-explictRatingTextFilepath=./MovieLens/rating_train.txt " 
              + "-negativeToPositiveRatio=0 "
              + "-implictRatingTextFilepath=./MovieLens/implict_rating_train.txt")
    run_command("../../util/implict_convert_longan "
              + "-explictRatingTextFilepath=./MovieLens/rating_test.txt "
              + "-negativeToPositiveRatio=0 "
              + "-implictRatingTextFilepath=./MovieLens/implict_rating_test.txt")
    
    run_command("../../pop/pop_convert_longan "
              + "-ratingTextFilepath=./MovieLens/implict_rating_train.txt "
              + "-ratingBinaryFilepath=./MovieLens/implict_rating_train.bin")
    run_command("../../pop/pop_convert_longan "
              + "-ratingTextFilepath=./MovieLens/implict_rating_test.txt "
              + "-ratingBinaryFilepath=./MovieLens/implict_rating_test.bin")
    
    run_command("../../pop/pop_train_longan "
              + "-ratingTrainFilepath=./MovieLens/implict_rating_train.bin "
              + "-configFilepath=./pop_implict_config.json "
              + "-modelFilepath=./pop_implict_model.dat") 
        
    run_command("../../pop/pop_evaluate_longan " 
              + "-ratingTrainFilepath=./MovieLens/implict_rating_train.bin "
              + "-configFilepath=./pop_implict_config.json "
              + "-modelFilepath=./pop_implict_model.dat "
              + "-ratingTestFilepath=./MovieLens/implict_rating_test.bin " 
              + "-resultFilepath=./pop_implict_result.json")

if __name__=="__main__":
    run_pop_implict()
