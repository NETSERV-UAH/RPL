#include "contiki.h"
#include "contiki-net.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "sys/node-id.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-ds6-nbr.h"
#include "net/ipv6/uip.h"
#include "net/nbr-table.h"
#include "sys/rtimer.h"
#include "sys/log.h"
//It's necessary to include rpl.h(rpl_get_parent_rank()), RPL_MIN_HOPRANKINC , rpl_rank_t 
#include "net/routing/rpl-classic/rpl-private.h"
#include "net/routing/rpl-classic/rpl-conf.h"
#include "net/routing/rpl-classic/rpl.h"
//It's necessary to include net_debug_lladdr_print() ( Hops to root count )
#include "os/net/net-debug.h"


#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#ifdef SIMULATIO_CONF_NUM_NODES
#define SIMULATIO_NUM_NODES SIMULATIO_CONF_NUM_NODES
#else
#define SIMULATIO_NUM_NODES 10000 //i.e. a big value
#endif

//to enable statistics log
#ifdef LOG_CONF_STATISTIC_DBG
#define LOG_STATISTIC_DBG LOG_CONF_STATISTIC_DBG
#else
#define LOG_STATISTIC_DBG 0
#endif

//To assign a different seed to each mote
#ifdef LOG_CONF_different_seed
#define LOG_different_seed LOG_CONF_different_seed
#else
#define LOG_different_seed 0
#endif

PROCESS(sink_process, "Sink");
AUTOSTART_PROCESSES(&sink_process);
/*---------------------------------------------------------------------------*/
static void print_routing_table(){
  /*  Aux.Vars  */
  uip_ds6_route_t *r;


  printf("M[%d] has %d routes\n",node_id,uip_ds6_route_num_routes());

  for(r = uip_ds6_route_head(); r != NULL; r = uip_ds6_route_next(r)) {
    printf("M[%d]: ",node_id);
    LOG_INFO_6ADDR(&r->ipaddr);
    printf(" through ");
    LOG_INFO_6ADDR(uip_ds6_route_nexthop(r));
    printf("\n");
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sink_process, ev, data)
{
#if LOG_different_seed == 1
	random_init((unsigned short) node_id);
#endif
  static struct etimer periodic_timer;

  PROCESS_BEGIN();

  /* Initialize DAG root */
#if LOG_STATISTIC_DBG == 1
	int num_nodes = SIMULATIO_NUM_NODES;
	printf("Periodic Statistics: Number of nodes: %d\n", num_nodes);
	printf("Periodic Statistics: convergence time started\n");
#endif
  NETSTACK_ROUTING.root_start();

  etimer_set(&periodic_timer, CLOCK_SECOND*30 + node_id);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
  print_routing_table();

  etimer_set(&periodic_timer, CLOCK_SECOND*3);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
  print_routing_table();
  
  etimer_set(&periodic_timer, CLOCK_SECOND*3);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
  print_routing_table();

  PROCESS_END();
}
