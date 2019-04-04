#! /usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys  

def recover_hops_avg(base_name_dir_node):
    #Dir, files, and aux vars
    hops_avg_total_list = []
    path = os.getcwd()+'/'+base_name_dir_node
    dir_scenario = os.listdir(path)

    for dirs in dir_scenario:
        path_result = os.getcwd()+'/'+base_name_dir_node+'/'+dirs+'/Parsed_data'
        files = os.listdir(path_result)
        for file in files:

            try:
                file_to_read=open(path_result+'/'+file,'r')
            except:
                print('Error: Cannot read the file: '+'/'+dirs+'/Parsed_data/'+file)

            try:
                getHopsAvg(file_to_read,hops_avg_total_list)
            except:
                print('Error: Cannot recover the hops avg in the file: '+'/'+dirs+'/Parsed_data/'+file)

    #Create file to export to excel 

    try :
        file_to_write = open(path + '/hopsAvg_data_to_excel.txt','w')

        for avg_data in hops_avg_total_list:
            file_to_write.write('{:.6f}\n'.format(avg_data))



    except:
        print('Error: cannot create the file: '+'/'+dirs+'/hopsAvg_data_to_excel.txt')


def getHopsAvg(file_to_read,hops_avg_total_list):

    for lines in file_to_read:
        if lines.count('|Total Hops avg all with all: '):
            hops_avg_total_list.append(float(lines.split(': ')[1][:len(lines.split(': ')[1]) - 1]))





#Main
if __name__ == '__main__':

    #Check args
    if(len(sys.argv) != 2):
        print('Error: usage: ' + sys.argv[0] + ' <dir_node>\n')
        sys.exit(0)

    # sys.argv[2] == 'log05' for example
    base_name_dir_node = sys.argv[1]

    #Main function of the parser
    try:
        recover_hops_avg(base_name_dir_node)
        print('\nRecover status: success')
    except:
        print('\nRecover status: fail')
