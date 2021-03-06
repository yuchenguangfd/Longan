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
        
def run_item_based_implict():
    run_command("../../util/implict_convert_longan "
              + "-explictRatingTextFilepath=./MovieLens/rating_train.txt " 
              + "-negativeToPositiveRatio=0 "
              + "-implictRatingTextFilepath=./MovieLens/implict_rating_train.txt")
    run_command("../../util/implict_convert_longan "
              + "-explictRatingTextFilepath=./MovieLens/rating_test.txt "
              + "-negativeToPositiveRatio=0 "
              + "-implictRatingTextFilepath=./MovieLens/implict_rating_test.txt")
      
    run_command("../../neighbor/item_based_convert_longan "
              + "-ratingTextFilepath=./MovieLens/implict_rating_train.txt "
              + "-ratingBinaryFilepath=./MovieLens/implict_rating_train.bin")
    run_command("../../neighbor/item_based_convert_longan "
              + "-ratingTextFilepath=./MovieLens/implict_rating_test.txt "
              + "-ratingBinaryFilepath=./MovieLens/implict_rating_test.bin")
    
    run_command("../../neighbor/item_based_train_longan "
            + "-ratingTrainFilepath=./MovieLens/implict_rating_train.bin "
            + "-configFilepath=./item_based_implict_config.json "
            + "-modelFilepath=./item_based_implict_model.dat") 
        
    run_command("../../neighbor/item_based_evaluate_longan " 
              + "-ratingTrainFilepath=./MovieLens/implict_rating_train.bin "
              + "-configFilepath=./item_based_implict_config.json "
              + "-modelFilepath=./item_based_implict_model.dat "
              + "-ratingTestFilepath=./MovieLens/implict_rating_test.bin " 
              + "-rankingResultFilepath=./item_based_implict_ranking_result.bin " 
              + "-evaluateResultFilepath=./item_based_implict_evaluate_result.json")

if __name__=="__main__":
    run_item_based_implict()
