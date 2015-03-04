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
        
def run_cf_auto_encoder():
    run_command("../../neural_network/cf_auto_encoder_convert_longan "
              + "-ratingTextFilepath=./MovieLens/rating_train.txt "
              + "-needNormalize=true "
              + "-ratingBinaryFilepath=./MovieLens/rating_train.bin")
    run_command("../../neural_network/cf_auto_encoder_convert_longan "
              + "-ratingTextFilepath=./MovieLens/rating_test.txt "
              + "-needNormalize=true "
              + "-ratingBinaryFilepath=./MovieLens/rating_test.bin")
     
    run_command("../../neural_network/cf_auto_encoder_train_longan "
            + "-ratingTrainFilepath=./MovieLens/rating_train.bin "
            + "-ratingValidateFilepath=./MovieLens/rating_test.bin "
            + "-configFilepath=./cf_auto_encoder_config.json "
            + "-modelFilepath=./cf_auto_encoder_model.dat") 
        
    run_command("../../neural_network/cf_auto_encoder_evaluate_longan " 
              + "-ratingTrainFilepath=./MovieLens/rating_train.bin "
              + "-configFilepath=./cf_auto_encoder_config.json "
              + "-modelFilepath=./cf_auto_encoder_model.dat "
              + "-ratingTestFilepath=./MovieLens/rating_test.bin " 
              + "-resultFilepath=./cf_auto_encoder_result.json")

if __name__=="__main__":
    run_cf_auto_encoder()
