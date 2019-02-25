#! /usr/bin/python
# -*- coding: utf-8 -*-

import os 
import operator
from collections import OrderedDict

def logrpl_parser(dir):
    #Dir, files vars
    result_dir = 'ParsedLogs'
    path = os.getcwd()+'/'+dir
    files = os.listdir(path)
    
    #Create a final dir(already create it)
    os.mkdir(path+'/'+result_dir+'/')


    for file in files:
        if(file != result_dir):
            try:
                
                file_to_parse = open(path+'/'+file, 'r')
                print('\n\nReading the file '+file)
            except:
                print('Error, cannot read the file: '+file)

            try:
                file_parsed = open(path+'/'+result_dir+'/parsed_'+file,'w')
                print('Creating the file parsed_'+file)
            except:
                print('Error, cannot create the file: parsed_'+file)

            try:
                num_motes=motes_count_per_file(file_to_parse)
            except:
                print('Error, cannot read the number of motes in the file: '+ file)
            
            
            try:
                #Data structs

                """
                #Hops to sink
                dic_motes_hops_to_sink = {'1':'0'}
                for i in range(0,num_motes):
                    if(i != 0):
                        dic_motes_hops_to_sink.update({str(i+1):'-'})
                """
                #Num_Routes
                dic_motes_routes = {0:{'Nothing':'here'}}
                for i in range(1,num_motes+1):
                    dic_motes_routes[i]  = {}
                    dic_motes_routes[i].update({'num_routes':'0'})
                
                #Num_Nbr
                dic_motes_nbr = {0:{'Nothing':'here'}}
                for i in range(1,num_motes+1):
                    dic_motes_nbr[i]  = {}
                    dic_motes_nbr[i].update({'num_nbr':'0'})
                

                #Conv Time
                conv_time_data_ms = []

                conv_time_data_ms.append(get_init_convtime(file_to_parse)) #Init value conv_time_data_ms[0]
                conv_time_data_ms.append(get_end_convtime(file_to_parse,num_motes)) #End value  conv_time_data_ms[1]


                #Number of DIO, DAO, DIS
                dic_motes_DDD_messages = {0:{'Nothing':'here'}}
                for i in range(1,num_motes+1):
                    dic_motes_DDD_messages[i]  = {}
                    dic_motes_DDD_messages[i].update({'DIO':'0'})
                    dic_motes_DDD_messages[i].update({'DAO':'0'})
                    dic_motes_DDD_messages[i].update({'DIS':'0'})



                print('Creating data struct in the file: '+file)

                #debug
                """
                for a,b in dic_motes.items():
                    print '\nMota:',a
                    for key in b:
                        print key+':',b[key] """

            except:
                print('Error, cannot create data struct in the file: '+ file)

            """
            try:
                #To collect Hops to sink
                hops_count_to_sink(file_to_parse,dic_motes_hops_to_sink)
                #print str(dic_motes_hops_to_sink)
            except:
                print('Error, cannot read the hops to the sink mote in the file: '+ file)
            """ 

            try:
                #To collect num_routes per mote
                routes_count_per_mote(file_to_parse,dic_motes_routes)
                #print str(dic_motes_routes)
            except:
                print('Error, cannot read the number of routes per mote in the file: '+ file)

            try:
                #To collect num_nbr per mote
                nbr_count_per_mote(file_to_parse,dic_motes_nbr)
                #print str(dic_motes_nbr)
            except:
                print('Error, cannot read the number of neighbors per mote in the file: '+ file)

            try:
                #To collect num_DDD_messages per mote
                ddd_count_per_mote(file_to_parse,dic_motes_DDD_messages)
                #print str(dic_motes_DDD_messages)
            except:
                print('Error, cannot read the number of DDD messages per mote in the file: '+ file)

            try:
                #To write the result
                #Añadir el dicc de hops al sink si queremos parsear esa info
                write_parsed_data(file_parsed,
                                  dic_motes_routes,
                                  dic_motes_nbr,
                                  dic_motes_DDD_messages,
                                  conv_time_data_ms,
                                  file,
                                  num_motes)
                
            except:
                print('Error, cannot write in the file: '+ file)

            file_parsed.close()
            file_to_parse.close()

    

def motes_count_per_file(file):
    motes_count = 0

    for line in file:
        if(line.count('Node ID: ')):
            motes_count+=1

    file.seek(0)  

    return motes_count

def get_init_convtime(file):
    init = 0

    for line in file:
        if(line.count('Init timer value:')):
            data = line.split('\t')  # log time == data[0]
            init = int(data[0])
            

    file.seek(0)      
    return init

def get_end_convtime(file,num_motes):
    end = 0
    """     #GET_END_CONVTIME in function of the last node reachable
    last_entry = num_motes -1
    iterator_count = 0
    list_nodeid = []
    mote_a = 0
    for line in file:
        if(line.count('ConvTime:')):
             
            data1 = line.split('M[')             #Data[1] == mote_id
            data_aux = data1[1].split(']')
            mote_a = int(data_aux[0])
             
            if not(mote_a in list_nodeid): 
                list_nodeid.append(int(data_aux[0]))   
                iterator_count += 1
                if(iterator_count == last_entry):
                    data = line.split('\t')     # log time == data[0]
                    end = int(data[0])
                
    """
    #GET_END_CONVTIME in function of sink_node's num_routes 
    for line in file:
        if(line.count('M[1] Routes [26 max]:' + str(num_motes-1))):
            data = line.split('\t')     # log time == data[0]
            end = int(data[0])
            break
            
    file.seek(0)      
    return end

"""
def hops_count_to_sink(file_to_parse,dic_motes_hops_to_sink):
    hops_to_sink = 0
    mote_a = 0

    for line in file_to_parse:          #line format: ' M[21] Min.Hops to root: <not reachable>\n '
        if(line.count('Min.Hops to root:')):
            data = line.split('M[') #Data[1] == mote_id
            data_aux = data[1].split(']')
            mote_a = int(data_aux[0]) #mote_id

            aux_data_split = data[1].split(':')
            if(aux_data_split[1][0:len(aux_data_split[1])-1] != ' <not reachable>'):
                hops_to_sink = int(aux_data_split[1][0:len(aux_data_split[1])-1]) #hops to sink 

                 #store in dic
                dic_motes_hops_to_sink[str(mote_a)] = str(hops_to_sink)
            

    file_to_parse.seek(0)
"""
def routes_count_per_mote(file_to_parse,dic_motes_routes):
    mote_a=0
    routes_count = 0

    for line in file_to_parse:          #line format: ' M[3] Routes [16 max]:0\n '
        if(line.count('Routes [')):
            data = line.split('M[') #Data[1] == mote_id
            data_aux = data[1].split(']')
            mote_a = int(data_aux[0]) #mote_id

            aux_data_split = data[1].split(':')
            routes_count = int(aux_data_split[1][0:len(aux_data_split[1])-1]) #number of routes

            #store in dic
            dic_motes_routes[mote_a]['num_routes'] = str(routes_count)
            

    file_to_parse.seek(0)

def nbr_count_per_mote(file_to_parse,dic_motes_nbr):
    mote_a=0
    nbr_count = 0

    for line in file_to_parse:          #line format: ' M[21] Neighbors [16 max]:14\n '
        if(line.count('Neighbors [26 max]:') or line.count('Neighbors [16 max]:') ):
            data = line.split('M[') #Data[1] == mote_id
            data_aux = data[1].split(']')
            mote_a = int(data_aux[0]) #mote_id

            aux_data_split = data[1].split(':')
            nbr_count = int(aux_data_split[1][0:len(aux_data_split[1])-1]) #num_nbr 

            #store in dic
            dic_motes_nbr[mote_a]['num_nbr'] = str(nbr_count)
            

    file_to_parse.seek(0)

def ddd_count_per_mote(file_to_parse,dic_motes_DDD_messages):
    mote_a=0
    dio = 0
    dao = 0
    dis = 0 
    end_time = get_end_convtime(file_to_parse,motes_count_per_file(file_to_parse))
    #Nota a david del futuro cambiar el orden de los logs para sacar antes el num de DDD que el convtime
    
    for line in file_to_parse:          #line format: ' M[5] | DIS:0 | DIO:8 | DAO:7\n '
        dt_time = line.split('\t')  # log time == dt_time[0]
        curr_log_time = int(dt_time[0])
        if(curr_log_time <= end_time):
            if(line.count('| DIS:')):
                data = line.split('M[') #Data[1] == mote_id
                data_aux = data[1].split(']')
                mote_a = int(data_aux[0]) #mote_id

                aux_data_split = data[1].split(':')
                data_aux1 = aux_data_split[1].split('|')
                data_aux2 = aux_data_split[2].split('|')


                dis= int(data_aux1[0][0:len(data_aux1[0])-1])
                dio= int(data_aux2[0][0:len(data_aux2[0])-1])
                dao= int(aux_data_split[3][0:len(aux_data_split[3])-1])

                #store in dic
                dic_motes_DDD_messages[mote_a]['DIS'] = str(dis)
                dic_motes_DDD_messages[mote_a]['DIO'] = str(dio)
                dic_motes_DDD_messages[mote_a]['DAO'] = str(dao)
            

    file_to_parse.seek(0)

def write_parsed_data(file_parsed,
                      dic_motes_routes,dic_motes_nbr,
                      dic_motes_DDD_messages,
                      conv_time_data_ms,
                      file,
                      num_motes):
    #Nota, añadir tambien el dicc de hops al sink si queremos parsear tambien esa info
    #Var aux
    avg = [0.0,0.0,0.0,0.0,0.0,0.0,0.0]

    #Header
    file_parsed.write('File generated by logrpl_parser.py \n')
    file_parsed.write('Original file name: '+ file + '\n')
    file_parsed.write('Number of motes: '+str(num_motes) +'\n')
    file_parsed.write('-------------------------------------------------------------------------------\n')
    
    #Conv Time
    file_parsed.write('convergence_time_start:  ' + str(float(conv_time_data_ms[0])/1000) + ' (s)\t\t' + str((float(conv_time_data_ms[0])/1000)/60) + ' (min)\n')
    file_parsed.write('convergence_time_end:    ' + str(float(conv_time_data_ms[1])/1000) + ' (s)\t\t' + str((float(conv_time_data_ms[0])/1000)/60) + ' (min)\n')
    file_parsed.write('convergence_time:        ' + str(float(conv_time_data_ms[1] - conv_time_data_ms[0])/1000) + ' (s)\t\t' + str((float(conv_time_data_ms[1]-conv_time_data_ms[0])/1000)/60) + ' (min)\n')
    
    file_parsed.write('-------------------------------------------------------------------------------')
    file_parsed.write('\n\nnode_id | Number_of_neighbours | Number_of_routes | Number_of_table_entries | nºDIO | nºDAO | nºDIS | Total DDD messages\n')
    for i in range(1,num_motes+1):
        if((int(dic_motes_nbr[i]['num_nbr']) + int(dic_motes_routes[i]['num_routes'])) <= 9):
            file_parsed.write(str(i) + '\t\t\t\t\t\t\t'
                          #"""+ str(dic_motes_hops_to_sink[str(i)]) + '\t\t\t\t\t\t'"""
                          + str(dic_motes_nbr[i]['num_nbr']) + '\t\t\t\t\t'
                          + str(dic_motes_routes[i]['num_routes']) + '\t\t\t\t\t\t '
                          + str(int(dic_motes_nbr[i]['num_nbr']) + int(dic_motes_routes[i]['num_routes'])) + '\t\t'
                          + str(dic_motes_DDD_messages[i]['DIO']) + '\t\t'
                          + str(dic_motes_DDD_messages[i]['DAO']) + '\t\t'
                          + str(dic_motes_DDD_messages[i]['DIS']) + '\t\t\t'
                          + str(int(dic_motes_DDD_messages[i]['DIS']) + int(dic_motes_DDD_messages[i]['DAO']) + int(dic_motes_DDD_messages[i]['DIO'])) + '\n'                   
            )
        else:
            file_parsed.write(str(i) + '\t\t\t\t\t\t\t'
                          #"""+ str(dic_motes_hops_to_sink[str(i)]) + '\t\t\t\t\t\t'"""
                          + str(dic_motes_nbr[i]['num_nbr']) + '\t\t\t\t\t'
                          + str(dic_motes_routes[i]['num_routes']) + '\t\t\t\t\t\t '
                          + str(int(dic_motes_nbr[i]['num_nbr']) + int(dic_motes_routes[i]['num_routes'])) + '\t\t'
                          + str(dic_motes_DDD_messages[i]['DIO']) + '\t\t'
                          + str(dic_motes_DDD_messages[i]['DAO']) + '\t\t'
                          + str(dic_motes_DDD_messages[i]['DIS']) + '\t\t\t'
                          + str(int(dic_motes_DDD_messages[i]['DIS']) + int(dic_motes_DDD_messages[i]['DAO']) + int(dic_motes_DDD_messages[i]['DIO'])) + '\n'                   
            )
    
        avg[0] += int(dic_motes_nbr[i]['num_nbr'])
        avg[1] += int(dic_motes_routes[i]['num_routes'])
        avg[2] += int(int(dic_motes_nbr[i]['num_nbr']) + int(dic_motes_routes[i]['num_routes']))
        avg[3] += int(dic_motes_DDD_messages[i]['DIO'])
        avg[4] += int(dic_motes_DDD_messages[i]['DAO'])
        avg[5] += int(dic_motes_DDD_messages[i]['DIS'])
        avg[6] += int(int(dic_motes_DDD_messages[i]['DIS']) + int(dic_motes_DDD_messages[i]['DAO']) + int(dic_motes_DDD_messages[i]['DIO']))
    file_parsed.write('------------------------------------------------------------------------------------------------------------------------------\n')
    if((avg[0]/num_motes) < 10):
        file_parsed.write('Average:' + '\t\t\t\t\t' + 
                            str(float(avg[0]/num_motes)) + '\t\t\t\t' +
                            str(float(avg[1]/num_motes)) + '\t\t\t\t\t' +
                            str(float(avg[2]/num_motes)) + '\t' +
                            str(float(avg[3]/num_motes)) + '\t' +
                            str(float(avg[4]/num_motes)) + '\t\t' +
                            str(float(avg[5]/num_motes)) + '\t\t\t' +
                            str(float(avg[6]/num_motes)) + '\n'
        )
    else:
        file_parsed.write('Average:' + '\t\t\t\t\t' + 
                            str(float(avg[0]/num_motes)) + '\t\t\t\t' +
                            str(float(avg[1]/num_motes)) + '\t\t\t\t\t\t' +
                            str(float(avg[2]/num_motes)) + '\t' +
                            str(float(avg[3]/num_motes)) + '\t\t' +
                            str(float(avg[4]/num_motes)) + '\t' +
                            str(float(avg[5]/num_motes)) + '\t\t\t' +
                            str(float(avg[6]/num_motes)) + '\n'
        )
    
if __name__ == '__main__':
    dir_logs = raw_input('Name of logs dir: ')
    if(dir_logs == ''):
        dir_logs= 'logs'
    
    logrpl_parser(dir_logs)

    print('\nDone it!')
