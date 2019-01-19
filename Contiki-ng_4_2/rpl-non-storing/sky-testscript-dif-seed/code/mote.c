#include "contiki.h"
#include "contiki-net.h"
#include "net/routing/routing.h"
#include "random.h"
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

//It's necessary to log our program
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

//RPL conf
#define RPL_CONF_OF_OCP 0
#define NODE_NOT_REACHABLE -1
//UDP conf 
#define UDP_CLIENT_PORT	7000
#define UDP_SERVER_PORT	5000
#define WITH_SERVER_REPLY  1

//SENDING time conf
#define START_INTERVAL		(15 * CLOCK_SECOND)
#define SEND_INTERVAL		  (60 * CLOCK_SECOND)

#ifdef LOG_CONF_STATISTIC_DBG
#define LOG_STATISTIC_DBG LOG_CONF_STATISTIC_DBG
#else
#define LOG_STATISTIC_DBG 0
#endif

static struct simple_udp_connection udp_conn;
//static int hops = NODE_NOT_REACHABLE;

 
/*---------------------------------------------------------------------------*/
PROCESS(mote_process, "Mote");
AUTOSTART_PROCESSES(&mote_process);
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
PROCESS_THREAD(mote_process, ev, data)
{
	random_init(unsigned short(node_id));

  PROCESS_BEGIN();


  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,UDP_SERVER_PORT, udp_rx_callback);

  //etimer_set(&periodic_timer, CLOCK_SECOND /*random_rand() % SEND_INTERVAL*/);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
