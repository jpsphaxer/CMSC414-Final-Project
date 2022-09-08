#include <stdio.h>
#include <string.h> 
#include <stdbool.h>
#include <regex.h>
#include <stdlib.h>

#ifndef __REGEXCHECK_H__
#define __REGEXCHECK_H__

regex_t compile_valid_filename_regex();
regex_t compile_valid_nums_regex();
regex_t compile_valid_amount_regex();
void check_case_a(regex_t file_reg, char *account);
void check_case_s(regex_t file_reg, char *authfile);
void check_case_i(char *ip);
void check_case_p(regex_t nums_reg, char *port);
void check_case_c(regex_t file_reg, char *card);
void check_case_n(regex_t amount_reg, char *balance);
void check_case_d_w(regex_t amount_reg, char *amount);

#endif