#!/bin/bash
#The basename of the experiment
BASENAME=$1

#The random seed to start from
BASESEED=$2

#The number of runs (with different seeds)
RUNCOUNT=$3

CONTIKI=$4

NUM_NODES=(002 005 010 015 020 025 050 100 200)


for node in "${NUM_NODES[@]}"; do
	python3 ./../tools/project_h_parser.py $((10#$node))
	mkdir "logs/${node}nodes"
	cd "logs/${node}nodes"
	
	#Runs second shellscript			
	./../../auto_scenario_sim.sh $CONTIKI $BASENAME $BASESEED $RUNCOUNT $node	

	cd -
	cd logs
	#Get stats for excel
    python3 ./../../tools/recover_hops_avg.py "${node}nodes"

	cd -


done


cd logs/
zip -r results_nonstoring.zip 002nodes 005nodes 010nodes 015nodes 020nodes 025nodes 050nodes
rm *nodes -r 
cd -

exit 0
