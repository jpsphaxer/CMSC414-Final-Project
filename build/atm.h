/*
 * The ATM interfaces with the user.  User commands should be
 * handled by atm_process_command.
 *
 * The ATM can read .card files, but not .pin files.
 *
 * You can add more functions as needed.
 */

#ifndef __ATM_H__
#define __ATM_H__

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "linkedlist.h"
#include "parser.h"
#include <stdio.h>

typedef struct _ATM {
	// Networking state
	int sockfd;
	struct sockaddr_in bank_addr;

	char authfile [128]; 
	unsigned char key [32];
	unsigned char iv [16]; 

	// Protocol state
	// TODO add more, as needed
} ATM;

ATM* atm_create(char *ip, unsigned short port, char* authfile);
void atm_free(ATM *atm);
int atm_send(ATM *atm, const char *data, size_t data_len);
ssize_t atm_recv(ATM *atm, char *data, size_t max_data_len);
void atm_process_command(ATM *atm,cmd_args * cmd_struct); //NOTE: bfore it was a single pointer
int authenticate_user(ATM * atm, cmd_args * cmd_struct);
int authenticate_user(ATM * atm, cmd_args * cmd_struct);
void create_card(ATM * atm, char * account_name, cmd_args * cmd_struct);
void get_account_balance_request(ATM *atm, char * account_name);
void withdraw_amount_request(ATM *atm, char * account_name, double amount);
void deposit_amount_request(ATM* atm, char * account_name, double amount);
void create_account_request(ATM *atm, char * account_name, double amount, cmd_args * cmd_struct);

#endif



//123 <special message>  -> atm -> atm check for integrity  