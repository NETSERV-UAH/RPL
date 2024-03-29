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
#include "./examples/RPLHopsCount/Storing/test-RPLStoring.h"
//It's necessary to include net_debug_lladdr_print() ( Hops to root count )
#include "os/net/net-debug.h"
#include "os/services/deployment/deployment.h"


#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO
#define NODE_NOT_REACHABLE -1


#define RPL_CONF_OF_OCP 0
#define RPL_CONF_WITH_STORING 1



#define UDP_CLIENT_PORT	7000
#define UDP_SERVER_PORT	5000

//Global Vars.
static struct simple_udp_connection udp_conn;
static struct simple_udp_connection udp_conn2;
static int hops = NODE_NOT_REACHABLE;
const struct id_mac custom_array[] = {
      { 1, {{0x00,0x12,0x74,0x1,0x0,0x1,0x1,0x1}}},
      { 2, {{0x00,0x12,0x74,0x2,0x0,0x2,0x2,0x2}}},
      { 3, {{0x00,0x12,0x74,0x3,0x0,0x3,0x3,0x3}}},
      { 4, {{0x00,0x12,0x74,0x4,0x0,0x4,0x4,0x4}}},
      { 5, {{0x00,0x12,0x74,0x5,0x0,0x5,0x5,0x5}}},
      { 6, {{0x00,0x12,0x74,0x6,0x0,0x6,0x6,0x6}}},
      { 7, {{0x00,0x12,0x74,0x7,0x0,0x7,0x7,0x7}}},
      { 8, {{0x00,0x12,0x74,0x8,0x0,0x8,0x8,0x8}}},
      { 9, {{0x00,0x12,0x74,0x9,0x0,0x9,0x9,0x9}}},
      { 10, {{0x00,0x12,0x74,0xa,0x0,0xa,0xa,0xa}}},
      { 11, {{0x00,0x12,0x74,0xb,0x0,0xb,0xb,0xb}}},
      { 12, {{0x00,0x12,0x74,0xc,0x0,0xc,0xc,0xc}}},
      { 13, {{0x00,0x12,0x74,0xd,0x0,0xd,0xd,0xd}}},
      { 14, {{0x00,0x12,0x74,0xe,0x0,0xe,0xe,0xe}}},
      { 15, {{0x00,0x12,0x74,0xf,0x0,0xf,0xf,0xf}}},
      { 16, {{0x00,0x12,0x74,0x10,0x0,0x10,0x10,0x10}}},
      { 17, {{0x00,0x12,0x74,0x11,0x0,0x11,0x11,0x11}}},
      { 18, {{0x00,0x12,0x74,0x12,0x0,0x12,0x12,0x12}}},
      { 19, {{0x00,0x12,0x74,0x13,0x0,0x13,0x13,0x13}}},
      { 20, {{0x00,0x12,0x74,0x14,0x0,0x14,0x14,0x14}}},
      { 21, {{0x00,0x12,0x74,0x15,0x0,0x15,0x15,0x15}}},
      { 22, {{0x00,0x12,0x74,0x16,0x0,0x16,0x16,0x16}}},
      { 23, {{0x00,0x12,0x74,0x17,0x0,0x17,0x17,0x17}}},
      { 24, {{0x00,0x12,0x74,0x18,0x0,0x18,0x18,0x18}}},
      { 25, {{0x00,0x12,0x74,0x19,0x0,0x19,0x19,0x19}}}
      
    };

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

  hops = uip_ds6_if.cur_hop_limit - UIP_IP_BUF->ttl +1;
  LOG_INFO("M[%d]-M[%d]:%d",node_id,deployment_id_from_iid(sender_addr),hops);

  simple_udp_sendto(&udp_conn, data, datalen, sender_addr);
  
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sink_process, ev, data)
{
  
  PROCESS_BEGIN();
  deployment_init();


  /* Initialize DAG root */
  NETSTACK_ROUTING.root_start();
  
  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,UDP_CLIENT_PORT, udp_rx_callback);
  simple_udp_register(&udp_conn2, UDP_CLIENT_PORT, NULL,UDP_SERVER_PORT, udp_rx_callback);
 
  PROCESS_END();
}
