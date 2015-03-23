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
        
def run_cf_auto_encoder_implict():
    run_command("../../util/implict_convert_longan "
              + "-explictRatingTextFilepath=./MovieLens/rating_train.txt " 
              + "-negativeToPositiveRatio=1 "
              + "-implictRatingTextFilepath=./MovieLens/implict_rating_train.txt")
    run_command("../../util/implict_convert_longan "
              + "-explictRatingTextFilepath=./MovieLens/rating_test.txt "
              + "-negativeToPositiveRatio=0 "
              + "-implictRatingTextFilepath=./MovieLens/implict_rating_test.txt")
    
    run_command("../../neural_network/cf_auto_encoder_convert_longan "
              + "-ratingTextFilepath=./MovieLens/implict_rating_train.txt "
              + "-needNormalize=false "
              + "-ratingBinaryFilepath=./MovieLens/implict_rating_train.bin")
    run_command("../../neural_network/cf_auto_encoder_convert_longan "
              + "-ratingTextFilepath=./MovieLens/implict_rating_test.txt "
              + "-needNormalize=false "
              + "-ratingBinaryFilepath=./MovieLens/implict_rating_test.bin")
       
    run_command("../../neural_network/cf_auto_encoder_train_longan "
            + "-ratingTrainFilepath=./MovieLens/implict_rating_train.bin "
            + "-ratingValidateFilepath=./MovieLens/implict_rating_test.bin "
            + "-configFilepath=./cf_auto_encoder_implict_config.json "
            + "-modelFilepath=./cf_auto_encoder_implict_model.dat") 
          
    run_command("../../neural_network/cf_auto_encoder_evaluate_longan " 
              + "-ratingTrainFilepath=./MovieLens/implict_rating_train.bin "
              + "-configFilepath=./cf_auto_encoder_implict_config.json "
              + "-modelFilepath=./cf_auto_encoder_implict_model.dat "
              + "-ratingTestFilepath=./MovieLens/implict_rating_test.bin " 
              + "-resultFilepath=./cf_auto_encoder_implict_result.json")

if __name__=="__main__":
    run_cf_auto_encoder_implict()
