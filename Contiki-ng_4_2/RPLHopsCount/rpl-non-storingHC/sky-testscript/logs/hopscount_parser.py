#! /usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys  
import operator
import argparse
from collections import OrderedDict

def hopscount_parser(base_name):
    #Dir, files vars
    result_dir = 'Parsed_data'
    path = os.getcwd()+'/'+base_name
    files = os.listdir(path)
    
    #Create a final dir(already create it)
    os.mkdir(path+'/'+result_dir+'/')

    #Param.
    parser = argparse.ArgumentParser()
    parser.add_argument("strings", metavar="<dir>", type=str, help ="Dir to parse")
    parser.add_argument("-ns", "--nonstoring", help="Non Storing Parsing", action="store_true")
    parser.add_argument("-s", "--storing", help="Storing Parsing", action="store_true")
    args = parser.parse_args()

    #log
    try:
        log_parser = open(path +'/.hopscount_parser.log', 'w')
        if args.storing:
            log_parser.write('Mode: Storing activated\n\n')
        elif args.nonstoring:
            log_parser.write('Mode: Non-Storing activated\n\n')
    except:
        print('Error: cannot create the parser logs\n')

    for file in files:
        if(file != result_dir):
            if(file.count('scriptlog')):
                try:
                    
                    file_to_parse = open(path+'/'+file, 'r')
                    log_parser.write('\n\nINFO: Reading the file '+file +'\n')
                except:
                    print('Error, cannot read the file: '+file)

                try:
                    file_parsed = open(path+'/'+result_dir+'/parsed_'+file,'w')
                    log_parser.write('INFO: Creating the file parsed_'+file+'\n')
                except:
                    print('Error, cannot create the file: parsed_'+file)

                try:
                    num_motes=motes_count_per_file(file_to_parse)
                    log_parser.write('INFO: Counting each mote '+'\n')
                except:
                    print('Error, cannot read the number of motes in the file: '+ file)
                try:
                    dic_motes_ip = read_ip_modes(file_to_parse)
                    log_parser.write('INFO: Creating data struct for all IPs in the file:' + file +'\n')

                except:
                    print('Error, cannot read motes IP in the file: '+ file)
                
                try:
                    dic_motes = {0:{'Nothing':'here'}}
                    for i in range(1,num_motes+1):
                        dic_motes[i]  = {}
                        for j in range(1,(num_motes+1)):
                            if(j != i):
                                dic_motes[i].update({str(j):'-'})

                    log_parser.write('INFO: Creating data struct for all hops in the file: '+file+'\n')

                except:
                    print('Error, cannot create data struct in the file: '+ file)

                try:
                    hops_count_per_file(file_to_parse,dic_motes)

                    #We are going to complete all posible paths
                    if args.nonstoring:
                        sim_conv = complete_hops_allWall_ns(file_to_parse,dic_motes,dic_motes_ip, log_parser)
                    
                    log_parser.write('INFO: reading the hops per mote in the file:' + file +'\n')
                    print(dic_motes)
                except:
                    print('Error, cannot read the hops per mote in the file: '+ file)

                try:
                    write_parsed_data(file_parsed,dic_motes,file,num_motes)
                    log_parser.write('INFO: writing in the file: '+ file+'\n')
                except:
                    print('Error, cannot write in the file: '+ file)
                
                file_parsed.close()
                file_to_parse.close()

    

def motes_count_per_file(file):
    aux_id = []

    # line == '31804806 ID:1 [INFO: Main      ] Node ID: 1'
    # aux_line[1] == '1 [INFO: Main      ] Node '
    # aux_line2[0]== '1 ' --> '12 '
    # aux_line2[0][0: len(aux_line2[0]) - 1] == 'ID'
    for line in file:
        if(line.count('Node ID: ')):                                            
            aux_line = line.split('ID: ')                                      
            aux_line2 = aux_line[1].split('[')
                                             
            if(not(int(aux_line2[0][0: len(aux_line2[0]) - 1]) in aux_id)):        
                aux_id.append(int(aux_line2[0][0: len(aux_line2[0]) - 1]))

    file.seek(0)  

    return len(aux_id)

def read_ip_modes(file_to_parse):
    dic = {0 : 'Nothing here'}


 
    for line in file_to_parse:
    # '563087 ID:4 [INFO: Main      ] Tentative link-local IPv6 address: fe80::212:7404:4:404'                  
        if(line.count('Tentative link-local IPv6 address:')):

            aux_line = line.split('ID:')                               
            aux_line2 = aux_line[1].split('[')

            aux_str_ip = line.split('address:')


            dic[int(aux_line2[0][0: len(aux_line2[0]) - 1])]=aux_str_ip[1][1:len(aux_str_ip[1]) -1]

    file_to_parse.seek(0)
    return dic


def hops_count_per_file(file_to_parse,dic_motes):
    mote_a = 0
    mote_b = 0
    hops_a_to_b = 0

    for line in file_to_parse:
        if(line.count('M[')):
            data = line.split('M[') #Data[1] and data[2] 

            mote_a = int(data[1][0:len(data[1])-2])
            aux_data_split = data[2].split(':')
            
            mote_b = int(aux_data_split[0][0:len(aux_data_split[0])-1])
            hops_a_to_b = int(aux_data_split[1][0:len(aux_data_split[1])-1])
            
            dic_motes[mote_a][str(mote_b)]= str(hops_a_to_b)
            dic_motes[mote_b][str(mote_a)]= str(hops_a_to_b)

    file_to_parse.seek(0)

def complete_hops_allWall_ns(file_to_parse,dic_motes, dic_ip, log_parser):
    bool_sim_conv = True

    #Initial check for hops to sink
    for i in range(2, motes_count_per_file(file_to_parse) + 1):
        if dic_motes[1][str(i)] == '-':
            bool_sim_conv = use_ip_hops_check(file_to_parse,dic_motes, dic_ip, str(i))
            if  not bool_sim_conv:
                log_parser.write('Error: Not converged \n')
                break
    

    #Complete all posible paths 
    for i in range(2, motes_count_per_file(file_to_parse) + 1):
        for j in range(2, motes_count_per_file(file_to_parse) + 1):
            dic_motes[i][str(j)] = str(int(dic_motes[1][str(i)]) + int(dic_motes[1][str(j)]))




def use_ip_hops_check(file_to_parse,dic_motes, dic_ip, key):
    bool_sim_conv = False

    for line in file_to_parse:
        if line.count(dic_ip[int(key)]):
            aux_line = line.split(dic_ip[int(key)])
            aux_line2 = aux_line[1].split(': ')

            dic_motes[1][key] = aux_line2[0: len(aux_line2[1]) - 1]
            bool_sim_conv = True

    file_to_parse.seek(0)

    return bool_sim_conv

def write_parsed_data(file_parsed,dic_motes,file,num_motes):
    file_parsed.write('File generated by hopscount_parser.py \n')
    file_parsed.write('Original file name: '+ file + '\n')
    file_parsed.write('Number of motes: '+str(num_motes) +'\n')
    avg=0.0
    aux_avg=0.0
    num_motes_aux=0

    dic_avg_per_mote = {'0':'-'}
    for i in range(1,num_motes+1):
        dic_avg_per_mote.update({str(i):'0'})

    

    for i in range(1,num_motes+1):
        file_parsed.write('\n\nMote['+str(i)+'] | Dest_Mote_ID | Hops\n')
        
        for key in sorted(dic_motes[i]):
            file_parsed.write('\t\t| \t\t\t'+str(key)+' | \t'+ str(dic_motes[i][key])+'\n')
            if(str(dic_motes[i][key]) != '-'):
                avg += float(dic_motes[i][key])
                num_motes_aux+=1
        
        avg /= float(num_motes_aux)
        file_parsed.write('Hops Avg to all motes: '+ str(avg) +'\n')
        dic_avg_per_mote[str(i)] = str(avg)
        avg=0.0
        num_motes_aux=0
    
    for i in range(1,num_motes+1):
        aux_avg+=float(dic_avg_per_mote[str(i)])

    aux_avg /=  float(num_motes)
    file_parsed.write('\n|Total Hops avg all with all: '+str(aux_avg) +'\n') 
    
if __name__ == '__main__':


    if(len(sys.argv) != 3):
        print('Error: usage: ' + sys.argv[0] + ' -[ns \ s]  <dir_log>\n')
        sys.exit(0)

    base_name = sys.argv[2]
    

    hopscount_parser(base_name)

    print('\nDone it!')
