#include "atm.h"
#include "parser.h"
#include <stdio.h>
#include <argp.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <unistd.h>

// Default port and ip address are defined here

int main(int argc, char** argv){
  	// unsigned short port = 3000;
	// char *ipAddr = "127.0.0.1";
	//printf("before parsing in atm\n");
	cmd_args * cmd_struct = (cmd_args *)malloc(sizeof(cmd_args));
	parse_cmds(argc, argv, cmd_struct);
	// check if mode has been passed in
	if (strlen(cmd_struct->mode) == 0 || strlen(cmd_struct->account) == 0) {
		exit(255);
	}
	//printf("after parsing in atm\n");


	if(strlen(cmd_struct->ip) == 0){
		char ip_temp [10] = "127.0.0.1";
		memcpy(cmd_struct->ip, ip_temp, strlen(ip_temp) + 1);
		cmd_struct->ip[strlen(ip_temp)+1] = '\0';
		// cmd_struct->ip = "127.0.0.1"; 
	}

	//printf("The set port for atm is %d\n", cmd_struct->port); 
	if(cmd_struct->port == NULL){
		cmd_struct->port = 3000; 
	}

	if(strlen(cmd_struct->authfile) == 0){
		char authfile_temp[128] = "bank.auth";
		memcpy(cmd_struct->authfile, authfile_temp, strlen(authfile_temp) + 1);
		cmd_struct->authfile[strlen(authfile_temp)+1] = '\0';
		// cmd_struct->authfile = "bank.auth";
	}

	if(access(cmd_struct->authfile, F_OK | R_OK) == -1){
		//printf("bad file access\n");
		exit(255); 
	}

	// FILE * auth_fp;
	// // unsigned char authfile_contents[AES_256_KEY_SIZE];

	// if (auth_fp = fopen(cmd_struct->authfile, "r")) {
	// 	printf("inside file\n");
	// 	fseek(auth_fp, 0, SEEK_END); 
	// 	int len  = ftell(auth_fp); 
	// 	rewind(auth_fp);

	// 	if(len < 48){
	// 		exit(255); 
	// 	}
	// }

	


	//printf("about to create atm with %s\n", cmd_struct->authfile);

	//printf("the card provided is %s\n", cmd_struct->card); 

	//printf("the IP is %s and PORT is %d\n", cmd_struct->ip, cmd_struct->port); 

	ATM *atm = atm_create(cmd_struct->ip, cmd_struct->port, cmd_struct->authfile);

	atm_process_command(atm, cmd_struct);
	
	//printf("about to free ATM \n");
	atm_free(atm);
	free(cmd_struct); 
	
	//We should likely do sanitizing here so we can send only what is necessary to
	//the atm_process_command() function

	// Implement how atm protocol will work: sanitizing inputs and using different modes of operations

	return EXIT_SUCCESS;
}
