#!/bin/bash

#The basename of the experiment
#BASENAME=$1
BASENAME=log

#The random seed to start from
#BASESEED=$2
BASESEED=123456

#The number of runs (with different seeds)
#RUNCOUNT=$3
RUNCOUNT=1

#CONTIKI=$4
CONTIKI=../../../../../..

NUM_NODES=(002) #(002 005 010 015 020 025 050 100 200)


mkdir "logs"

for node in "${NUM_NODES[@]}"; do
	python3 ./../tools/project_h_parser.py $((10#$node))
	cd "logs"
	mkdir "${node}nodes"
	cd "${node}nodes"
	
	#Runs second shellscript			
	./../../auto_scenario_sim.sh $CONTIKI $BASENAME $BASESEED $RUNCOUNT $node	

	cd ..

	#Get stats for excel
    python3 ./../../tools/recover_hops_avg.py "${node}nodes"

	cd ..
done



zip -r results_storing.zip 002nodes 005nodes 010nodes 015nodes 020nodes 025nodes 050nodes 100nodes 200nodes
rm *nodes -r 
cd -

exit 0
