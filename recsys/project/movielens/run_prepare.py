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
        
def run_prepare():
    run_command("./movielens_prepare_longan " 
              + "-inputDirpath=./MovieLens "
              + "-configFilepath=./prepare_config.json "
              + "-outputDirpath=./MovieLens")

if __name__=="__main__":
    run_prepare()
