/*
 * Copyright (C) 2018 Elisa Rojas(1), Hedayat Hosseini(2), and David Carrascal(1);
 *                    (1) GIST, University of Alcala, Spain.
 *                    (2) CEIT, Amirkabir University of Technology (Tehran
 *                        Polytechnic), Iran.
 * To read and process the IoTorii log file generated in Contiki-ng
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NODE_NUMBERS_MAX 500
#define CONDITION_MAX 11
#define HLMAC_STR_LEN_MAX 20
#define GLOBAL_STATISTICS 1  //To print a metric for all runs in a file
#define LOG_SCRIPT_SEARCH_MAX 100
#define NUMBER_OF_STATS_FILES 12  //To order parsed and raw data

int main(int argc, char *argv[])
{
    int log_file_parser(FILE *, char *, char *);
    void log_file_order(char *, int , int );  //To order parsed and raw data

    FILE *input_file_fp;
    char destfile[62];
    int ok_count;

    printf("\nRPL script log parser 2019\n\n");

    if (argc < 4)
    {
        fprintf(stderr,"\n 3 parameters are required: base name, start seeds, and Ok count.\n");
        return 0;
    } else{
            FILE *parser_log_fp;
            int has_log = 0;
            if (parser_log_fp = fopen("parser.log.txt", "w"))
              has_log = 1;

            char base_name[20];
            char seed_str[10];
            char num_ok_run_str[5];
            int seed_int,aux_seed_int;
            int num_ok_run_int;
            char input_file[50];

            strcpy(base_name, argv[1]);
            strcpy(seed_str, argv[2]);
            strcpy(num_ok_run_str, argv[3]);
            printf("base name: %s\nstart seed: %s\nOk count: %s\n", base_name, seed_str, num_ok_run_str);
            printf("========================================\n");

            strcat(strcat(strcat(strcpy(input_file, base_name), "."), seed_str), ".scriptlog");

            seed_int = atoi(seed_str);
            aux_seed_int = seed_int;
            num_ok_run_int = atoi(num_ok_run_str);

            int fail_count = 0;
            int not_converged = 0;
            for (ok_count = 0; ok_count < num_ok_run_int && fail_count <= LOG_SCRIPT_SEARCH_MAX; seed_int++){
              sprintf(seed_str, "%d", seed_int);
              printf("Seed number: %s\n", seed_str);
              if (has_log)
                fprintf(parser_log_fp, "Seed number: %s\n", seed_str);

              strcat(strcat(strcat(strcpy(input_file, base_name), "."), seed_str), ".scriptlog");
              printf("Input file: %s\n", input_file);
              if (has_log)
                fprintf(parser_log_fp, "Input file: %s\n", input_file);

              if (input_file_fp = fopen(input_file,"r")){
                fail_count = 0;
                strcpy(destfile, "output_file_");
                strcat(destfile, input_file);
                printf("Output file: %s\n", destfile);
                if (has_log)
                  fprintf(parser_log_fp, "Output file: %s\n", destfile);

                int parser_result = log_file_parser(input_file_fp, destfile, seed_str);
                if (parser_result == 1){
                    printf("Parser result: Converged. \n");
                if (has_log)
                    fprintf(parser_log_fp, "Parser result: Converged. \n\n");
                    ok_count++;
                } else if (parser_result == 0){
                    printf("Parser result: Not converged. \n");
                    if (has_log)
                        fprintf(parser_log_fp, "Parser result: Not converged. \n\n");
                    not_converged++;
                }else{
                    printf("Parser result: A file can not be opened!\n");
                    if (has_log)
                        fprintf(parser_log_fp, "Parser result: A file can not be opened!\n\n");
                }
                fclose(input_file_fp);
                printf("----------------------------------------\n");
              }else{
                fail_count++;
                printf("Parser result: %s can not be opened!\n", input_file);
                printf("----------------------------------------\n");
              }
            }

            printf("number of converged: %d\n", ok_count);
            printf("number of not converged: %d\n", not_converged);
            if (has_log){
              fprintf(parser_log_fp, "number of converged: %d\n", ok_count);
              fprintf(parser_log_fp, "number of not converged: %d\n", not_converged);
            }
            fclose(parser_log_fp);

            log_file_order(base_name, aux_seed_int, num_ok_run_int); //To order parsed and raw data
    }

    
    return ok_count;
}
/*---------------------------------------------------------------------------*/
int log_file_parser(FILE *fp, char *destfile, char *seed){

    char line[200];
    int i,common_check=0,check_condition[CONDITION_MAX];

#if GLOBAL_STATISTICS == 1

    FILE *ConvergenceTime; //A file to save convergence time for all nodes
    if((ConvergenceTime=fopen("01_ConvergenceTime.txt","a")) == NULL){
       fprintf(stderr,"\nCan't open destination file (01_ConvergenceTime.txt)\n");
       return -1;

     }

     FILE *numberOfTableEntries;
     if((numberOfTableEntries=fopen("02_NumberOfTableEntries.txt","a"))==NULL){
        fprintf(stderr,"\nCan't open destination file (02_NumberOfTableEntries.txt)\n");
        return -1;

     }

     FILE *numberOfMessages;
     if((numberOfMessages=fopen("03_NumberOfMessages.txt","a"))==NULL){
        fprintf(stderr,"\nCan't open destination file (03_NumberOfMessages.txt)\n");
        return -1;

     }

     FILE *numberOfHops;
     if((numberOfHops=fopen("04_NumberOfHops.txt","a"))==NULL){
        fprintf(stderr,"\nCan't open destination file (04_NumberOfHops.txt)\n");
        return -1;

     }

     FILE *numberOfNeighbors;
     if((numberOfNeighbors=fopen("05_NumberOfNeighbors.txt","a"))==NULL){
        fprintf(stderr,"\nCan't open destination file (05_NumberOfNeighbors.txt)\n");
        return -1;

     }

     FILE *numberOfParents;
     if((numberOfParents=fopen("06_NumberOfParents.txt","a"))==NULL){
        fprintf(stderr,"\nCan't open destination file (06_NumberOfParents.txt)\n");
        return -1;

     }

     FILE *numberOfDefaultRoutes;
     if((numberOfDefaultRoutes=fopen("07_NumberOfDefaultRoutes.txt","a"))==NULL){
        fprintf(stderr,"\nCan't open destination file (07_NumberOfDefaultRoutes.txt)\n");
        return -1;

     }

     FILE *numberOfRoutes;
     if((numberOfRoutes=fopen("08_NumberOfRoutes.txt","a"))==NULL){
        fprintf(stderr,"\nCan't open destination file (08_NumberOfRoutes.txt)\n");
        return -1;

     }

     FILE *numberOfSREntries;
     if((numberOfSREntries=fopen("09_NumberOfSREntries.txt","a"))==NULL){
        fprintf(stderr,"\nCan't open destination file (09_NumberOfSREntries.txt)\n");
        return -1;

     }

     FILE *numberOfDISMessages;
     if((numberOfDISMessages=fopen("10_NumberOfDISMessages.txt","a"))==NULL){
        fprintf(stderr,"\nCan't open destination file (10_NumberOfDISMessages.txt)\n");
        return -1;

     }

     FILE *numberOfDIOMessages;
     if((numberOfDIOMessages=fopen("11_NumberOfDIOMessages.txt","a"))==NULL){
        fprintf(stderr,"\nCan't open destination file (11_NumberOfDIOMessages.txt)\n");
        return -1;

     }

     FILE *numberOfDAOMessages;
     if((numberOfDAOMessages=fopen("12_NumberOfDAOMessages.txt","a"))==NULL){
        fprintf(stderr,"\nCan't open destination file (12_NumberOfDAOMessages.txt)\n");
        return -1;

     }

#endif

    FILE *destfp;
    int return_value = 0;

    if((destfp=fopen(destfile,"w"))==NULL){
       fprintf(stderr,"\nCan't open destination file (%s)\n",destfile);
       return -1;

       }else{
           fprintf(destfp, "** generated by script RPL log parser **\n\nSeed number\t%s\n", seed);

           int node_id_max = 0;
           long int convergence_time_start = 0;
           long int convergence_time_end = 0;
           long int convergence_time = 0;
           int number_of_neighbours[NODE_NUMBERS_MAX], number_of_parents[NODE_NUMBERS_MAX], number_of_default_routes[NODE_NUMBERS_MAX], number_of_routes[NODE_NUMBERS_MAX], number_of_sr_entries[NODE_NUMBERS_MAX];
           int number_of_dis_messages[NODE_NUMBERS_MAX], number_of_dio_messages[NODE_NUMBERS_MAX], number_of_dao_messages[NODE_NUMBERS_MAX];
           int number_of_table_entries[NODE_NUMBERS_MAX], number_of_messages[NODE_NUMBERS_MAX];
           //int sum_hop[NODE_NUMBERS_MAX];
           float average_sum_hop_for_node = 0;

           //Variables to calculate average
           float average_number_of_neighbours = 0, average_number_of_parents = 0, average_number_of_default_routes = 0, average_number_of_routes = 0, average_number_of_sr_entries = 0;
           float average_number_of_dis_messages = 0, average_number_of_dio_messages = 0, average_number_of_dao_messages = 0;
           float average_number_of_table_entries = 0, average_number_of_messages = 0;
           float average_sum_hop = 0;

           for (i=0; i<NODE_NUMBERS_MAX; i++){
             number_of_neighbours[i] = number_of_parents[i] = number_of_default_routes[i] = number_of_routes[i] = number_of_sr_entries[i] = 0;
             number_of_dis_messages[i] = number_of_dio_messages[i] = number_of_dao_messages[i] = 0;
             number_of_table_entries[i] = number_of_messages[i] = 0;
           }

           while ((fgets(line,200,fp)) && (convergence_time_end == 0)){
                for(i=0;i<CONDITION_MAX;i++)
                    check_condition[i]=0;

                common_check=(strstr(line,"Periodic Statistics:")) && 1; // || 1 to avoid warning because type of common_check is int and the type of strstr() is char *.
                check_condition[0] = common_check && (strstr(line,"DIS:"));
                check_condition[1] = common_check && (strstr(line,"DIO:"));
                check_condition[2] = common_check && (strstr(line,"DAO:"));

                check_condition[3] = common_check && (strstr(line,"convergence time started"));
                check_condition[4] = common_check && (strstr(line,"convergence time ended"));

                check_condition[5] = common_check && (strstr(line,"Neighbors:"));
                check_condition[6] = common_check && (strstr(line,"Parents:"));

                check_condition[7] = common_check && (strstr(line,"Default routes:"));
                check_condition[8] = common_check && (strstr(line,"Routes:"));

                check_condition[9] = common_check && (strstr(line,"Number of nodes:"));

                check_condition[10] = common_check && (strstr(line,"SR-Entries:"));


                if(check_condition[0]){ //Number of DIS
                  int node_id;
                  sscanf(strstr(line,"M[") + strlen("M["), "%d", &node_id);
                  sscanf(strstr(line,"DIS:") + strlen("DIS:"), "%d", &number_of_dis_messages[node_id-1]);
                }

                if(check_condition[1]){ //Number of DIO
                  int node_id;
                  sscanf(strstr(line,"M[") + strlen("M["), "%d", &node_id);
                  sscanf(strstr(line,"DIO:") + strlen("DIO:"), "%d", &number_of_dio_messages[node_id-1]);
                }
                if(check_condition[2]){ //Number of DAO
                  int node_id;
                  sscanf(strstr(line,"M[") + strlen("M["), "%d", &node_id);
                  sscanf(strstr(line,"DAO:") + strlen("DAO:"), "%d", &number_of_dao_messages[node_id-1]);
                }

                if(check_condition[3]){ //convergence time started
                  sscanf(line, "%ld", &convergence_time_start);
                  printf("start %ld\n", convergence_time_start);
                }

                if(check_condition[4]){ //convergence time ended
                  return_value = 1;
                  sscanf(line, "%ld", &convergence_time_end);
                  printf("end %ld\n", convergence_time_end);
                }

                if(check_condition[5]){ //Neigbors
                  int node_id;
                  sscanf(strstr(line,"M[") + strlen("M["), "%d", &node_id);
                  sscanf(strstr(line,"Neighbors:") + strlen("Neighbors:"), "%d", &number_of_neighbours[node_id-1]);
                }

                if(check_condition[6]){ //Parents
                  int node_id;
                  sscanf(strstr(line,"M[") + strlen("M["), "%d", &node_id);
                  sscanf(strstr(line,"Parents:") + strlen("Parents:"), "%d", &number_of_parents[node_id-1]);
                }

                if(check_condition[7]){ //Default Routes
                  int node_id;
                  sscanf(strstr(line,"M[") + strlen("M["), "%d", &node_id);
                  sscanf(strstr(line,"Default routes:") + strlen("Default routes:"), "%d", &number_of_default_routes[node_id-1]);
                }

                if(check_condition[8]){ //Routes
                  int node_id;
                  sscanf(strstr(line,"M[") + strlen("M["), "%d", &node_id);
                  sscanf(strstr(line,"Routes:") + strlen("Routes:"), "%d", &number_of_routes[node_id-1]);
                }

                if(check_condition[9]){ //Number of nodes:
                  sscanf(strstr(line,"Number of nodes:") + strlen("Number of nodes:"), "%d", &node_id_max);
                }

                if(check_condition[10]){ //SR-Entries
                  int node_id;
                  sscanf(strstr(line,"M[") + strlen("M["), "%d", &node_id);
                  sscanf(strstr(line,"SR-Entries:") + strlen("SR-Entries:"), "%d", &number_of_sr_entries[node_id-1]);
                }

           }//END while
           fprintf(destfp, "Number of nodes:\t%d\n", node_id_max);

           fprintf(destfp, "convergence_time_start\t%ld\t(s)\n", convergence_time_start);
           fprintf(destfp, "convergence_time_end\t%ld\t(s)\n", convergence_time_end);

           convergence_time = convergence_time_end - convergence_time_start;
           fprintf(destfp, "convergence_time\t%ld\t(s)\n", convergence_time);
           fputs("---------------------------------------------------------------\n",destfp);

           fprintf(destfp, "node_id\tnumber_of_neighbours\tnumber_of_parents\tnumber_of_default_routes\tnumber_of_routes\tnumber_of_sr_entries\tnumber_of_table_entries");
           fprintf(destfp, "\tnumber_of_dis_messages\tnumber_of_dio_messages\tnumber_of_dao_messages\tnumber_of_messages\n");
           //fprintf(destfp, "\tsum_hop\taverage_sum_hop_for_node\n");
           int i;
           for (i=0; i<node_id_max; i++){
             number_of_table_entries[i] = number_of_neighbours[i] + number_of_parents[i] + number_of_default_routes[i] + number_of_routes[i] + number_of_sr_entries[i];
             number_of_messages[i] = number_of_dis_messages[i] + number_of_dio_messages[i] + number_of_dao_messages[i];
             fprintf(destfp, "%7d\t%20d\t%17d\t%24d\t%16d\t%20d\t%23d\t%22d\t%22d\t%22d\t%18d\n", i+1, number_of_neighbours[i], number_of_parents[i], number_of_default_routes[i], number_of_routes[i],number_of_sr_entries[i], number_of_table_entries[i], number_of_dis_messages[i], number_of_dio_messages[i], number_of_dao_messages[i], number_of_messages[i]);

             //Summation calculation
             average_number_of_neighbours += number_of_neighbours[i];
             average_number_of_parents += number_of_parents[i];
             average_number_of_default_routes += number_of_default_routes[i];
             average_number_of_routes += number_of_routes[i];
             average_number_of_sr_entries += number_of_sr_entries[i];
             average_number_of_table_entries += number_of_table_entries[i];

             average_number_of_dis_messages += number_of_dis_messages[i];
             average_number_of_dio_messages += number_of_dio_messages[i];
             average_number_of_dao_messages += number_of_dao_messages[i];
             average_number_of_messages += number_of_messages[i];
             //average_sum_hop += sum_hop[i];
           }
           fputs("---------------------------------------------------------------\n",destfp);

           fprintf(destfp, "Sum    \t%20f\t%17f\t%24f\t%16f\t%20f\t%23f\t%22f\t%22f\t%22f\t%18f\n", average_number_of_neighbours, average_number_of_parents, average_number_of_default_routes, average_number_of_routes, average_number_of_sr_entries, average_number_of_table_entries, average_number_of_dis_messages, average_number_of_dio_messages, average_number_of_dao_messages, average_number_of_messages);

           //Average calculation
           //average_sum_hop /= average_number_of_hlmac_addresses; //Now, average_number_of_hlmac_addresses includes the total number of the hlmac addresses
           average_number_of_neighbours /= node_id_max;
           average_number_of_parents /= node_id_max;
           average_number_of_default_routes /= node_id_max;
           average_number_of_routes /= node_id_max;
           average_number_of_table_entries /= node_id_max;

           average_number_of_dis_messages /= node_id_max;
           average_number_of_dio_messages /= node_id_max;
           average_number_of_dao_messages /= node_id_max;
           average_number_of_messages /= node_id_max;

           fprintf(destfp, "Average\t%20f\t%17f\t%24f\t%16f\t%20f\t%23f\t%22f\t%22f\t%22f\t%18f\n", average_number_of_neighbours, average_number_of_parents, average_number_of_default_routes, average_number_of_routes, average_number_of_sr_entries, average_number_of_table_entries, average_number_of_dis_messages, average_number_of_dio_messages, average_number_of_dao_messages, average_number_of_messages);

           #if GLOBAL_STATISTICS == 1
           if (return_value == 1){
             fprintf(ConvergenceTime, "%ld\n", convergence_time);
             fprintf(numberOfTableEntries, "%f\n", average_number_of_table_entries);
             fprintf(numberOfNeighbors, "%f\n", average_number_of_neighbours);
             fprintf(numberOfParents, "%f\n", average_number_of_parents);
             fprintf(numberOfDefaultRoutes, "%f\n", average_number_of_default_routes);
             fprintf(numberOfRoutes, "%f\n", average_number_of_routes);
             fprintf(numberOfSREntries, "%f\n", average_number_of_sr_entries);

             fprintf(numberOfMessages, "%f\n", average_number_of_messages);
             fprintf(numberOfDISMessages, "%f\n", average_number_of_dis_messages);
             fprintf(numberOfDIOMessages, "%f\n", average_number_of_dio_messages);
             fprintf(numberOfDAOMessages, "%f\n", average_number_of_dao_messages);
           }
           #endif
           fputs("---------------------------------------------------------------\n",destfp);

           fclose(destfp);
           //fclose(numberOfHops);
           fclose(ConvergenceTime);
           fclose(numberOfTableEntries);
           fclose(numberOfNeighbors);
           fclose(numberOfParents);
           fclose(numberOfDefaultRoutes);
           fclose(numberOfRoutes);
           fclose(numberOfSREntries);
           fclose(numberOfMessages);
           fclose(numberOfDISMessages);
           fclose(numberOfDIOMessages);
           fclose(numberOfDAOMessages);

       }
       return return_value;
}

void log_file_order(char * base_name, int seed , int runs ){  //To order parsed and raw data

  //Var aux.
  char command[200];
  char aux_seed_str[10];
  int  aux_seed_int=seed;

  //To create directories tree
  system("mkdir Cooja_logs");
  system("mkdir Raw_data");
  system("mkdir Parsed_data");
  
  //To move the cooja logs
  for(int i = 0; i < runs; i++){
    sprintf(aux_seed_str,"%d",aux_seed_int);
    strcat(strcat(strcat(strcat(strcpy(command, "mv "),base_name),"."),aux_seed_str),".coojalog ./Cooja_logs/");
    system(command);
    aux_seed_int++;
  }

  //To move the script logs
  aux_seed_int = seed;
  for(int i = 0; i < runs; i++){
    sprintf(aux_seed_str,"%d",aux_seed_int);
    strcat(strcat(strcat(strcat(strcpy(command, "mv "),base_name),"."),aux_seed_str),".scriptlog ./Raw_data/");
    system(command);
    aux_seed_int++;
  }

  //To move the parsed data
  aux_seed_int = seed;
  for(int i = 0; i < runs; i++){
    sprintf(aux_seed_str,"%d",aux_seed_int);
    strcat(strcat(strcat(strcat(strcat(strcpy(command, "mv "),"output_file_"),base_name),"."),aux_seed_str),".scriptlog ./Parsed_data/");
    system(command);
    aux_seed_int++;
  }

  for(int i=1; i < NUMBER_OF_STATS_FILES + 1; i ++){
    switch(i){
      case 1:
        strcpy(command, "mv 01_ConvergenceTime.txt ./Parsed_data/");
        system(command);
        break;
      case 2:
        strcpy(command, "mv 02_NumberOfTableEntries.txt ./Parsed_data/");
        system(command);
        break;
      case 3:
        strcpy(command, "mv 03_NumberOfMessages.txt ./Parsed_data/");
        system(command);
        break;
      case 4:
        strcpy(command, "mv 04_NumberOfHops.txt ./Parsed_data/");
        system(command);
        break;
      case 5:
        strcpy(command, "mv 05_NumberOfNeighbors.txt ./Parsed_data/");
        system(command);
        break;
      case 6: 
        strcpy(command, "mv 06_NumberOfParents.txt ./Parsed_data/");
        system(command);
        break;
      case 7:
        strcpy(command, "mv 07_NumberOfDefaultRoutes.txt ./Parsed_data/");
        system(command);
        break;
      case 8:
        strcpy(command, "mv 08_NumberOfRoutes.txt ./Parsed_data/");
        system(command);
        break;
      case 9:
        strcpy(command, "mv 09_NumberOfSREntries.txt ./Parsed_data/");
        system(command);
        break;
      case 10:
        strcpy(command, "mv 10_NumberOfDISMessages.txt ./Parsed_data/");
        system(command);
        break;
      case 11:
        strcpy(command, "mv 11_NumberOfDIOMessages.txt ./Parsed_data/");
        system(command);
        break;
      case 12:
        strcpy(command, "mv 12_NumberOfDAOMessages.txt ./Parsed_data/");
        system(command);
        break;
    }
    
  }

}