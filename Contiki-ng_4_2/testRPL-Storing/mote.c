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
#include "./examples/test-RPLStoring/test-RPLStoring.h"
//It's necessary to include net_debug_lladdr_print() ( Hops to root count )
#include "os/net/net-debug.h"

//It's necessary to log our program
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

//RPL conf
#define RPL_CONF_OF_OCP 0
#define RPL_CONF_WITH_STORING 1
#define NODE_NOT_REACHABLE -1
//UDP conf 
#define UDP_CLIENT_PORT	7000
#define UDP_SERVER_PORT	5000
#define WITH_SERVER_REPLY  1

//SENDING time conf
#define START_INTERVAL		(15 * CLOCK_SECOND)
#define SEND_INTERVAL		  (60 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;
static int hops = NODE_NOT_REACHABLE;

 
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
  }else{
    hops = -1;
  }
  

}

static void print_resul(rtimer_clock_t conv_time){
  extern uip_ds6_netif_t uip_ds6_if;
  uip_ds6_nbr_t *nbr;
  uip_ds6_route_t *r;
  int i,count_addr=0,count_nbr=0,count_route=0;

 
   /* ---- Main ---- */

  for (i = 0; i < UIP_DS6_ADDR_NB; i++) {
      if (uip_ds6_if.addr_list[i].isused) {
        count_addr++;
        }
  }
  LOG_INFO("M[%d] Addresses [%u max]:%d\n",node_id, UIP_DS6_ADDR_NB,count_addr);
  
  if(nbr_table_is_registered(nbr_routes)){
      for(nbr = uip_ds6_nbr_head(); nbr != NULL; nbr = uip_ds6_nbr_next(nbr)) {

        switch (nbr->state) {
            case NBR_INCOMPLETE:
              //printf(" INCOMPLETE");
              break;
            case NBR_REACHABLE:
              //printf(" REACHABLE");
              count_nbr++;
              break;
            case NBR_STALE:
              //printf(" STALE");
              break;
            case NBR_DELAY:
              //printf(" DELAY");
              break;
            case NBR_PROBE:
              //printf(" PROBE");
              break;
        }
       
      }
      LOG_INFO("M[%d] Neighbors [%u max]:%d\n",node_id,NBR_TABLE_MAX_NEIGHBORS,count_nbr);
    
      for(r = uip_ds6_route_head();r != NULL; r = uip_ds6_route_next(r)) {
       
        count_route++;
      }
      
      LOG_INFO("M[%d] Routes [%u max]:%d\n",node_id,UIP_DS6_ROUTE_NB,count_route);
      

      /*Hops count to DODAG root */
      // OUR ipv6 &uip_ds6_if.addr_list[2].ipaddr
      // OUR Mac  uip_ds6_set_lladdr_from_iid(uip_lladdr_t *lladdr, const uip_ipaddr_t *ipaddr);
      //rpl_print_neighbor_list(); rpl-dag.c
     
      //aux_rank = default_instance->current_dag->rank;
      //hops_to_root = aux_rank/RPL_MIN_HOPRANKINC;
      if(hops == NODE_NOT_REACHABLE){
        LOG_INFO("M[%d] Min.Hops to root: <not reachable>\n",node_id);
      }else{
        LOG_INFO("M[%d] Min.Hops to root:%d\n",node_id,hops);
      }
      
      /* DIS,DIO,DAO counter */
      LOG_INFO("M[%d] | DIS:%d | DIO:%d | DAO:%d \n",node_id,dis_storing_rpl,dio_storing_rpl,dao_storing_rpl);
      
      /*Conv time for each mote */
      LOG_INFO("M[%d] ConvTime:%u us\n",node_id,(conv_time*64));
  }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mote_process, ev, data)
{
  static struct etimer periodic_timer;
  static unsigned count;
  static char str[32];
  uip_ipaddr_t dest_ipaddr;
  static rtimer_clock_t conv_time,end_count,start_count;
  bool firstTime= true;

  PROCESS_BEGIN();


  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,UDP_SERVER_PORT, udp_rx_callback);

  //etimer_set(&periodic_timer, CLOCK_SECOND /*random_rand() % SEND_INTERVAL*/);

  while(1) {
    

    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
      if(firstTime){
        end_count=RTIMER_NOW();
        conv_time=end_count-start_count;
      }
      
      /*Show resul RPL routing*/
      print_resul(conv_time);


      snprintf(str, sizeof(str), "hello %d", count);
      simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
      count++;
    
    } else {
      //LOG_INFO("Not reachable yet\n");
    }

    /* Add some jitter */
    etimer_set(&periodic_timer, 5*CLOCK_SECOND/*SEND_INTERVAL - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND))*/);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
