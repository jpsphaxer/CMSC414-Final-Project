#include <stdio.h>
#include <string.h> 
#include <stdbool.h>
#include <stdlib.h>


#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__



typedef struct _user_node
{
    //int account_num; 
    char * account_name; 
    void * balance;
    struct _user_node * next; 

}user_node;

//struct user_node *head = NULL; 
//struct user_node *tail = NULL; 


typedef struct _user_list{

    user_node * head; 
    user_node * tail;
    int size;  

}user_list; 

user_list * create_list();
void free_list(user_list * users); 
void printList(user_list * users); //might have to change later
void add_user_to_list(user_list * users, char * account_names, void * balances);
void * search_for_user(user_list * users, char * account_names); 
bool is_empty(user_list * users); //if head is null
void modify_amount(user_list * users, char * account_names, void * amount); 

#endif
