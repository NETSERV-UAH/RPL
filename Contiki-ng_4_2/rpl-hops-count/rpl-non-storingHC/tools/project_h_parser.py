#! /usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os 

def change_num_motes_tableenries_per_file(file_to_parse, num, mote_type):

        temp_file = open(path+'/'+ 'temp.h', 'w')

        if(mote_type == 1): #Sky
                max_route_entries = 100;
                max_neighbors = 26;
        elif(mote_type == 2): #Cooja
                max_route_entries = 200;
                max_neighbors = 200;

        for line in file_to_parse:
                if(line.count('#define SIMULATIO_CONF_NUM_NODES')):
                        line_aux =  '#define SIMULATIO_CONF_NUM_NODES\t'+ str(num) + '\n'
                        temp_file.write(line_aux)
                elif(line.count('#define NETSTACK_MAX_ROUTE_ENTRIES')):
                        line_aux =  '#define NETSTACK_MAX_ROUTE_ENTRIES\t'+ str(max_route_entries) + '\n'
                        temp_file.write(line_aux)
                elif(line.count('#define NBR_TABLE_CONF_MAX_NEIGHBORS')):
                        line_aux =  '#define NBR_TABLE_CONF_MAX_NEIGHBORS\t'+ str(max_neighbors) + '\n'
                        temp_file.write(line_aux)
                else:
                        temp_file.write(line)
        temp_file.close()







if __name__ == "__main__":

        num_motes = int(sys.argv[1])
        mote_type = int(sys.argv[2])		#1:SKY, 2:Cooja
        code_dir = 'code'
        file = 'project-conf.h'
        path = os.getcwd()+'/../../sky-testscript/'+code_dir    #To be adapted to both sky-testscript and cooja-testscript simulations

        try:
                
                file_to_parse = open(path+'/'+ file, 'r')
                #print('\n\nRead the file ' + file)
        except:
                print('Error, cannot read the file: '+file)

        try:
                change_num_motes_tableenries_per_file(file_to_parse, num_motes, mote_type)
                file_to_parse.close()
        except:
                print('Error, cannot change number of motes in the file: '+file)

        try:
                os.remove(path+'/'+ file)
                os.rename(path+'/'+ 'temp.h',path+'/'+ file)
        except:
                print('Error, cannot remove' + file)
                





