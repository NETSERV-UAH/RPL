#! /usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os 

def change_num_motes_per_file(file_to_parse, num):

        temp_file = open(path+'/'+ 'temp.h', 'w')

        for line in file_to_parse:
                if(line.count('#define SIMULATIO_CONF_NUM_NODES')):
                        line_aux =  '#define SIMULATIO_CONF_NUM_NODES\t'+ str(num) + '\n'
                        temp_file.write(line_aux)
                else:
                        temp_file.write(line)
        temp_file.close()







if __name__ == "__main__":

        num_motes = int(sys.argv[1])
        code_dir = 'code'
        file = 'project-conf.h'
        path = os.getcwd()+'/'+code_dir

        try:
                
                file_to_parse = open(path+'/'+ file, 'r')
                #print('\n\nRead the file ' + file)
        except:
                print('Error, cannot read the file: '+file)

        try:
                change_num_motes_per_file(file_to_parse,num_motes)
                file_to_parse.close()
        except:
                print('Error, cannot change number of motes in the file: '+file)

        try:
                os.remove(path+'/'+ file)
                os.rename(path+'/'+ 'temp.h',path+'/'+ file)
        except:
                print('Error, cannot remove' + file)
                





