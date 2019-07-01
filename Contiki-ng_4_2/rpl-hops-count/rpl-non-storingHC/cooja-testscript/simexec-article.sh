#!/bin/bash

#Number of Ok run
NUMOKRUN=10 #1

#Contiki-NG directory
CONTIKING=../../../../../..

#Simulation list
SIMULATIONS=(002 005 010 015 020 025 050 100 200) #(002 005 010 015 020 025 050 100 200) #(002 005) #(002) #(005) #(010) #(015) #(020) #(050)

#Scenario list
SCENARIOS=(01 02 03 04 05 06 07 08 09 10) #(01) #(01 02)

cd ~/contiki-ng/examples/rpl-non-storingHC/cooja-testscript
#Compile log parser
gcc -o parser.out ~/contiki-ng/examples/log-parser-rpl-hopcount/non-storing/log-parser-rpl-cooja.c

mkdir logs
cd logs

#To select each simulation
for i in "${SIMULATIONS[@]}"; do
	python3 ./../../tools/project_h_parser.py $i 2	#$i=>number of nodes, 2=> Cooja mote
	mkdir "${i}nodes"
	cd "${i}nodes"
	#To select each scenario in a simulaion
	for j in "${SCENARIOS[@]}"; do
		mkdir "scenario$i-$j"
		cd "scenario$i-$j"
		mkdir "Cooja_logs"
		mkdir "Parsed_data"
		mkdir "Raw_data"
		printf "Working directory: %s\n" `pwd`;
		../../.././simexec3.sh "../../../simulation/${i}nodes/Scenario$i-$j.csc" $CONTIKING log 123456 $NUMOKRUN
		cd ..
	done
	cd ..

done

cd ..
rm parser.out

