#include "contiki.h"
#include "contiki-net.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include "os/sys/node-id.h"
#include "os/net/ipv6/uip-ds6.h"
#include "os/net/ipv6/uip-ds6-nbr.h"
#include "os/net/ipv6/uip.h"
#include "os/net/nbr-table.h"
#include "os/sys/rtimer.h"
#include "sys/log.h"
//It's necessary to include rpl.h(rpl_get_parent_rank()), RPL_MIN_HOPRANKINC , rpl_rank_t 
#include "os/net/routing/rpl-classic/rpl-private.h"
#include "os/net/routing/rpl-classic/rpl-conf.h"
#include "os/net/routing/rpl-classic/rpl.h"
//It's necessary to include rtimer_clock_t start_count( Conv time )
#include "./examples/test-RPLStoring/test-RPLStoring.h"
//It's necessary to include net_debug_lladdr_print() ( Hops to root count )
#include "os/net/net-debug.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1

#define RPL_CONF_OF_OCP 0
#define RPL_CONF_WITH_STORING 1



#define UDP_CLIENT_PORT	7000
#define UDP_SERVER_PORT	5000

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

  int count_route = 0,count_nbr=0;
  uip_ds6_route_t *r ;
  uip_ds6_nbr_t *nbr;
  LOG_INFO("M[%d] | DIS:%d | DIO:%d | DAO:%d \n",node_id,dis_storing_rpl,dio_storing_rpl,dao_storing_rpl);
  for(r = uip_ds6_route_head();r != NULL; r = uip_ds6_route_next(r)) {
      count_route++;
  }
      
  LOG_INFO("M[%d] Routes [%u max]:%d\n",node_id,UIP_DS6_ROUTE_NB,count_route);
  for(nbr = uip_ds6_nbr_head(); nbr != NULL; nbr = uip_ds6_nbr_next(nbr)) { 
      if(nbr->state == NBR_REACHABLE){
          count_nbr++;
      }
  }
  LOG_INFO("M[%d] Neighbors [%u max]:%d\n",node_id,NBR_TABLE_MAX_NEIGHBORS,count_nbr);
#if WITH_SERVER_REPLY
  /* send back the same string to the client as an echo reply */
  //LOG_INFO("Sending response.\n");
  simple_udp_sendto(&udp_conn, data, datalen, sender_addr);
#endif /* WITH_SERVER_REPLY */
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sink_process, ev, data)
{
  rtimer_clock_t start_count;
  PROCESS_BEGIN();


  /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();
  start_count=RTIMER_NOW();
  LOG_INFO("Init timer value: %u\n", start_count);

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,UDP_CLIENT_PORT, udp_rx_callback);

 
  PROCESS_END();
}
