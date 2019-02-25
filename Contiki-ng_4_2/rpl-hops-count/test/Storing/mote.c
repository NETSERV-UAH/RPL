#include "contiki.h"
#include "contiki-net.h"
#include "net/routing/routing.h"
#include "random.h"
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

//It's necessary to log our program
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO
#define NUM_MOTES 25
//RPL conf
#define RPL_CONF_OF_OCP 0
#define RPL_CONF_WITH_STORING 1
#define NODE_NOT_REACHABLE -1
//UDP conf 
#define UDP_CLIENT_PORT	7000
#define UDP_SERVER_PORT	5000


//SENDING time conf
#define START_INTERVAL		(15 * CLOCK_SECOND)
#define SEND_INTERVAL		  (60 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;
static struct simple_udp_connection udp_conn2;
static int hops = NODE_NOT_REACHABLE;
static int num_id_rec[NUM_MOTES];
static bool shouldFinish=false;
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
  if(NETSTACK_ROUTING.node_is_reachable()){

    hops = uip_ds6_if.cur_hop_limit - UIP_IP_BUF->ttl +1;
    LOG_INFO("M[%d]-M[%d]:%d\n",node_id,deployment_id_from_iid(sender_addr),hops);
    num_id_rec[deployment_id_from_iid(sender_addr)-1] = 1;
    if(datalen != 1){
      simple_udp_sendto(&udp_conn, data, 1, sender_addr);
    }
  }else{
    hops = -1;
  }

}
/*---------------------------------------------------------------------------*/
static bool sum(int * a){
  int i,aux=0;

  for(i=0 ; i < NUM_MOTES;i++){
    aux+=a[i];
  }
  LOG_INFO("Mote[%d]: %d/24\n", node_id,aux);
  return aux == 24;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mote_process, ev, data)
{
  static struct etimer periodic_timer;
  static char str[32];
  uip_ipaddr_t dest_ipaddr;
  int count = 0,i;
  
  PROCESS_BEGIN();
  
  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,UDP_SERVER_PORT, udp_rx_callback);
  simple_udp_register(&udp_conn2, UDP_SERVER_PORT, NULL,UDP_CLIENT_PORT, udp_rx_callback);
  etimer_set(&periodic_timer, CLOCK_SECOND /*random_rand() % SEND_INTERVAL*/);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer) );

    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr) ) {
      if(!shouldFinish){
        LOG_INFO("Mota[%d]: TX\n",node_id);
        for(i= 0; i < NUM_MOTES; i++){
          if((i+1) != node_id){
            
              deployment_iid_from_id(&dest_ipaddr, (uint16_t )i +1);
              snprintf(str, sizeof(str), "hello %d", count);
              if(!num_id_rec[deployment_id_from_iid(&dest_ipaddr)-1]){
                simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
              }
              count++;
            
         }
       }
      }
    
    
    } else {
      LOG_INFO("Not reachable yet\n");
    }
    shouldFinish = sum(num_id_rec);
    
    /* Add some jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL
      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
