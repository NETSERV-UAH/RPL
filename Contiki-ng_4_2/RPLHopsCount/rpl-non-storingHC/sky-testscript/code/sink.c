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
#include "net/net-debug.h"
#include "./../examples/rpl-non-storing/sky-testscript/code/project-conf.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO


//RPL conf
#define NODE_NOT_REACHABLE -1
#define UDP_CLIENT_PORT	7000
#define UDP_SERVER_PORT	5000

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


//GLobal Vars.
static struct simple_udp_connection udp_conn;
static int hops = NODE_NOT_REACHABLE;
static int sum_motes = 0;
PROCESS(sink_process, "Sink");
AUTOSTART_PROCESSES(&sink_process);
/*---------------------------------------------------------------------------*/

static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  

  /*  I/O hops */
  hops = uip_ds6_if.cur_hop_limit - UIP_IP_BUF->ttl +1;
  LOG_INFO("Ip: ");
  LOG_INFO_6ADDR(sender_addr);
  printf(" : %d\n",hops);
  sum_motes++;
 
  /*  Reply our request */
  simple_udp_sendto(&udp_conn, data, datalen, sender_addr);
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sink_process, ev, data)
{
#if LOG_different_seed == 1
	random_init((unsigned short) node_id);
#endif

  /*  Var.aux */
  static struct etimer reply_timer; /* timer to wait reply rtt */
  static struct etimer periodic_timer; /* timer to wait until rpl conv  */

  PROCESS_BEGIN();

  /*  30sec until RPL conv  */
  etimer_set(&periodic_timer, CLOCK_SECOND * 30);


  /* Initialize DAG root */
#if LOG_STATISTIC_DBG == 1
	int num_nodes = SIMULATIO_NUM_NODES;
	printf("Periodic Statistics: Number of nodes: %d\n", num_nodes);
	printf("Periodic Statistics: convergence time started\n");
#endif

  NETSTACK_ROUTING.root_start();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,UDP_CLIENT_PORT, udp_rx_callback);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer) );
  while(1){
    
    /*  New cond. fin */
    if(sum_motes == SIMULATIO_NUM_NODES -1){
      etimer_set(&reply_timer, CLOCK_SECOND * 5);
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&reply_timer) );
      printf("Periodic Statistics: convergence time ended + hops\n");
    }
  }

  PROCESS_END();
}
