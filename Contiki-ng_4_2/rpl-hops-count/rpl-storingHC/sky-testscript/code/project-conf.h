/*
 * Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), and David Carrascal(1);
 *                    (1) GIST, University of Alcala, Spain.
 *                    (2) CEIT, Amirkabir University of Technology (Tehran
 *                        Polytechnic), Iran.
 * Adapted to use IoTorii, a link layer protocol for Low pawer and Lossy Network
 * (LLN), over the IEEE 802.15.4 none beacon mode.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*---------------------------------------------------------------------------*/
#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_
/*---------------------------------------------------------------------------*/
//#define LOG_CONF_LEVEL_IPV6                        LOG_LEVEL_DBG
/*---------------------------------------------------------------------------*/
//#define LOG_CONF_LEVEL_RPL                           LOG_LEVEL_DBG
//#define LOG_CONF_LEVEL_RPL                         LOG_LEVEL_INFO
//#define LOG_CONF_LEVEL_RPL                         LOG_LEVEL_WARN
/*---------------------------------------------------------------------------*/
//#define LOG_CONF_LEVEL_IPV6                          LOG_LEVEL_DBG
/*---------------------------------------------------------------------------*/
//#define LOG_CONF_LEVEL_6LOWPAN                     LOG_LEVEL_DBG
/*---------------------------------------------------------------------------*/
//#define LOG_CONF_LEVEL_TCPIP                       LOG_LEVEL_DBG
/*---------------------------------------------------------------------------*/
//#define LOG_CONF_LEVEL_MAC                         LOG_LEVEL_DBG
//#define LOG_CONF_LEVEL_MAC                         LOG_LEVEL_INFO
/*---------------------------------------------------------------------------*/
//#define LOG_CONF_LEVEL_FRAMER                      LOG_LEVEL_DBG
/*---------------------------------------------------------------------------*/
//#define LOG_CONF_LEVEL_MAIN			     		 LOG_LEVEL_DBG
/*---------------------------------------------------------------------------*/
//To assign a different seed to each mote
#define LOG_CONF_different_seed	0
/*---------------------------------------------------------------------------*/
#define LOG_CONF_STATISTIC_DBG                           1
/*---------------------------------------------------------------------------*/
#define SIMULATIO_CONF_NUM_NODES			5 //2//10 //15 //20 //25 //50 //100 //200
/*---------------------------------------------------------------------------*/
#define	RPL_CONF_MOP	RPL_MOP_STORING_NO_MULTICAST
//#define	RPL_CONF_MOP	RPL_MOP_STORING_MULTICAST
//#define RPL_CONF_WITH_STORING 1
/*---------------------------------------------------------------------------*/
#define NETSTACK_MAX_ROUTE_ENTRIES 50 //Sky Mote = 50, Cooja Mote = 200
/*---------------------------------------------------------------------------*/
#define NBR_TABLE_CONF_MAX_NEIGHBORS 26 //Sky Mote = 26, Cooja Mote = 200
/*---------------------------------------------------------------------------*/
/*Var aux to count DIS,DIO,DAO*/
extern int dis_storing_rpl;
extern int dio_storing_rpl;
extern int dao_storing_rpl;

#endif /* PROJECT_CONF_H_ */
