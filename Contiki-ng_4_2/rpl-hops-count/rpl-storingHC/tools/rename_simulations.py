#! /usr/bin/python
# -*- coding: utf-8 -*-

import os 
import sys  



if __name__ == "__main__":

    if len(sys.argv) != 4:
        print('Error: Usage, '+sys.argv[0]+' <dir> <base_name> <base_seed>')
        exit(-1)

    path = os.getcwd()+'/'+sys.argv[1]
    base_name = sys.argv[2]
    base_seed = sys.argv[3]

    files = os.listdir(path)

    for file in files:
        os.rename(path +'/'+file, path+'/'+base_name+'.'+base_seed+'.scriptlog')
        base_seed = str(int(base_seed) + 1)

