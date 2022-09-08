#include "bank.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <openssl/rand.h>
#include <openssl/evp.h>

// Default port and ip address are defined here

int main(int argc, char** argv){
	
	//unsigned short port = 3000;
	char *ip = "127.0.0.1";

	if (argc >= 1 && argc <= 5) {
		cmd_args * cmd_struct = (cmd_args *)malloc(sizeof(cmd_args));
		parse_cmds(argc, argv, cmd_struct);
		
	
		// unsigned short port = cmd_struct->port;
		// char * auth_file_name = cmd_struct->authfile;

		//If port is null, use default
		if (cmd_struct->port == NULL) {
			cmd_struct->port = 3000;
		}

		//If authfile name is null, use default
		
		if (strlen(cmd_struct->authfile) == 0) {
			// auth_file_name = "bank.auth";
			char authfile_temp[128] = "bank.auth";
			
			memcpy(cmd_struct->authfile, authfile_temp, strlen(authfile_temp) + 1);
			cmd_struct->authfile[strlen(authfile_temp)+1] = '\0';
		}

		/* no error checking is done on any of this. may need to modify this */
		// Bank *b = bank_create("an auth file", ip, port);
		
		Bank *b = bank_create(cmd_struct->authfile, ip, cmd_struct->port);
		fflush(stdout);
		
		struct timeval to; 



		/* process each incoming client, one at a time, until complete */
		for(;;) {

			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(0, &fds);
			FD_SET(b->sockfd, &fds);
			to.tv_sec = 5;
			to.tv_usec = 0; 

			select(b->sockfd+1, &fds, NULL,NULL,NULL);

			if(FD_ISSET(b->sockfd,&fds)){

			
			unsigned int len = sizeof(b->remote_addr);
			b->clientfd = accept(b->sockfd, (struct sockaddr*)&b->remote_addr, &len);
			if (b->clientfd < 0) {
				perror("error on accept call");
				exit(255);
			}

			/* okay, connected to bank/atm. Send/recv messages to/from the bank/atm. */
			unsigned char iv[1000];
			unsigned char key[1000];
			unsigned char auth[1000];
			bank_recv(b, key,sizeof(key)); 
			bank_recv(b, iv,sizeof(iv)); 
			int n = bank_recv(b, auth,sizeof(auth)); 
			//printf("KEY:  %s\nIV:  %s\nCIPHER:  %s\n", key,iv,auth);

			unsigned char new_message[1000];
			int de = decrypt(new_message, key, iv, auth, n);
			new_message[de] = '\0';
			//printf("the auth file received is: %s\n", new_message); 
			//printf("The authfile received is: %s\n",auth);

			if(strcmp(new_message, b->auth_file) == 0){
				bank_send(b, "true", 5);
				bank_process_remote_command(b);
				
			} else {
				bank_send(b, "false", 6);
			}
			
			// printf("bank-main.c - the buffer is %s\n", buffer);
			fflush(stdout);
			}
			/* when finished processing commands ...*/
			close(b->clientfd);
			b->clientfd = -1;
		}


		bank_free(b);
		free(cmd_struct); 
		// Implement how atm protocol will work: sanitizing inputs and using different modes of operations

		return EXIT_SUCCESS;
	} else {
		//Can't start the bank properly. Probably wouldn't hurt to exit here;
		//it hasn't even started yet.
		// perror("Incorrect number of arguments for starting bank. Exiting.\n");
		exit(255);
	}
}