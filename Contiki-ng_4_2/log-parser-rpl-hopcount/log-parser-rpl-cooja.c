/*
 * Copyright (C) 2019 Elisa Rojas(1), Hedayat Hosseini(2), and David Carrascal(1);
 *                    (1) GIST, University of Alcala, Spain.
 *                    (2) CEIT, Amirkabir University of Technology (Tehran
 *                        Polytechnic), Iran.
 * To read and pars the RPL log file generated in Contiki-ng
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODE_NUMBERS_MAX 500
#define CONDITION_MAX 11
#define GLOBAL_STATISTICS 1  //To print a metric for all runs in a file

#define REEDERROR 0
#define CONVERGED 1
#define NOTCONVERGED 2
#define CALCULATED 3
#define NOTCALCULATED 4


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
int linklocal_addr_to_nodeid(char linklocal[17], int node_id_max)
{
  for (int i=0; i<node_id_max; i++){
    if (strcmp(map[i].linklocal, linklocal) == 0){
      return i+1;
    }
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
//Converts a global address to a node_id
int global_addr_to_nodeid(char global[17], int node_id_max)
{
  for (int i=0; i<node_id_max; i++){
    if (strcmp(map[i].global, global) == 0){
      return i+1;
    }
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
//nexthop is a link local address, and the function conversts it to a node_id.
void add_default_route(int node_id, char nexthop[17], int node_id_max)
{
  int nexthop_id = linklocal_addr_to_nodeid(nexthop, node_id_max);
  default_routing_table[node_id-1] = nexthop_id;

}
/*---------------------------------------------------------------------------*/
//nexthop is a link local address, and the function conversts it to a node_id.
int remove_default_route(int node_id, char nexthop[17], int node_id_max)
{
  int nexthop_id = linklocal_addr_to_nodeid(nexthop, node_id_max);
  if (default_route[node_id-1] == nexthop_id){
    default_routing_table[node_id-1] = -1;
    return 1;
  }else{
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
//destination is a global address, and nexthop is a link local addresses, and
//the function conversts them to node_ids.
int add_route(int node_id, char destination[17], char nexthop[17], int node_id_max)
{
  int nexthop_id = linklocal_addr_to_nodeid(nexthop, node_id_max);
  int destination_id = global_addr_to_nodeid(destination, node_id_max);
  if (routing_table[node_id-1][destination_id-1] == -1){
    routing_table[node_id-1][destination_id-1] = nexthop_id;
    return 1;
  }else
    return 0;
}
/*---------------------------------------------------------------------------*/
//destination is a global address, and nexthop is a link local addresses, and
//the function conversts them to node_ids.
int remove_route(int node_id, char destination[17], char nexthop[17], int node_id_max)
{
  int nexthop_id = linklocal_addr_to_nodeid(nexthop, node_id_max);
  int destination_id = global_addr_to_nodeid(destination, node_id_max);
  if (routing_table[node_id-1][destination_id-1] == nexthop_id){
    routing_table[node_id-1][destination_id-1] = -1;
    return 1;
  }else
    return 0;
}
/*---------------------------------------------------------------------------*/
int get_nexthop(int src_id, int dst_id, int node_id_max)
{
  if ((src_id <= node_id_max) && (dst_id <= node_id_max)){
    return routing_table[src_id-1][dst_id-1];
  }else
    return 0;
}
/*---------------------------------------------------------------------------*/
int get_default_nexthop(src_id)
{
  if (src_id <= node_id_max){
    return default_routing_table[src_id-1];
  }else
    return 0;
}
/*---------------------------------------------------------------------------*/
//Hop count calculation
int calculate_hopcount(FILE *parsed_file_FP, FILE *numberOfHops, int node_id_max)
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

for(unsigned int i=1; i<=node_id_max; i++){
  fprintf(parsed_file_FP, "%u\t", i+1); //The left collumn of the table

  hopcount[i] = (int *)malloc(sizeof(int) * node_id_max);
  for(unsigned int j=1; j<=node_id_max; j++){
    if (i != j){
      int hopcount = 0;
      int nexthop_id = 0;
      int src_id = i;
      int dst_id = j;
      do{
        if ((nexthop_id = get_nexthop(src_id, dst_id)) == 0)
          nexthop_id = get_default_nexthop(src_id);
      }while((nexthop_id != dst) && (nexthop_id != 0) && (hopcount < node_id_max));
      if (nexthop_id != dst){   // if ((nexthop_id == 0) && (hopcount >= node_id_max))
        fprintf (parsed_file_FP, "\nThere is not a route between the nodes %d and %d\n");
        return NOTCALCULATED;
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
}
/*---------------------------------------------------------------------------*/
int log_file_parser(FILE *fp, char *parsed_file_name){

    int return_value;
    char line[200];
    int i,common_check=0,check_condition[CONDITION_MAX];

#if GLOBAL_STATISTICS == 1

     FILE *numberOfHops_FP;
     if((numberOfHops_FP=fopen("numberOfHops.txt","a"))==NULL){
        fprintf(stderr,"\nCan't open destination file (numberOfHops.txt)\n");
        return REEDERROR;
     }

#endif

    FILE *parsed_file_FP;
    if((parsed_file_FP=fopen(parsed_file_name,"w"))==NULL){
       fprintf(stderr,"\nCan't open destination file (%s)\n",parsed_file_name);
       return REEDERROR;

       }else{
           fprintf(parsed_file_FP,"** Generated by RPL Hopciunt log parser **\n\n");

           int node_id_max = 0;

           for (i=0; i<NODE_NUMBERS_MAX; i++){
             number_of_table_entries[i] = number_of_messages[i] = 0;
           }

           while ((fgets(line,200,fp)) && (convergence_time_end == 0)){
                for(i=0;i<CONDITION_MAX;i++)
                    check_condition[i]=0;

                common_check=(strstr(line,"Periodic Statistics:")) && 1; // || 1 to avoid warning because type of common_check is int and the type of strstr() is char *.
                check_condition[0] = strstr(line,"[INFO: Main      ] Tentative link-local IPv6 address:");
                check_condition[1] = common_check && (strstr(line,"add defaultroute to"));
                check_condition[2] = common_check && (strstr(line,"remove defaultroute from"));

                check_condition[3] = common_check && (strstr(line,"add route to"));
                check_condition[4] = common_check && (strstr(line,"delete route to"));

                check_condition[5] = common_check && (strstr(line,"convergence time ended"));

                if(check_condition[0]){ //link-local IPv6 address
                  int node_id;
                  char strtemp[17];
                  sscanf(strstr(line,"M[") + strlen("M["), "%d", &node_id);
                  sscanf(strstr(line,"link-local IPv6 address:") + strlen("link-local IPv6 address:"), "%s", node_addresses[node_id-1].linklocal);
                  sscanf(strstr(line,"link-local IPv6 address: fe80") + strlen("link-local IPv6 address: fe80"), "%s", str_temp);
                  node_addresses[node_id-1].global = strcat(node_addresses[node_id-1].global, str_temp)
                }

                if(check_condition[1]){ //add defaultroute
                  int node_id;
                  char nextHop[17];
                  sscanf(strstr(line,"M[") + strlen("M["), "%d", &node_id);
                  sscanf(strstr(line,"through") + strlen("through"), "%s", nextHop);
                  add_default_route(node_id, nextHop);

                  sscanf(strstr(line,"through") + strlen("through"), "%s", default_route[node_id-1].linklocal);
                  default_route[node_id-1].node_id = linklocal_addr_to_nodeid(default_route[node_id-1].linklocal);
                }
                if(check_condition[2]){ //remove defaultroute
                  int node_id;
                  char nextHop[17];
                  sscanf(strstr(line,"M[") + strlen("M["), "%d", &node_id);
                  sscanf(strstr(line,"through") + strlen("through"), "%d", nextHop);
                  remove_default_route(node_id, nextHop);
                }

                if(check_condition[3]){ //add route
                  int node_id;
                  char destination[17], nextHop[17];
                  sscanf(strstr(line,"M[") + strlen("M["), "%d] %s through %s", &node_id, destination, nextHop);
                  add_route(node_id, destination, nextHop);
                }

                if(check_condition[4]){ //delete route
                  int node_id;
                  char destination[17], nextHop[17];
                  sscanf(strstr(line,"M[") + strlen("M["), "%d] %s through %s", &node_id, destination, nextHop);
                  remove_route(node_id, destination, nextHop);
                }

                if(check_condition[5]){ //convergence time ended
                  return_value = CONVERGED; //converged
                }

           }//END while
           fprintf(parsed_file_FP, "Number of nodes:\t%d\n", node_id_max);
           fputs("---------------------------------------------------------------\n",parsed_file_FP);

           if (return_value == CONVERGED){
             if (calculate_hopcount(parsed_file_FP, numberOfHops_FP, node_id_max))
              return_value = CALCULATED;
             else
              return_value = NOTCALCULATED;
            }else
              return_value NOTCONVERGED;

           fclose(parsed_file_FP);
           fclose(numberOfHops_FP);

       }
       return return_value;
}
