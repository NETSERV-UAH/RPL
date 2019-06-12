/*
 * Copyright (C) 2019 Elisa Rojas(1), Hedayat Hosseini(2), and David Carrascal(1);
 *                    (1) GIST, University of Alcala, Spain.
 *                    (2) CEIT, Amirkabir University of Technology (Tehran
 *                        Polytechnic), Iran.
 * To read and pars the RPL log file generated in Contiki-ng
 */

 /*
  * The real node_id is started from 1 in Contiki-NG, but we suppose the node_id
  * is started from 0 because of the index of the array in the C language.
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NODE_NUMBERS_MAX 500
#define CONDITION_MAX 11
#define GLOBAL_STATISTICS 1  //To print a metric for all runs in a file

enum Error_Type {
  File_Read_Error = 0,
  Simulation_Converged,
  Simulation_Not_Converged,
  Hopcount_Calculated,
  Hopcount_Not_Calculated
};
typedef enum Error_Type Error_Type_t;

typedef struct{
  char linklocal[40];
  char global[40];
} Map_Entry_t;
Map_Entry_t address_map[NODE_NUMBERS_MAX]; //Index of the array reffers to node_id

int default_routing_table[NODE_NUMBERS_MAX];
int routing_table[NODE_NUMBERS_MAX][NODE_NUMBERS_MAX];

int main(int argc, char *argv[])
{
    int log_file_parser(FILE *, char *);
    FILE *fp;
    char parsed_file_name[50] = "output_file_";

    printf("\nRPL Hopcount log parser 2019\n\n");

    if (argc==1)
    {
        fprintf(stderr,"\nNo log file introduced\n");
    }
    else{
            if ((fp= fopen(*++argv,"r"))==NULL){
                fprintf(stderr,"\nCan't open %s\n",*argv);
                return 1;
            }else{
                strcat(parsed_file_name,*argv);
                if (log_file_parser(fp,parsed_file_name))
                    printf("\nNew files were generated successfully.\n");
                else
                    printf("\nNew files are not generated successfully! please try again.\n");
                fclose(fp);
            }

    }

    return 0;
}
/*---------------------------------------------------------------------------*/
//Converts a link local address to a node_id
int linklocal_addr_to_nodeid(char linklocal[40], int node_id_max)
{
  for (int i=0; i<node_id_max; i++){
    if (strcmp(address_map[i].linklocal, linklocal) == 0){
      return i+1;
    }
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
//Converts a global address to a node_id
int global_addr_to_nodeid(char global[40], int node_id_max)
{
  for (int i=0; i<node_id_max; i++){
    if (strcmp(address_map[i].global, global) == 0){
      return i+1;
    }
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
//nexthop is a link local address, and the function conversts it to a node_id.
void add_default_route(int node_id, char nexthop[40], int node_id_max)
{
  int nexthop_id = linklocal_addr_to_nodeid(nexthop, node_id_max);
  default_routing_table[node_id-1] = nexthop_id-1;

}
/*---------------------------------------------------------------------------*/
//nexthop is a link local address, and the function conversts it to a node_id.
int remove_default_route(int node_id, char nexthop[40], int node_id_max)
{
  int nexthop_id = linklocal_addr_to_nodeid(nexthop, node_id_max);
  if (default_routing_table[node_id-1] == nexthop_id-1){
    default_routing_table[node_id-1] = -1;
    return 1;
  }else{
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
//destination is a global address, and nexthop is a link local addresses, and
//the function conversts them to node_ids.
int add_route(int node_id, char destination[40], char nexthop[40], int node_id_max)
{
  int nexthop_id = linklocal_addr_to_nodeid(nexthop, node_id_max);
  int destination_id = global_addr_to_nodeid(destination, node_id_max);
  if (routing_table[node_id-1][destination_id-1] == -1){
    routing_table[node_id-1][destination_id-1] = nexthop_id-1;
    return 1;
  }else
    return 0;
}
/*---------------------------------------------------------------------------*/
//destination is a global address, and nexthop is a link local addresses, and
//the function conversts them to node_ids.
int remove_route(int node_id, char destination[40], char nexthop[40], int node_id_max)
{
  int nexthop_id = linklocal_addr_to_nodeid(nexthop, node_id_max);
  int destination_id = global_addr_to_nodeid(destination, node_id_max);
  if (routing_table[node_id-1][destination_id-1] == nexthop_id-1){
    routing_table[node_id-1][destination_id-1] = -1;
    return 1;
  }else
    return 0;
}
/*---------------------------------------------------------------------------*/
int get_nexthop_id(int src_id, int dst_id, int node_id_max)
{
  if ((src_id <= node_id_max) && (dst_id <= node_id_max)){
    return routing_table[src_id-1][dst_id-1];
  }else
    return 0;
}
/*---------------------------------------------------------------------------*/
int get_default_nexthop_id(int src_id, int node_id_max)
{
  if (src_id <= node_id_max){
    return default_routing_table[src_id-1];
  }else
    return 0;
}
/*---------------------------------------------------------------------------*/
//Hop count calculation
Error_Type_t calculate_hopcount(FILE *parsed_file_FP, FILE *numberOfHops_FP, int node_id_max)
{
/*Create, set, and initailize a 2-dimentional array/matrix to hold
 *the metric values.
 */
int **hopcount = (int **)malloc(sizeof(int *) * node_id_max);

//Print header of table
fprintf(parsed_file_FP, "node_id");
for(unsigned int i=0; i<node_id_max; i++){
  fprintf(parsed_file_FP, "\t%u", i+1);
}
fprintf(parsed_file_FP, "\n");

for(unsigned int i=0; i<node_id_max; i++){
  fprintf(parsed_file_FP, "%u\t", i+1); //The left collumn of the table

  hopcount[i] = (int *)malloc(sizeof(int) * node_id_max);
  for(unsigned int j=0; j<node_id_max; j++){
    int hopcountij = 0;
    int nexthop_id = 0;
    int src_id = i;
    int dst_id = j;

    if (i != j){
      do{
        //or if ((nexthop_id = get_nexthop_id(src_id, dst_id, node_id_max)) == 0)
        if ((nexthop_id = get_nexthop_id(src_id, dst_id, node_id_max)-1) == -1)
          nexthop_id = get_default_nexthop_id(src_id, node_id_max)-1;
      }while((nexthop_id != dst_id) && (nexthop_id != -1) && (hopcountij < node_id_max));
      //if node i(or src) couldn't reach node j(or dst)
      if (nexthop_id != dst_id){   // if ((nexthop_id == -1) && (hopcount >= node_id_max))
        fprintf (parsed_file_FP, "\nThere is not a route between the nodes %d and %d\n", src_id, dst_id);
        return Hopcount_Not_Calculated;
      }else{
        hopcount[i][j] = hopcountij;
        fprintf(parsed_file_FP, "(%d)\t", hopcount[i][j]);
      }
    }else{
      hopcount[i][j] = 0;
      fprintf(parsed_file_FP, "(%d)\t", hopcount[i][j]);
    }
  }
  fprintf(parsed_file_FP, "\n");
}

fputs("---------------------------------------------------------------\n",parsed_file_FP);

//Average calculation of hop count
float average_hop_count_mp2p = 0; //average hop count for Multi Point to Point traffic type
float average_hop_count_p2mp = 0; //average hop count for Point to Multi Point traffic type
float average_hop_count_p2p = 0;  //average hop count for Point to Point traffic type
float average_hop_count_all = 0;  //average hop count for all traffic type

int sum_hop_count_mp2p = 0;
int sum_hop_count_p2mp = 0;
int sum_hop_count_p2p = 0;
int sum_hop_count_all = 0;

for (unsigned int i=0; i<node_id_max; i++){
  for (unsigned int j=0; j<node_id_max; j++){
    if (hopcount[i][j] != -1){
      /* All nodes,i, except the root node,i!=0, are src, and the root
      * node,j==0, is dst.
      */
     if ((i != 0) && (j == 0)){
       average_hop_count_mp2p += hopcount[i][j];
       sum_hop_count_mp2p ++;
     }
     /* The root node,i==0, is src, and all nodes,j, except the root
      * node,j==0, is dst.
      */
     if ((i == 0) && (j != 0)){
       average_hop_count_p2mp += hopcount[i][j];
       sum_hop_count_p2mp ++;
     }
     /* All nodes except the root node,i!=0 and j!=0, are src and dst,
      * and src and dst are not the same, i!=j.
      */
     if ((i != 0) && (j != 0 ) && (i != j)){
       average_hop_count_p2p += hopcount[i][j];
       sum_hop_count_p2p ++;
     }
     /* All nodes are src and dst,
      * and src and dst are not the same, i!=j.
      */
     if (i != j){
       average_hop_count_all += hopcount[i][j];
       sum_hop_count_all ++;
     }
    }
  }
}

//sum_hop_count_mp2p = node_id_max - 1
average_hop_count_mp2p /= sum_hop_count_mp2p;
//sum_hop_count_p2mp = node_id_max - 1
average_hop_count_p2mp /= sum_hop_count_p2mp;
//sum_hop_count_p2p = node_id_max * node_id_max - 3 * node_id_max + 2
average_hop_count_p2p /= sum_hop_count_p2p;
//sum_hop_count_all = node_id_max * node_id_max - node_id_max
average_hop_count_all /= sum_hop_count_all;

fprintf(parsed_file_FP, "average_hop_count_mp2p\t%f\n", average_hop_count_mp2p);
fprintf(parsed_file_FP, "average_hop_count_p2mp\t%f\n", average_hop_count_p2mp);
fprintf(parsed_file_FP, "average_hop_count_p2p\t%f\n", average_hop_count_p2p);
fprintf(parsed_file_FP, "average_hop_count_all\t%f\n", average_hop_count_all);

#if GLOBAL_STATISTICS == 1
fprintf(numberOfHops_FP, "%f\n", average_hop_count_all);
#endif

//Release memory
//nodes array
  for (int i=0; i<node_id_max; i++)
    free(hopcount[i]);
  free(hopcount);

  return Hopcount_Calculated;
}
/*---------------------------------------------------------------------------*/
int log_file_parser(FILE *fp, char *parsed_file_name){

    Error_Type_t return_value;
    char line[200];
    int converged = 0;
    int common_check = 0;
    int check_condition[CONDITION_MAX];

#if GLOBAL_STATISTICS == 1

     FILE *numberOfHops_FP;
     if((numberOfHops_FP=fopen("numberOfHops.txt","a"))==NULL){
        fprintf(stderr,"\nCan't open destination file (numberOfHops.txt)\n");
        return File_Read_Error;
     }

#endif

    FILE *parsed_file_FP;
    if((parsed_file_FP=fopen(parsed_file_name,"w"))==NULL){
       fprintf(stderr,"\nCan't open destination file (%s)\n",parsed_file_name);
       return File_Read_Error;

       }else{
           fprintf(parsed_file_FP,"** Generated by RPL Hopciunt log parser **\n\n");

           int node_id_max = 0;

           for (int i=0; i<NODE_NUMBERS_MAX; i++){
             default_routing_table[i] = -1; //-1 means UNSPECIFIED_ADDRESS
             address_map[i].linklocal[0] = '\0';
             address_map[i].global[0] = '\0';
             for (int j=0; j<NODE_NUMBERS_MAX; j++){
               routing_table[i][j] = -1; //-1 means UNSPECIFIED_ADDRESS
             }
           }

           while ((fgets(line,200,fp)) && (!converged)){
                for(int i=0;i<CONDITION_MAX;i++)
                    check_condition[i]=0;

                common_check=(strstr(line,"Periodic Statistics:")) && 1; // || 1 to avoid warning because type of common_check is int and the type of strstr() is char *.
                check_condition[0] = 1 && strstr(line,"[INFO: Main      ] Tentative link-local IPv6 address:");
                check_condition[1] = common_check && (strstr(line,"add defaultroute to"));
                check_condition[2] = common_check && (strstr(line,"remove defaultroute from"));

                check_condition[3] = common_check && (strstr(line,"add route to"));
                check_condition[4] = common_check && (strstr(line,"delete route to"));

                check_condition[5] = common_check && (strstr(line,"convergence time ended"));

                if(check_condition[0]){ //link-local IPv6 address
                  int node_id;
                  char addr_suffix[40];
                  sscanf(strstr(line,"ID:") + strlen("ID:"), "%d", &node_id);
                  if (node_id > node_id_max)
                    node_id_max = node_id;
                  sscanf(strstr(line,"link-local IPv6 address:") + strlen("link-local IPv6 address:"), "%s", address_map[node_id-1].linklocal);
                  sscanf(strstr(line,"link-local IPv6 address: fe80") + strlen("link-local IPv6 address: fe80"), "%s", addr_suffix);
                  strcpy(address_map[node_id-1].global, "fd00");
                  strcat(address_map[node_id-1].global, addr_suffix);
                }

                if(check_condition[1]){ //add defaultroute
                  int node_id;
                  char nextHop[40];
                  sscanf(strstr(line,"M[") + strlen("M["), "%d", &node_id);
                  sscanf(strstr(line,"through") + strlen("through"), "%s", nextHop);
                  add_default_route(node_id, nextHop, node_id_max);
                }

                if(check_condition[2]){ //remove defaultroute
                  int node_id;
                  char nextHop[40];
                  sscanf(strstr(line,"M[") + strlen("M["), "%d", &node_id);
                  sscanf(strstr(line,"through") + strlen("through"), "%s", nextHop);
                  remove_default_route(node_id, nextHop, node_id_max);
                }

                if(check_condition[3]){ //add route
                  int node_id;
                  char destination[40], nextHop[40];
                  sscanf(strstr(line,"M[") + strlen("M["), "%d] %s through %s", &node_id, destination, nextHop);
                  //printf("%d\t%s\t%s\n", node_id, destination, nextHop);
                  add_route(node_id, destination, nextHop, node_id_max);
                }
/*
                if(check_condition[4]){ //delete route
                  int node_id;
                  char destination[40], nextHop[40];
                  sscanf(strstr(line,"M[") + strlen("M["), "%d] %s through %s", &node_id, destination, nextHop);
                  remove_route(node_id, destination, nextHop, node_id_max);
                }
*/
                if(check_condition[5]){ //convergence time ended
                  converged = 1;
                  return_value = Simulation_Converged;
                }

           }//END while
           fprintf(parsed_file_FP, "Number of nodes:\t%d\n", node_id_max);
           fputs("---------------------------------------------------------------\n",parsed_file_FP);

           //print the node addresses
           fprintf(parsed_file_FP, "Node id\tLink Local Address\tGlobal address\n");
           for (int i=0; i<node_id_max; i++){
             fprintf(parsed_file_FP, "%7d\t%18s\t%14s\n", i+1, address_map[i].linklocal, address_map[i].global);
           }
           fputs("---------------------------------------------------------------\n",parsed_file_FP);

           //print the default routing tables
           fprintf(parsed_file_FP, "Node id\tDefault Next Hop address(node id)\tDefault Next Hop address(Link Local)\n");
           for (int i=0; i<node_id_max; i++){
             char nexthop_linklocal_addr[40];
             if (default_routing_table[i] != -1){
               strcpy(nexthop_linklocal_addr, address_map[default_routing_table[i]].linklocal);
             }else{
               strcpy(nexthop_linklocal_addr, "UNSPECIFIED_ADDRESS");
             }
             fprintf(parsed_file_FP, "%7d\t%33d\t%36s\n", i+1, default_routing_table[i] + 1, nexthop_linklocal_addr);
           }
           fputs("---------------------------------------------------------------\n",parsed_file_FP);

           //print the routing tables
           fprintf(parsed_file_FP, "Node id\tNext Hop address(node id)\tNext Hop address(Link Local)\n");
           for (int i=0; i<node_id_max; i++){  //i :src index
             fprintf(parsed_file_FP, "%7d", i+1);
             for (int j=0; j<node_id_max; j++){ //j: dst index
/*               char nexthop_linklocal_addr[40];
               if (routing_table[i][j] != -1){ //routing_table[i][j]: includes the next hop for route i->j
                 strcpy(nexthop_linklocal_addr, address_map[routing_table[i][j]].linklocal);
               }else{
                 strcpy(nexthop_linklocal_addr, "UNSPECIFIED_ADDRESS");
               }
              fprintf(parsed_file_FP, "\t%33d\t%36s", routing_table[i][j] + 1, nexthop_linklocal_addr);
*/             fprintf(parsed_file_FP, "\t%33d", routing_table[i][j] + 1);}
             fprintf(parsed_file_FP, "\n");
           }
           fputs("---------------------------------------------------------------\n",parsed_file_FP);

           if (return_value == Simulation_Converged)
             return 12;//calculate_hopcount(parsed_file_FP, numberOfHops_FP, node_id_max);
           else
              return_value = Simulation_Not_Converged;

           fclose(parsed_file_FP);
           fclose(numberOfHops_FP);

       }
       return return_value;
}
