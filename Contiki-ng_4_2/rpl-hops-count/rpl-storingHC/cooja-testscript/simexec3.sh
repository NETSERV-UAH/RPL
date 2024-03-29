#!/bin/bash
# The simulation to run
CSC=$1
shift
#Contiki directory
CONTIKI=$1
shift
#The basename of the experiment
BASENAME=$1
shift
#The random seed to start from
BASESEED=$1
shift
#The number of runs (with different seeds)
RUNCOUNT=$1
shift

# Counts all tests run
declare -i TESTCOUNT=0

# Counts successfull tests
declare -i OKCOUNT=0

# A list of seeds the resulted in failure
FAILSEEDS=

#EXTRA BEGIN
#for (( SEED=$BASESEED; SEED<$(($BASESEED+$RUNCOUNT)); SEED++ )); do
for (( SEED=$BASESEED; OKCOUNT<$RUNCOUNT; SEED++ )); do
#EXTRA END
	echo -n "Running test $BASENAME with random Seed $SEED"

	# run simulation
	java -Xshare:on -jar $CONTIKI/tools/cooja/dist/cooja.jar -nogui=$CSC -contiki=$CONTIKI -random-seed=$SEED > $BASENAME.$SEED.coojalog &
	JPID=$!

	# Copy the log and only print "." if it changed
	touch progress.log
	while kill -0 $JPID 2> /dev/null
	do
		sleep 1
		diff $BASENAME.$SEED.coojalog progress.log > /dev/null
		if [ $? -ne 0 ]
		then
		  echo -n "."
		  cp $BASENAME.$SEED.coojalog progress.log
		fi
	done
	rm progress.log

  # wait for end of simulation
	wait $JPID
	JRV=$?

	# Save testlog
	touch COOJA.testlog;
	mv COOJA.testlog $BASENAME.$SEED.scriptlog
	rm COOJA.log

  TESTCOUNT+=1
	if [ $JRV -eq 0 ] ; then
#EXTRA BEGIN
		#echo "Ok!"
		#OKCOUNT+=1
		../../.././parser.out $BASENAME.$SEED.scriptlog
		RETURN_VAL=$?
		if [ $RETURN_VAL -eq 1 ]
		then
			echo "Test failed; Introduce a Log file!"
			FAILSEEDS+=" $SEED"
		elif [ $RETURN_VAL -eq 2 ]
		then
			echo "Test failed; Log file can't be read!"
			FAILSEEDS+=" $SEED"
		elif [ $RETURN_VAL -eq 3 ]
		then
			echo "Test failed; Parser output log file can't be opened!"
			FAILSEEDS+=" $SEED"
		elif [ $RETURN_VAL -eq 4 ]
		then
			echo "Test failed; not converged!"
			FAILSEEDS+=" $SEED"
		elif [ $RETURN_VAL -eq 5 ]
		then
			echo "Test failed; Hopcount can't be calculated!"
			FAILSEEDS+=" $SEED"
		elif [ $RETURN_VAL -eq 0 ]
		then
			OKCOUNT+=1
			echo "Test ok!"
		fi
		mv output_file_$BASENAME.$SEED.scriptlog ./Parsed_data
#EXTRA END
	else
		#EXTRA BEGIN
		#FAILSEEDS+=" $BASESEED"
		FAILSEEDS+=" $SEED"
		#EXTRA END
		echo " FAIL"
		echo "==== $BASENAME.$SEED.coojalog ====" ; cat $BASENAME.$SEED.coojalog;
		echo "==== $BASENAME.$SEED.scriptlog ====" ; cat $BASENAME.$SEED.scriptlog;
	fi
#EXTRA BEGIN
	mv $BASENAME.$SEED.scriptlog ./Raw_data
	mv $BASENAME.$SEED.coojalog ./Cooja_logs
#EXTRA END
done

if [ $TESTCOUNT -ne $OKCOUNT ] ; then
	# At least one test failed
	# EXTRA BEGIN	
	#printf "%-40s TEST FAIL  %3d/%d -- failed seeds:%s\n" "$BASENAME" "$OKCOUNT" "$TESTCOUNT" "$FAILSEEDS" > $BASENAME.testlog;
	printf "%-40s TEST OK    %3d/%d\n" "$BASENAME" "$OKCOUNT" "$TESTCOUNT" > $BASENAME.testlog;
	printf "%-40s TEST FAIL  -- failed seeds:%s\n" "$BASENAME" "$FAILSEEDS" > $BASENAME.testlog;
	#EXTRA END
else
	printf "%-40s TEST OK    %3d/%d\n" "$BASENAME" "$OKCOUNT" "$TESTCOUNT" > $BASENAME.testlog;
fi

# We do not want Make to stop -> Return 0
# The Makefile will check if a log contains FAIL at the end
exit 0
