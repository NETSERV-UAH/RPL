#!/bin/bash
#The basename of the experiment
BASENAME=$1

#The random seed to start from
BASESEED=$2

#The number of runs (with different seeds)
RUNCOUNT=$3

python3 project_h_parser.py 2
mkdir logs/log02
./simexec_out_share.sh /home/david/contiki-ng/examples/rpl-storingHC/sky-testscript/Scenario02.csc /home/david/contiki-ng /home/david/contiki-ng/examples/rpl-storing/sky-testscript/logs/log02/$BASENAME $BASESEED $RUNCOUNT
cd logs
#python3 hopscount_parser.py -s log02
cd -

python3 project_h_parser.py 5
mkdir logs/log05
./simexec_out_share.sh /home/david/contiki-ng/examples/rpl-storingHC/sky-testscript/Scenario05.csc /home/david/contiki-ng /home/david/contiki-ng/examples/rpl-storing/sky-testscript/logs/log05/$BASENAME $BASESEED $RUNCOUNT
cd logs
#python3 hopscount_parser.py -s log05
cd -

python3 project_h_parser.py 10
mkdir logs/log10
./simexec_out_share.sh /home/david/contiki-ng/examples/rpl-storingHC/sky-testscript/Scenario10.csc /home/david/contiki-ng /home/david/contiki-ng/examples/rpl-storing/sky-testscript/logs/log10/$BASENAME $BASESEED $RUNCOUNT
cd logs
#python3 hopscount_parser.py -s log10
cd -

python3 project_h_parser.py 15
mkdir logs/log15
./simexec_out_share.sh /home/david/contiki-ng/examples/rpl-storingHC/sky-testscript/Scenario15.csc /home/david/contiki-ng /home/david/contiki-ng/examples/rpl-storing/sky-testscript/logs/log15/$BASENAME $BASESEED $RUNCOUNT
cd logs
#python3 hopscount_parser.py -s log15
cd -

python3 project_h_parser.py 20
mkdir logs/log20
./simexec_out_share.sh /home/david/contiki-ng/examples/rpl-storingHC/sky-testscript/Scenario20.csc /home/david/contiki-ng /home/david/contiki-ng/examples/rpl-storing/sky-testscript/logs/log20/$BASENAME $BASESEED $RUNCOUNT
cd logs
#python3 hopscount_parser.py -s log20
cd -

python3 project_h_parser.py 25
mkdir logs/log25
./simexec_out_share.sh /home/david/contiki-ng/examples/rpl-storingHC/sky-testscript/Scenario25.csc /home/david/contiki-ng /home/david/contiki-ng/examples/rpl-storing/sky-testscript/logs/log25/$BASENAME $BASESEED $RUNCOUNT
cd logs
#python3 hopscount_parser.py -s log25
cd -

cd logs/
zip -r results_storing_$BASENAME-$BASESEED-$RUNCOUNT.zip log02 log05 log10 log15 log20 log25
rm log?? -r 
cd -

exit 0