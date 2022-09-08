
#include <stdio.h>
#include <string.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h> 
#include "parser.h"
#include "regexcheck.h"

cmd_args * parse_cmds(int argc, char ** argv, cmd_args * cmd_struct)
{
    //printf("in parse_cmds\n");
    int c;
    opterr = 0;
    int mode_count = 0;
    bool port_exists = false;
    bool ip_exists = false;
    
    regex_t file_reg = compile_valid_filename_regex();
    //printf("returned file_reg successfully\n");
    regex_t nums_reg = compile_valid_nums_regex();
   // printf("returned nums_reg successfully\n");
    regex_t amount_reg = compile_valid_amount_regex();
    //printf("returned amount_reg successfully\n");

    while ((c = getopt (argc, argv, "a:s:i:p:c:n:d:w:g")) != -1) {
        
        switch (c) {
        case 'a':
            //printf("parsing a opt before regex\n");
            check_case_a(file_reg, optarg);
            //printf("parsing a opt after regex\n");
            memcpy(cmd_struct->account, optarg, strlen(optarg) + 1);
            // cmd_struct->account = optarg;
            cmd_struct->account[strlen(optarg)+1] = '\0';
            //printf("afetr assigning to struct \n");
            break;
        case 's':
            check_case_s(file_reg, optarg);
            memcpy(cmd_struct->authfile, optarg, strlen(optarg) + 1);
            cmd_struct->authfile[strlen(optarg)+1] = '\0';
            // cmd_struct->authfile = optarg;
            break;
        case 'i':
            if (ip_exists == true) {
                exit(255);
            }
           // printf("inside case i in parser, the ip  arg passed in is %s\n", optarg); 

            check_case_i(optarg);
            memcpy(cmd_struct->ip, optarg, strlen(optarg) + 1);
            cmd_struct->ip[strlen(optarg)+1] = '\0';
            // cmd_struct->ip = optarg;
            ip_exists = true;
            //printf("%s this is the ip after doing regex check", cmd_struct->ip);
            break;
        case 'p':
            if (port_exists == true) {
                exit(255);
            }
            //printf("inside case p in parser about to check case\n");  
            check_case_p(nums_reg, optarg);
            unsigned short port = atoi(optarg);
            cmd_struct->port = port;
            port_exists = true;
            break;
        case 'c':
            //printf("in case c\n");
            check_case_c(file_reg, optarg);
            memcpy(cmd_struct->card, optarg, strlen(optarg) + 1);
            cmd_struct->card[strlen(optarg)+1] = '\0';

            // cmd_struct->card = optarg;
            break;
        case 'n':
            mode_count++;
            //printf("in opt n bfore regex\n");
            check_case_n(amount_reg, optarg);
            //printf("in opt n after regex\n");
            
            char mode_n[10] = "create";
            memcpy(cmd_struct->mode, mode_n, strlen(mode_n) + 1);
            // cmd_struct->mode = "create";
            cmd_struct->mode[strlen(mode_n)+1] = '\0';
            memcpy(cmd_struct->amount, optarg, strlen(optarg) + 1);
            cmd_struct->amount[strlen(optarg)+1] = '\0';
            // cmd_struct->amount = optarg;
            // printf("in opt n after assignment\n");
            break;
        case 'd':
            mode_count++;
            char mode_d [10] = "deposit";
            check_case_d_w(amount_reg, optarg);
            memcpy(cmd_struct->mode, mode_d, strlen(mode_d) + 1);
            cmd_struct->mode[strlen(mode_d)+1] = '\0';
            memcpy(cmd_struct->amount, optarg, strlen(optarg) + 1);
            cmd_struct->amount[strlen(optarg)+1] = '\0';
            // cmd_struct->mode = "deposit";
            // cmd_struct->amount = optarg;
            break;
        case 'w':
            mode_count++;
            char mode_w[10] = "withdraw";
            check_case_d_w(amount_reg, optarg);
            // cmd_struct->mode = "withdraw";
            memcpy(cmd_struct->mode, mode_w, strlen(mode_w) + 1);
            cmd_struct->mode[strlen(mode_w)+1] = '\0';
            memcpy(cmd_struct->amount, optarg, strlen(optarg) + 1);
            cmd_struct->amount[strlen(optarg)+1] = '\0';
            // cmd_struct->amount = optarg;
            break;
        case 'g':
            mode_count++;
            char mode_b [10] = "balance";
            //printf("in balance case in parser.c\n");
            memcpy(cmd_struct->mode, mode_b, strlen(mode_b) + 1);
            cmd_struct->mode[strlen(mode_b)+1] = '\0';
            // cmd_struct->mode = "balance";
           // printf("in balance case after setting mode\n");
            break;
        // options do not match the provided ones
        case '?':
            exit(255);
        default:
            exit(255);
        }
    }
    if (mode_count > 1) {
        exit(255);
    } 
        
  return cmd_struct;
}
