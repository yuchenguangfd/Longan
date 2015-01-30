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
        
def run_bp_network():
    run_command("../../neural_network/bp_network_train_longan "
            + "-dataTrainFilepath=./Mnist/data_train.bin "
            + "-configFilepath=./bp_network_config.json "
            + "-modelFilepath=./bp_network_model.dat") 
        
    run_command("../../neural_network/bp_network_evaluate_longan " 
              + "-dataTrainFilepath=./Mnist/data_train.bin "
              + "-configFilepath=./bp_network_config.json "
              + "-modelFilepath=./bp_network_model.dat "
              + "-dataTestFilepath=./Mnist/data_test.bin " 
              + "-resultFilepath=./bp_network_result.json")

if __name__=="__main__":
    run_bp_network()
