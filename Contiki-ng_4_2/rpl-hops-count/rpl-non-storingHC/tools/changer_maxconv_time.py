#! /usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os 

def change_num_motes_per_file(file_to_parse, conv_time):

        path = os.getcwd()

        temp_file = open(path+'/'+ 'temp.csc', 'w')

        for line in file_to_parse:
                if(line.count('log.log("Not converged')):
                        line_aux =  '<script>TIMEOUT('+str(conv_time)+', log.log("Not converged\\n"));\n'
                        temp_file.write(line_aux)
                else:
                        temp_file.write(line)
        temp_file.close()







if __name__ == "__main__":

     #Check args
    if(len(sys.argv) != 3):
        print('Error: usage: ' + sys.argv[0] + ' <Path to Scenario>  <MAX_CONV_TIME>\n')
        sys.exit(0)

    conv_time = int(sys.argv[2])
    scenario = sys.argv[1]
    path = os.getcwd()

    try:
                
        file_to_parse = open(path+'/'+ scenario, 'r')

    except:
        print('Error, cannot read the file: '+scenario)

    try:
        change_num_motes_per_file(file_to_parse,conv_time)
        file_to_parse.close()
    except:
        print('Error, cannot change number of motes in the file: '+scenario)

    try:
        os.remove(path+'/'+ scenario)
        os.rename(path+'/'+ 'temp.csc',path+'/'+ scenario)
    except:
        print('Error, cannot remove ' + scenario)
                





