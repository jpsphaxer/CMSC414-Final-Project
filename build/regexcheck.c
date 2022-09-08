#include "regexcheck.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <regex.h>

regex_t compile_valid_filename_regex() {
    regex_t file_reg;
    if (regcomp(&file_reg, "^([_\\-\\.0-9a-z]*)$", REG_EXTENDED) != 0) {
        exit(255);
    }
    return file_reg;
}

regex_t compile_valid_nums_regex() {
    regex_t nums_reg;
    if (regcomp(&nums_reg, "^(0|[1-9][0-9]*)$", REG_EXTENDED) != 0) {
        exit(255);
    }
    return nums_reg;
}

regex_t compile_valid_amount_regex() {
    regex_t amount_reg;
    if (regcomp(&amount_reg, "^(0|[1-9]+[0-9]*)(\\.[0-9]{2})$", REG_EXTENDED) != 0) {
        exit(255);
    }
    //printf("returning amount_reg\n");
    return amount_reg;
}

void check_case_a(regex_t file_reg, char *account) {
    if ((regexec(&file_reg, account, 0, NULL, 0) != 0) || 
    (strlen(account) < 1) || (strlen(account) > 122)) {
        exit(255);
    }
}

void check_case_s(regex_t file_reg, char *auth_file) {
    if ((regexec(&file_reg, auth_file, 0, NULL, 0) != 0) || 
        strlen(auth_file) < 1 || strlen(auth_file) > 127 ||
        (strcmp(".", auth_file) == 0) || 
        (strcmp("..", auth_file) == 0)) {
        exit(255);
    }
}

void check_case_i(char *ip) {
    // regex_t ip_reg;
    int sect_count = 0;
    char ip_check [strlen(ip)];
    strncpy(ip_check, ip, strlen(ip));
    char * ip_sect = strtok(ip_check, ".");
   // printf("ip sect is %s\n",ip_sect); 
    // sect_count++;
    while (ip_sect != NULL) {
        //printf("in while loop\n");
        sect_count++;
        if (atoi(ip_sect) > 255 || atoi(ip_sect) < 0) {
            //printf("invalid ip address\n");
            exit(255);
        }
        ip_sect = strtok(NULL, ".");
        //printf("ip sect is %s\n",ip_sect); 
    }

    //printf("after while loop in check case i \n");
   if (sect_count != 4) {
        exit(255);
    }
   //printf("ip check was a success\n");
} 

void check_case_p(regex_t nums_reg, char *port) {
    if (regexec(&nums_reg, port, 0, NULL, 0) != 0 || atoi(port) < 1024 || atoi(port) > 65535) {
        exit(255);
    }
}

void check_case_c(regex_t file_reg, char *card) {
    // printf("regex checking card\n");
    if ((regexec(&file_reg, card, 0, NULL, 0) != 0) ||
     (strlen(card) < 1) || (strlen(card) > 127)) {
        // printf("regex failed for card\n");
        exit(255);
    }
}

//Maybe in check_case_n we'd have to check account against a new regex (. and .. are allowed), 
//and balance against amount_reg (as we are already doing)
void check_case_n(regex_t amount_reg, char *balance) {
    if ((regexec(&amount_reg, balance, 0, NULL, 0) != 0) || (atof(balance)< 10.00) || (atof(balance) > 4294967295.99)){
        //printf("regexexec failed n\n");
        exit(255);
    }
    
    //printf("returning from check case n\n");
}

void check_case_d_w(regex_t amount_reg, char *amount) {
    if ((regexec(&amount_reg, amount, 0, NULL, 0) != 0) || (atof(amount) < 0) || (atof(amount) > 4294967295.99)){
       // printf("regexexec failed d_w\n");
        exit(255);
    }
}