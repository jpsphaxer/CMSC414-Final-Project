#include <stdio.h>
#include <string.h> 
#include <stdbool.h>
#include <stdlib.h>
#include "regexcheck.h"

#ifndef __PARSER_H__
#define __PARSER_H__


typedef struct _cmd_args{
    char mode[10]; 
    char account[123]; 
    char authfile[128]; 
    char ip[17]; 
    unsigned short port; 
    char card[128];
    char balance[14]; 
    char amount[14];

}cmd_args;

cmd_args * parse_cmds(int argc, char ** argv, cmd_args* cmd_struct); 

#endif