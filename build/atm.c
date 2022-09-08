#include "atm.h"
#include "net.h"
#include "parser.h"
#include "crypto.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/rand.h>
#include <sys/time.h>
#include <openssl/evp.h>

#define AES_256_KEY_SIZE 32
#define AES_BLOCK_SIZE 16

ATM* atm_create(char *ip, unsigned short port, char * authfile)
{
	ATM *atm = (ATM*) calloc(1, sizeof(ATM));
	if(atm == NULL) {
		//perror("Could not allocate ATM");
		exit(255);
	}

	struct timeval tv; 
	tv.tv_sec = 100;
	tv.tv_usec = 0;

#define BOOL_CHK(x,msg) if (x) { perror(msg); exit(255); }

	// Set up the network state
	
	BOOL_CHK(inet_pton(AF_INET, ip, &(atm->bank_addr.sin_addr)) != 1, "could not convert ip address");
	atm->bank_addr.sin_port = htons(port);
	atm->bank_addr.sin_family = AF_INET;

	atm->sockfd = socket(AF_INET, SOCK_STREAM, 0);
	BOOL_CHK(atm->sockfd < 0, "could not create socket");
	setsockopt(atm->sockfd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(tv)); 
	BOOL_CHK(connect(atm->sockfd, (struct sockaddr*)&(atm->bank_addr), sizeof(atm->bank_addr)) < 0, "could not connect");

#undef BOOL_CHK
	
	
	char buff[1000];
	unsigned char key[32];
	unsigned char iv[16];
	RAND_bytes(key, sizeof(key));
	RAND_bytes(iv, sizeof(iv));

	unsigned char cipher[1000];
	atm_send(atm, (char*) key, sizeof(key)); 
	atm_send(atm, (char*)iv, sizeof(iv)); 
	int en = encrypt(authfile, key, iv, cipher);
	//printf("our encrypted cipher is %s\n and en is %d\n", cipher, en);
	atm_send(atm, (char *) cipher, en);

	// printf("KEY:  %s\nIV:  %s\nCIPHER:  %s\n", key,iv,cipher);
	//printf("result of atm send %d\n", n);

	//atm_send(atm,authfile,sizeof(authfile)+1); 
	atm_recv(atm, buff, sizeof(buff)); 
	//printf("Is this true or false? %s\n", buff); 

	if(strcmp(buff, "false")==0){
		exit(255); 
	}
	// Set up the protocol state
	// TODO set up more, as needed
	FILE * auth_fp;
	// unsigned char authfile_contents[AES_256_KEY_SIZE];

	if ((auth_fp = fopen(authfile, "r"))) {
		//printf("inside file\n");
		fseek(auth_fp, 0, SEEK_END); 
		int len  = ftell(auth_fp); 
		rewind(auth_fp);

		// if(len < 48){
		// 	exit(255); 
		// }

		fread(atm->key, sizeof(unsigned char), AES_256_KEY_SIZE, auth_fp);
		fread(atm->iv, sizeof(unsigned char), AES_BLOCK_SIZE, auth_fp);
		//atm->key = malloc(sizeof(unsigned char) * 32); 
		//atm->key = authfile_contents;
		//printf("In atm the key saved is: %s\n", atm->key);
	} else {
		//perror("Authentication file does not exist. Closing connection.\n");
		exit(255);
	}

	// fclose(auth_fp);
	return atm;
}

void atm_free(ATM *atm){
	
	close(atm->sockfd);
	free(atm); 
// TODO
}

/* send a message to the bank of size data_len stored in the buffer
 * data. return 0 on success, negative on error */
int atm_send(ATM *atm, const char *data, size_t data_len){

	if (atm->sockfd < 0) {
		return -1;
	}

	if (send_all(atm->sockfd, (const char*)&data_len, sizeof(data_len)) != sizeof(data_len)) {
		return -2;
	}

	if (send_all(atm->sockfd, data, data_len) != (ssize_t)data_len) {
		return -3;
	}
	return 0;
}

/* receive a message (i.e., something sent via bank_send) and store it
 * in data. If the message exceeds max_data_len, a negative value is
 * returned and the message is discarded */
ssize_t atm_recv(ATM *atm, char *data, size_t max_data_len){

	size_t msg_len;

	if (atm->sockfd < 0) {
		return -1;
	}

	if (recv_all(atm->sockfd, (char*)&msg_len, sizeof(msg_len)) != sizeof(msg_len)) {
		return -2;
	}

	if (msg_len > max_data_len) {
		/* message doesn't fit in data, read all of the message to discard it */
		char tmp[4096];
		do {
			size_t to_read = msg_len > sizeof(tmp) ? sizeof(tmp) : msg_len;
			if (recv_all(atm->sockfd, tmp, to_read) != sizeof(to_read)) {
				/* logic error somewhere, should probably crash/restart */
				return -3;
			}
			msg_len -= to_read;
		} while(msg_len > 0) ;
	}

	return recv_all(atm->sockfd, data, msg_len);
}

void atm_process_command(ATM *atm, cmd_args * cmd_struct)
{
	// printf("in process command\n");
	// Implement the ATM's side of the ATM-bank protocol

	if(strcmp(cmd_struct->mode, "create") == 0){
		double amount = atof(cmd_struct->amount); 
		create_account_request(atm, cmd_struct->account, amount, cmd_struct);
	} else {
		if (authenticate_user(atm, cmd_struct) == 0) {
			if(strcmp(cmd_struct->mode, "deposit") == 0){
				double amount = atof(cmd_struct->amount); 
				deposit_amount_request(atm, cmd_struct->account, amount);
			}

			if(strcmp(cmd_struct->mode, "withdraw") == 0){
				double amount = atof(cmd_struct->amount); 
				withdraw_amount_request(atm, cmd_struct->account, amount);
			}

			if(strcmp(cmd_struct->mode, "balance") == 0){
				//double amount = atof(cmd_struct->amount);
				get_account_balance_request(atm, cmd_struct->account);
			}
		} else {
			exit(255);
		}
	}
	
	
	return;
		
	//printf("mode: %s, amount: %s\n", cmd_new->mode, cmd_new->amount);
	//printf("the key from the bank is %s\n", atm->key); 
	
}

// Add more functions as needed
void create_account_request(ATM *atm, char * account_name, double amount, cmd_args * cmd_struct) {
	//ex. ./atm -a alice -n 1500.00 
	//argv 2 would be the account name 

	unsigned char mode[] = "create";
	unsigned char message[1000];
	
	int c_acct_err;

	unsigned char IV[16];
	// add check to make sure rand_bytes worked
	RAND_bytes(IV, sizeof(IV));
	//atm->iv = IV; 
	//printf("iv before comands %s\n", IV);
	//printf("the key from the bank is %s\n", atm->key); 

	//&& account_name does not exist && card file does not exist
	if (amount >= 10.00) {
		//printf("the mode is : %s\n", mode);

		sprintf(message, "%s %s %.2f", mode, account_name, amount);
		int length = strlen(message);
		message[length] ='\0';
		// printf("account name is %s\n", account_name);
		// printf("amount is %.2f\n", amount);
		// printf("the message is %s\n", message);
		

		unsigned char cipher[1000];
		int en = encrypt(message, atm->key, IV, cipher);
		//printf("our encrypted cipher is %s\n and en is %d\n", cipher, en);

		int n = atm_send(atm,(char *) IV, sizeof(IV));
		//printf("result of atm send %d\n", n);
		atm_send(atm, (char *) cipher, en);

		atm_recv(atm,message, sizeof(message)); 
		//printf("the bank created the acoutn? %s\n", message); 
		if(strcmp(message, "true")==0){

			create_card(atm, account_name, cmd_struct);
			//"account":"55555","initial_balance":10.00
			fprintf(stdout, "{\"account\": \"%s\",\"initial_balance\": %.2f}\n", account_name, amount); 
			fflush(stdout);
		} else {
			exit(255);
		}

		//printf("sending from create account ERROR CODE # %d\n", c_acct_err); 
		//prints account and initial balance to standard output, encoded as JSON.
		//ex: account":"55555","initial_balance":10.00
	} else {

		// atm_recv(atm,message, sizeof(message)); 
		///printf("the bank created the acoutn? %s\n", message); 
		exit(255);
	}
}

void deposit_amount_request(ATM* atm, char * account_name, double amount) {

	unsigned char mode[] = "deposit";
	unsigned char message[1000]; 

	unsigned char IV[16];
	// add check to make sure rand_bytes worked
	RAND_bytes(IV, sizeof(IV));
	//atm->iv = IV; 
	//printf("iv before comands %s\n", IV);
	//printf("the key from the bank is %s\n", atm->key);

	//printf("atm.c - before if statement in deposit amount request\n");

	if (amount > 0.00) {
		sprintf(message, "%s %s %.2f", mode, account_name, amount);
		int length = strlen(message);
		message[length] = '\0';

		unsigned char cipher[1000];
		int en = encrypt(message, atm->key, IV, cipher);
		//printf("our encrypted cipher is %s\n and en is %d\n", cipher, en);

		int n = atm_send(atm,(char *) IV, sizeof(IV));
		//printf("result of atm send %d\n", n);
		atm_send(atm, (char *) cipher, en);
		// make sure the card matches
		atm_recv(atm,message, sizeof(message)); 

		
		if(strcmp(message, "true") == 0){
			//"account":"55555","deposit":10.00
			fprintf(stdout, "{\"account\": \"%s\",\"deposit\": %.2f}\n", account_name, amount); 
			fflush(stdout); 
		} else {
			exit(255);
		}
	} else {
		exit(255);
	}
}

void withdraw_amount_request(ATM *atm, char * account_name, double amount) {

	unsigned char mode[] = "withdraw"; 
	unsigned char message[1000]; 

	//printf("atm.c - before if statement in withdraw amount request\n");

	unsigned char IV[16];
	// add check to make sure rand_bytes worked
	RAND_bytes(IV, sizeof(IV));
	//atm->iv = IV; 
	////printf("iv before comands %s\n", IV);
	//printf("the key from the bank is %s\n", atm->key);

	if (amount > 0.00) {
		sprintf(message, "%s %s %.2f", mode, account_name, amount); 
		int length = strlen(message); 
		message[length] = '\0';

		unsigned char cipher[1000];
		int en = encrypt(message, atm->key, IV, cipher);
		//printf("our encrypted cipher is %s\n and en is %d\n", cipher, en);

		int n = atm_send(atm,(char *) IV, sizeof(IV));
		//printf("result of atm send %d\n", n);
		atm_send(atm, (char *) cipher, en);

		// check if account exists
		// make sure the card matches
		atm_recv(atm,message, sizeof(message)); 
		if(strcmp(message, "true") == 0){
			//"account":"55555","withdraw":10.00
			fprintf(stdout, "{\"account\": \"%s\",\"withdraw\": %.2f}\n", account_name, amount); 
			fflush(stdout);
		} else {
			//printf("exit 255 in withdraw for ATM\n");
			exit(255);
		}
	} else {
		exit(255);
	}
}

void get_account_balance_request(ATM *atm, char * account_name) {
	//return balance for the account name
	unsigned char mode[] = "balance";
	unsigned char message[1000];
	
	sprintf(message, "%s %s",mode, account_name); 
	int length = strlen(message); 
	message[length] = '\0';

	unsigned char IV[16];
	// add check to make sure rand_bytes worked
	RAND_bytes(IV, sizeof(IV));
	//atm->iv = IV; 
	//printf("iv before comands %s\n", IV);
	//printf("the key from the bank is %s\n", atm->key);
	//printf(" the message being sent from the ATM is: %s\n", message); 
	unsigned char cipher[1000];
	int en = encrypt(message, atm->key, IV, cipher);
	//printf("our encrypted cipher is %s\n and en is %d\n", cipher, en);

	int n = atm_send(atm,(char *) IV, sizeof(IV));
	//printf("result of atm send %d\n", n);
	atm_send(atm, (char *) cipher, en); 

	unsigned char iv[100];
	unsigned char buffer [1024];

	int iv_bytes = atm_recv(atm, (char *)iv, sizeof(iv));

	int nn = atm_recv(atm, buffer ,sizeof(buffer));

	unsigned new_message[1000];
	int de = decrypt(new_message, atm->key, iv, buffer, nn);
	new_message[de] = '\0';
	
	//printf("our decrypted message is: %s\n and de is %d\n", new_message, de);

	if(strcmp(new_message, "false") == 0){
		exit(255);
	} else {
		fprintf(stdout, "{\"account\": \"%s\",\"balance\": %.2f}\n", account_name, atof(new_message));
		fflush(stdout); 
	}

	//printf("ATM: %s\n", new_message);

	//IMPORTANT: EVENTRUALLY WRITE A CHECK OR ERROR 255
}

// IMPORTANT: CHANGE THIS TO TAKE IN THE CARD NAME PROVIDED IN CMD_ARGS->CARD (WHICH SHOULD ALSO TAKE CARE
//	OF DEFAULTS)
void create_card(ATM * atm, char * account_name, cmd_args * cmd_struct) {
	
	FILE *card_fp;
	char *card_name;

	// if no card name was provided, use default to make file name
	//printf("the card in create card is %ld\n",strlen(cmd_struct->card)); 
	if (strlen(cmd_struct->card) == 0) {

		char card_usr [128];
		sprintf(card_usr,"%s.card",account_name);
		memcpy(cmd_struct->card,card_usr, strlen(card_usr)+1);
		cmd_struct->card[strlen(card_usr)+1] = '\0';

	} 

	// encrypt account name 
	unsigned char cipher[1000];
	int en = encrypt(account_name, atm->key, atm->iv, cipher);

	int length = strlen(cmd_struct->card);
	//printf("creating a card for : %s with length %ld\n", cmd_struct->card, length); 

	card_fp = fopen(cmd_struct->card, "w+");
	fprintf(card_fp, cipher); 
	fclose(card_fp); 
}

int authenticate_user(ATM * atm, cmd_args * cmd_struct){

	FILE * card; 
	unsigned char cipher[1000];

	//printf("Card name: %s\n", cmd_struct->card);

	if (strlen(cmd_struct->card) == 0) {
		//printf("Made it inside null check for card\n");
		char card_usr [128];
		sprintf(card_usr,"%s.card",cmd_struct->account);
		memcpy(cmd_struct->card,card_usr, strlen(card_usr)+1);
		cmd_struct->card[strlen(card_usr)+1] = '\0';
		//("Card name: %s\n", cmd_struct->card);

	} 
	// else{
	// }
	if(access (cmd_struct->card, F_OK) != -1){
		
	} else {
		atm_send(atm, " ", 1);
		exit(255);

	}
	// if(access (cmd_struct->card, F_OK) == -1){
	// 	atm_send(atm, " ", 1);
	// 	exit(255);
	// } 

	card = fopen(cmd_struct->card, "r");

	if (card == NULL){
		exit(255);
	}

	fseek(card, 0, SEEK_END); 
	int len  = ftell(card); 
	rewind(card);

	int en = fread(cipher, sizeof(unsigned char), len, card);

	char card_user[128];
	int de = decrypt(card_user, atm->key, atm->iv, cipher, en);
	card_user[de] = '\0';
	//printf("our decrypted message is %s\n ", card_user);

	int validation = strcmp(card_user, cmd_struct->account); 

	fclose(card);

	if(validation  == 0){
		return 0; 
	} else{
		atm_send(atm, " ", 1);
		return 1; 
		//exit(255); 
	}
}