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
        
def run_character_dataset():
    run_command("../../util/character_dataset_longan "
              + "-ratingTextFilepath=./Ocn/rating.txt "
              + "-resultFilepath=./character_result.json")

if __name__=="__main__":
    run_character_dataset()