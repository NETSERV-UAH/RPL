#include "./../../os/contiki.h"
#include "os/sys/rtimer.h"



extern rtimer_clock_t start_count;

/*Var aux to count DIS,DIO,DAO*/
extern int dis_storing_rpl;
extern int dio_storing_rpl;
extern int dao_storing_rpl;