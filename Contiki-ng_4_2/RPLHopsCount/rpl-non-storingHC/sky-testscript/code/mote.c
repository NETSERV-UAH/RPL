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
//It's necessary to include net_debug_lladdr_print() ( Hops to root count )
#include "net/net-debug.h"


//It's necessary to log our program
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

//RPL conf
#define NODE_NOT_REACHABLE -1


//UDP conf 
#define UDP_CLIENT_PORT	7000
#define UDP_SERVER_PORT	5000

//SENDING time conf
#define SEND_INTERVAL		  (60 * CLOCK_SECOND)

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


static struct simple_udp_connection udp_conn;
static int hops = NODE_NOT_REACHABLE;
static bool shouldSend=true;

 
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

    /*  I/O hops */
    hops = uip_ds6_if.cur_hop_limit - UIP_IP_BUF->ttl +1;
    LOG_INFO("M[%d]-M[1]:%d\n",node_id,hops);

    /*  Finish  */
    shouldSend = false;

  }else{
    hops = -1;
  }

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mote_process, ev, data)
{
#if LOG_different_seed == 1
	random_init((unsigned short) node_id);
#endif
  /*  Var.aux */
  static struct etimer periodic_timer; /* timer to wait until rpl conv  */
  static char str[32]; /* msg to sink node */
  uip_ipaddr_t dest_ipaddr; /*  ipaddr sink mote  */


  /*  Bombardeo de sesiones UDP */
  PROCESS_BEGIN();

  /*  30sec until RPL conv  */
  etimer_set(&periodic_timer, CLOCK_SECOND * 30);
  
  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,UDP_SERVER_PORT, udp_rx_callback);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer) );
    /*  Check if this mote is reachable and get sink's ip */
    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr) ) { 
      if(shouldSend){
        #if LOG_CONF_STATISTIC_DBG == 1
          LOG_INFO("Mota[%d]: TX\n",node_id);
        #endif   
        snprintf(str, sizeof(str), "hello sink\n"); /*  Make msg */
        simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr); /*  Send it to sink mote  */
      }
      
    } else {
      #if LOG_CONF_STATISTIC_DBG == 1
        LOG_INFO("Not reachable yet\n");
      #endif   
    }

    /* Add some jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
