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
//It's necessary to include rtimer_clock_t start_count( Conv time )
#include "test-RPLStoring.h"
//It's necessary to include net_debug_lladdr_print() ( Hops to root count )
#include "net/net-debug.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1

#define RPL_CONF_OF_OCP 0
#define RPL_CONF_WITH_STORING 1



#define UDP_CLIENT_PORT	7000
#define UDP_SERVER_PORT	5000

#ifdef SIMULATIO_CONF_NUM_NODES
#define SIMULATIO_NUM_NODES SIMULATIO_CONF_NUM_NODES
#else
#define SIMULATIO_NUM_NODES 10000 //i.e. a big value
#endif

#ifdef LOG_CONF_STATISTIC_DBG
#define LOG_STATISTIC_DBG LOG_CONF_STATISTIC_DBG
#else
#define LOG_STATISTIC_DBG 0
#endif

//GLobal Vars.
static struct simple_udp_connection udp_conn;

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
	//Nothing, it will be used for hop count
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sink_process, ev, data)
{


  PROCESS_BEGIN();


  /* Initialize DAG root */
#if LOG_STATISTIC_DBG == 1
	int num_nodes = SIMULATIO_NUM_NODES;
	printf("Periodic Statistics: Number of nodes: %d\n", num_nodes);
	printf("Periodic Statistics: convergence time started\n");
#endif
  NETSTACK_ROUTING.root_start();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,UDP_CLIENT_PORT, udp_rx_callback);

 
  PROCESS_END();
}
