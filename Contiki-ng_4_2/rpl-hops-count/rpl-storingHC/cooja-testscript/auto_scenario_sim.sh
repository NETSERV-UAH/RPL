#!/bin/bash

#Contiki-ng Dir
CONTIKI=$1

#Base name .scriptlog
BASE_NAME=$2

#Start seed
SEED=$3

#Number of Ok run
NUM_SIM=$4

#Number motes string
NUM_MOTES_STR=$5

#Scenario list
SCENARIOS=(01) #(01 02 03 04 05 06 07 08 09 10)


#To select each scenario in a simulaion
for j in "${SCENARIOS[@]}"; do
	mkdir "scenario$NUM_MOTES_STR-$j"
	cd "scenario$NUM_MOTES_STR-$j"
	./../../../simexec_out_share.sh ~/contiki-ng/examples/rpl-storingHC/cooja-testscript/simulation/${NUM_MOTES_STR}nodes/Scenario$NUM_MOTES_STR-$j.csc $CONTIKI ~/contiki-ng/examples/rpl-storingHC/cooja-testscript/logs/${NUM_MOTES_STR}nodes/scenario${NUM_MOTES_STR}-${j}/$BASE_NAME $SEED $NUM_SIM
	cd ..
	python3 ./../../../tools/hopscount_parser.py -s "scenario$NUM_MOTES_STR-$j"
done
