#include "bank.h"
#include "net.h"
#include "crypto.h"
#include "linkedlist.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <openssl/rand.h>
#include <openssl/evp.h>

#define AES_256_KEY_SIZE 32
#define AES_BLOCK_SIZE 16


Bank* bank_create(char *auth_file, char *ip, unsigned short port)
{

    Bank *bank = (Bank*) calloc(1, sizeof(Bank));
	if(bank == NULL) {
		//perror("Could not allocate Bank");
		exit(255);
	}

	struct timeval tv; 
	tv.tv_sec = 100; 
	tv.tv_usec = 0; 

#define BOOL_CHK(x,msg) if (x) { perror(msg); exit(255); }
	//place print statement here with flush (TEST)
	//
	/* setup network connection */
	BOOL_CHK(inet_pton(AF_INET, ip, &(bank->bank_addr.sin_addr)) != 1, "could not convert ip address");

	bank->bank_addr.sin_port = htons(port);
	bank->bank_addr.sin_family = AF_INET;

	int s = socket(AF_INET, SOCK_STREAM, 0); //af_inet ipv4 - sock_stream tcp - 
	BOOL_CHK(s<0,"could not create socket");

	int enable = 1;
	BOOL_CHK(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0, "setsockopt(SO_REUSEADDR) failed");

	setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(tv)); 

	BOOL_CHK(bind(s, (struct sockaddr*)&(bank->bank_addr), sizeof(bank->bank_addr)) < 0, "could not bind");

	listen(s, 5);

	bank->sockfd = s;

#undef BOOL_CHK

	// Set up the protocol state
	// TODO set up more, as needed


	
	FILE *auth_fp;
	int num_key = -1; 
	int num_iv = -1;
	

	if (access(auth_file, F_OK) == 0) {
		//File already exists
		//fprintf(stderr, "Authentication file already exists. Closing bank.\n");
		exit(255);
	} else {
		//File doesn't exist; create file in read/write mode (is reading necessary?)
		auth_fp = fopen(auth_file, "w");
		

		unsigned char temp_key [AES_256_KEY_SIZE]; 
		unsigned char temp_iv [AES_BLOCK_SIZE]; 
		// unsigned char keycpy [AES_256_KEY_SIZE];
			
		//write random bytes to buffer
		if (!RAND_bytes(temp_key, sizeof(temp_key))) {
			exit(255);
			//fprintf(stderr, "Could not write random bytes to auth file\n");
			//Should it exit too? Probably not, right?
		}

		if (!RAND_bytes(temp_iv, sizeof(temp_iv))) {
			exit(255);
		}

	
		memcpy(bank->key, temp_key, AES_256_KEY_SIZE);
		memcpy(bank->iv, temp_iv, AES_BLOCK_SIZE);
	
	
		num_key = fwrite(bank->key, sizeof(unsigned char), AES_256_KEY_SIZE, auth_fp);
		num_iv = fwrite(bank->iv, sizeof(unsigned char), AES_BLOCK_SIZE, auth_fp);
	

		fclose(auth_fp);
		
		memcpy(bank->auth_file, auth_file, strlen(auth_file)+1);
		bank->auth_file[strlen(auth_file) + 1] = '/0';

		printf("created\n");
		fflush(stdout); 
	}

	bank->users = create_list(); 
	return bank;
}

void bank_free(Bank *bank){
	if (bank != NULL) {
		close(bank->sockfd);
		free_list(bank->users);
		free(bank);
	}
	// TODO
}

/* sends data_len bytes from data to atm, returns size 0 on success, negative on failure */
int bank_send(Bank *bank, const char *data, size_t data_len) {
	if (bank->clientfd < 0) {
		return -1;
	}

	if (send_all(bank->clientfd, (const char*)&data_len, sizeof(data_len)) != sizeof(data_len)) {
		return -2;
	}

	if (send_all(bank->clientfd, data, data_len) != (ssize_t)data_len) {
		return -3;
	}
	return 0;
}

/* receive a message (i.e., something sent via atm_send) and store it
 * in data. If the message exceeds max_data_len, a negative value is
 * returned and the message is discarded */
ssize_t bank_recv(Bank *bank, char *data, size_t max_data_len) {

	size_t msg_len;
	//size_t second_if; 
	//printf("bank.c - bank_recv - data array passed in is %s\n", data);
	//printf("bank.c - bank_recv - the bank client fd # %d\n",bank->clientfd);
	if (bank->clientfd < 0) {
		return -1;
	}

	if (recv_all(bank->clientfd, (char*)&msg_len, sizeof(msg_len)) != sizeof(msg_len)) {
		return -2;
	}
	//printf("bank.c - bank_recv - second if value is: %ld, and sizerof(msg_len) is %ld ", second_if, sizeof(msg_len));

	//printf("msg_len %s \n", (char *)&msg_len); 
	if (msg_len > max_data_len) {
		/* message doesn't fit in data, read all of the message to discard it */
		char tmp[4096];
		do {
			size_t to_read = msg_len > sizeof(tmp) ? sizeof(tmp) : msg_len;
			//printf("In bank_receive, to_read has a size of %ld\n",to_read);
			if (recv_all(bank->clientfd, tmp, to_read) != sizeof(to_read)) {
				/* logic error somewhere, should probably crash/restart */
				return -3;
			}
			msg_len -= to_read;
		} while(msg_len > 0) ;
	}
	return recv_all(bank->clientfd, data, msg_len);
}

void bank_process_remote_command(Bank *bank) {
	// TODO: Implement the bank side of the ATM-bank protocol
	//printf("%s\n", "Entered bank_process_remote_command");

	unsigned char buffer [1024];
	unsigned char iv[100];


	int iv_bytes = bank_recv(bank, iv, 100);
	//printf("Bank received iv: %s\n  with bytes %d\n", iv, iv_bytes); 

	if (iv_bytes != 1) {
		int n = bank_recv(bank, buffer, sizeof(buffer));
		//printf("the message to decrypt is %s\n", buffer); 
	unsigned char new_message[1000];
	int de = decrypt(new_message, bank->key, iv, buffer, n);
	new_message[de] = '\0';
	// unsigned char * final = malloc(sizeof(unsigned char) * de); 
	// strncpy(final, new_message, de);
	// final[de] = '\0';
	//printf("our decrypted message is: %s\n and de is %d\n", final, de);
	//printf("command is %s\n", buffer);
	// printf("bank recieved: %s num bytes %d\n", new_message, n); 

	char * mode;
	char * account_name;
	char * amount;

	mode = strtok(new_message, " ");
	//printf("The strlen of mode is %ld The mode is: %s\n", strlen(mode), mode); 
	if(strcmp(mode, "create") == 0 || strcmp(mode, "deposit")== 0 || strcmp(mode, "withdraw")==0){
		account_name = strtok(NULL, " ");
		//printf("(1) the strlen of account_name is %ld The account_name is: %s\n",strlen(account_name), account_name); 

		amount = strtok(NULL, " ");
		//printf("The amount is: %s\n", amount); 

	} else if(strcmp(mode, "balance") == 0){

		//printf("inside blance if statement about to process in bank;\n");
		account_name = strtok(NULL, " ");
		// printf("bank.c - The account name is: %s\n", account_name);
		get_balance(bank, account_name); 

	}

	// double amt = atof(amount); 

	if(strcmp(mode, "create") == 0) {
		//printf("about to create account\n");
		create_account(bank, account_name, amount);
	}

	if(strcmp(mode, "deposit") == 0) {
		//printf("About to make a deposit\n");
		deposit_amount(bank, account_name, amount); 
	}

	if(strcmp(mode, "withdraw") == 0){
		//printf("about to make a withdrawal\n");
		withdraw_amount(bank, account_name, amount); 
	}

	// char msg[6];
	// sprintf(msg, "%s", "true");
    // bank_send(bank, msg, strlen(msg));
	// fflush(stdout);
	}
}

void create_account(Bank * bank, char * account_name, char * amount) {
	//printf("Inside create_account method\n");
	void * balance = NULL; 	

	balance = (search_for_user(bank->users, account_name));
	//printf("balance double %p\n", balance);
	
	if (balance == NULL) {
		//printf("Inside if statesment in create account\n");

		char * calloc_amount = calloc(11, 1);
		sprintf(calloc_amount, "%s", amount);
		//printf("sprintf worked in create_account\n");

		char * calloc_account_name = calloc(strlen(account_name) + 1, 1);
		strncpy(calloc_account_name, account_name, strlen(account_name));
		
		//printf("Successfully calloc'd space for amount and account name\n");

		add_user_to_list(bank->users, calloc_account_name, calloc_amount);
		balance = NULL; 
		calloc_amount = NULL;
		calloc_account_name = NULL;

		// JSON formatin
		// printf("{\"account name\": %s, \"initial balance\": %0.2f}\n",
		// account_name, amount);
		
		// printList(bank->users);
		/*************IMPORTANT**************/
		/***FREE CALLOC!!!!!!!!!!!!!*AND MAKE CARD*******/
		fprintf(stdout, "{\"account\": \"%s\",\"initial_balance\": %s}\n", account_name, amount);
		fflush(stdout); 

		free(calloc_account_name);
		free(calloc_amount); 
		bank_send(bank, "true", 5);
	} else {
		bank_send(bank, "false", 6);
	}
}

void deposit_amount(Bank * bank, char * account_name, char * amount) {
	//printf("in dep amnt\n");
	void * balance = search_for_user(bank->users, account_name);
	
	//printf("The new amount is %f\n", new_amount);

	//char * namount_str; 

	//printf("we are in deposit amount method\n");

	if (balance != NULL) {
		char * old_amount = (char *) balance;
		//printf("The old amount before converting to double is %s\n", old_amount);
		
		double o_amount = atof(old_amount);
		//printf("The old amount is %f\n", o_amount);
		
		//printf("The amount to add is %f\n", atof(amount));
		
		double new_amount = o_amount + atof(amount);
		//printf("we are now in the if statement before we go and modify amount\n");
		char * namount_str = calloc(11,1); 
		//printf("after we calloc \n");
		sprintf(namount_str, "%f", new_amount); 
		//printf("The new amount %s \n", namount_str); 
		
		modify_amount(bank->users, account_name, namount_str); 
		fprintf(stdout, "{\"account\": \"%s\",\"deposit\": %s}\n", account_name, amount);
		fflush(stdout);
		//free(namount_str); 
		// printList(bank->users);
		bank_send(bank, "true", 5);
	} else {
		bank_send(bank, "false", 6);
	}
}

void withdraw_amount(Bank * bank, char * account_name, char * amount) {
	
	void * balance = search_for_user(bank->users, account_name);
	char * old_amount = (char *) balance;
	//printf("The old amount before converting to double is %s\n", old_amount);
	
	double o_amount = atof(old_amount);
	//printf("The old amount is %f\n", o_amount);
	
	//printf("The amount to add is %f\n", atof(amount));
	
	double new_amount = o_amount - atof(amount);
	//printf("The new amount is %f\n", new_amount);

	//char * namount_str;

	//printf("we are in withdraw amount method\n");
	if (balance != NULL) {

		//printf("we are now in the if statement before we go and modify amount\n");
		char * namount_str = calloc(11,1); 
		//printf("after we calloc \n");
		sprintf(namount_str, "%f", new_amount); 
		//printf("The new amount %s \n", namount_str); 
		
		//Need to make sure it doesn't become negative

		if (new_amount < 0) {
			// change to stderr later
			//printf("Withdrawal would make account balance negative. Transaction refused.\n");
			bank_send(bank, "false", 6);
		} else {
			//printf("balance double %p\n", balance);
			modify_amount(bank->users, account_name, namount_str); 
			fprintf(stdout, "{\"account\": \"%s\",\"withdraw\": %s}\n", account_name, amount);
			fflush(stdout);
			//free(namount_str); 
			//printList(bank->users);
			bank_send(bank, "true", 5);
		}
	} else {
		bank_send(bank, "false", 6);
	}
}

void get_balance(Bank * bank, char * account_name) {

	char *balance = search_for_user(bank->users, account_name);
	char * balance_str = (char* ) balance; 
	//printf("get_balance function - the balance is: %s", balance_str);

	unsigned char message[1000];
	
	unsigned char IV[16];
	// add check to make sure rand_bytes worked
	RAND_bytes(IV, sizeof(IV));
	//atm->iv = IV; 
	//printf("iv before comands %s\n", IV);

	if (balance != NULL) {
		sprintf(message, "%s", balance_str);
		int length = strlen(message); 
		message[length] = '\0';

		unsigned char cipher[1000];
		int en = encrypt(message, bank->key, IV, cipher);
		//printf("our encrypted cipher is %s\n and en is %d\n", cipher, en);

		int n = bank_send(bank, (char *) IV, sizeof(IV));
		//printf("result of atm send %d\n", n);
		bank_send(bank, (char *) cipher, en); 

		
		fprintf(stdout, "{\"account\": \"%s\",\"balance\": %.2f}\n", account_name, atof(balance));
		fflush(stdout);
		// bank_send(bank, message, sizeof(message)); 

	} else {
		bank_send(bank, "false", 6);
	}
}