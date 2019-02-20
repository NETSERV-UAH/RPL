#!/bin/bash
#The basename of the experiment
BASENAME=$1

#The random seed to start from
BASESEED=$2

#The number of runs (with different seeds)
RUNCOUNT=$3

python3 project_h_parser.py 2
mkdir logs/log02
./simexec_out_share.sh /home/david/contiki-ng/examples/rpl-non-storingHC/sky-testscript/Scenario02hc.csc /home/david/contiki-ng /home/david/contiki-ng/examples/rpl-non-storingHC/sky-testscript/logs/log02/$BASENAME $BASESEED $RUNCOUNT
cd logs/log02
#.././a.out $BASENAME $BASESEED $RUNCOUNT
cd -

python3 project_h_parser.py 5
mkdir logs/log05
./simexec_out_share.sh /home/david/contiki-ng/examples/rpl-non-storingHC/sky-testscript/Scenario05hc.csc /home/david/contiki-ng /home/david/contiki-ng/examples/rpl-non-storingHC/sky-testscript/logs/log05/$BASENAME $BASESEED $RUNCOUNT
cd logs/log05
#.././a.out $BASENAME $BASESEED $RUNCOUNT
cd -

python3 project_h_parser.py 10
mkdir logs/log10
./simexec_out_share.sh /home/david/contiki-ng/examples/rpl-non-storingHC/sky-testscript/Scenario10hc.csc /home/david/contiki-ng /home/david/contiki-ng/examples/rpl-non-storingHC/sky-testscript/logs/log10/$BASENAME $BASESEED $RUNCOUNT
cd logs/log10
#.././a.out $BASENAME $BASESEED $RUNCOUNT
cd -

python3 project_h_parser.py 15
mkdir logs/log15
./simexec_out_share.sh /home/david/contiki-ng/examples/rpl-non-storingHC/sky-testscript/Scenario15hc.csc /home/david/contiki-ng /home/david/contiki-ng/examples/rpl-non-storingHC/sky-testscript/logs/log15/$BASENAME $BASESEED $RUNCOUNT
cd logs/log15
#.././a.out $BASENAME $BASESEED $RUNCOUNT
cd -

python3 project_h_parser.py 20
mkdir logs/log20
./simexec_out_share.sh /home/david/contiki-ng/examples/rpl-non-storingHC/sky-testscript/Scenario20hc.csc /home/david/contiki-ng /home/david/contiki-ng/examples/rpl-non-storingHC/sky-testscript/logs/log20/$BASENAME $BASESEED $RUNCOUNT
cd logs/log20
#.././a.out $BASENAME $BASESEED $RUNCOUNT
cd -

python3 project_h_parser.py 25
mkdir logs/log25
./simexec_out_share.sh /home/david/contiki-ng/examples/rpl-non-storingHC/sky-testscript/Scenario25hc.csc /home/david/contiki-ng /home/david/contiki-ng/examples/rpl-non-storingHC/sky-testscript/logs/log25/$BASENAME $BASESEED $RUNCOUNT
cd logs/log25
#.././a.out $BASENAME $BASESEED $RUNCOUNT
cd -

cd logs/
zip -r results_nonstoring_$BASENAME_$BASESEED_$RUNCOUNT.zip log02 log05 log10 log15 log20 log25
rm log?? -r 
cd -

exit 0