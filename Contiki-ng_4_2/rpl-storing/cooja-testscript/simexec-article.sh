#!/bin/bash

#Number of Ok run
NUMOKRUN=10

#Contiki-NG directory
CONTIKING=../../../../../..

#Simulation list
SIMULATIONS=(002) #(005) #(010) #(015) #(020) #(050) #(002 005 010 015 020 025 050 100 200)

#Scenario list
SCENARIOS=(01 02 03 04 05 06 07 08 09 10)

cd ~/contiki-ng/examples/rpl-storing/cooja-testscript
mkdir logs
cd logs

#Compile log parser
gcc -o parser.out ../../../log-parser-rpl/log-parser-rpl-script.c

#To select each simulation
for i in "${SIMULATIONS[@]}"; do
	mkdir "${i}nodes"
	cd "${i}nodes"
	#To select each scenario in a simulaion
	for j in "${SCENARIOS[@]}"; do
		mkdir "scenario$i-$j"
		cd "scenario$i-$j"
		printf "Working directory: %s\n" `pwd`;
		../../.././simexec2.sh "../../../simulation/${i}nodes/Scenario$i-$j.csc" $CONTIKING log 123456 $NUMOKRUN
		../.././parser.out log 123456 $NUMOKRUN
		cd ..
	done
	cd ..

done

rm parser.out
