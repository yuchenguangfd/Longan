#! /usr/bin/python
#-*- coding: utf-8 -*-

import os
import sys

CODE_FILE_EXTS = [".h", ".cpp", ".java", ".py", ".js", ".css", ".html"]

def count_line(file):
    fp = open(file)
    count = 0
    for line in fp.readlines():
        count += 1
    fp.close()
    return count
        
def count_code_lines_under(dir):
    num_line = 0
    num_file = 0
    for (path, dir, files) in os.walk(dir):
        if (path.find("external/") >= 0): continue
        for file in files:
            ext = os.path.splitext(file)[1]
            if ext in CODE_FILE_EXTS:
                num_line += count_line(os.sep.join([path, file]))
                num_file += 1
    return (num_line, num_file)

def count_code_lines(dir_list):
    num_line = 0
    num_file = 0
    for dir in dir_list:
        (n, m) = count_code_lines_under(dir)
        num_line += n
        num_file += m
    return (num_line, num_file)

if __name__=="__main__":
    if len(sys.argv) == 1:
        dir_list = ["./"]
    else:
        dir_list = sys.argv[1:]
    (num_line, num_file) = count_code_lines(dir_list)  
    print ("%d lines in %d files.")%(num_line, num_file)
