#! /usr/bin/python
# -*- coding: utf-8 -*-

import shutil
import os 

def deleteSourceCode_Contiki(files_to_change):

    path_a= os.getcwd()+'/../../../os/net/routing/rpl-classic'
    path_b= os.getcwd()+'/../../../os/net/ipv6'

    for i in range(0,2) :
        os.remove(path_a + '/' + files_to_change[i])

    for i in range(2,5):        
        os.remove(path_b + '/' + files_to_change[i])
    

def applyChanges(files_to_change):
    path_a= os.getcwd()+'/../../../os/net/routing/rpl-classic'
    path_b= os.getcwd()+'/../../../os/net/ipv6'
    dir_with_contiki_changes = 'contiki-changes'


    for i in range(0,2) :
        shutil.copy(os.getcwd() +'/'+ dir_with_contiki_changes +'/'+ files_to_change[i], path_a + '/')

    for i in range(2,5) :        
        shutil.copy(os.getcwd() +'/'+ dir_with_contiki_changes +'/'+ files_to_change[i], path_b + '/')



if __name__ == "__main__":

    files_to_change = ["rpl-dag.c","rpl-icmp6.c","uip-ds6-nbr.c","uip-ds6-route.c","uip-sr.c"]

    try:
        deleteSourceCode_Contiki(files_to_change)
    except:
        print('Error, cannot delete contiki source code...')
    try:    
        applyChanges(files_to_change)
    except:
        print('Error, cannot apply changes to contiki source code...')
