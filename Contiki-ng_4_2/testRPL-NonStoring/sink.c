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
#include "os/net/routing/rpl-lite/rpl-types.h"
#include "os/net/routing/rpl-lite/rpl-conf.h"
#include "os/net/routing/rpl-lite/rpl-dag-root.h"
#include "os/net/routing/rpl-lite/rpl.h"
//It's necessary to include rtimer_clock_t start_count( Conv time )
#include "./examples/test-RPLNonStoring/test-RPLNonStoring.h"
//It's necessary to include net_debug_lladdr_print() ( Hops to root count )
#include "os/net/net-debug.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1

#define RPL_CONF_OF_OCP 0
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

  int count_nbr=0;
  
  uip_ds6_nbr_t *nbr;

  LOG_INFO("M[%d] | DIS:%d | DIO:%d | DAO:%d \n",node_id,dis_nonstoring_rpl,dio_nonstoring_rpl,dao_nonstoring_rpl);
 
  if(rpl_dag_root_is_root()) {
    if(uip_sr_num_nodes() > 0) {
     
      LOG_INFO("M[%d] Routes [26 max]:%d\n", node_id,uip_sr_num_nodes() -1 );
      
    } else {
      LOG_INFO("No routing links\n");
    }
  }
  for(nbr = uip_ds6_nbr_head(); nbr != NULL; nbr = uip_ds6_nbr_next(nbr)) { 
      if(nbr->state == NBR_REACHABLE){
          count_nbr++;
      }
  }
  LOG_INFO("M[%d] Neighbors [%u max]:%d\n",node_id,NBR_TABLE_MAX_NEIGHBORS,count_nbr);
#if WITH_SERVER_REPLY
  
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
  rpl_dag_root_start();
  start_count=RTIMER_NOW();
  LOG_INFO("Init timer value: %u\n", start_count);

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,UDP_CLIENT_PORT, udp_rx_callback);

 
  PROCESS_END();
}
