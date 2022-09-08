#include "linkedlist.h"
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <stdbool.h>

user_list * create_list() {

    user_list * users = (struct user_list*) malloc(sizeof(user_list)); 
    users->head = NULL;
    users->size = 0;
    return users;
}

//might need to revise based on tail node.
void free_list(user_list * users) {
    user_node* tmp;

    while (users->head != NULL) {
        tmp = users->head;
        users->head = users->head->next;
        //free(tmp->account_name); 
        //free(tmp->balance); 
        free(tmp);
    }
    free(users);
}

void printList(user_list * users) {
    user_node *ptr = users->head;
    //printf("The list size is %d\n", users->size);
    //printf("\n[ ");

    while (ptr != NULL) {
       // printf("(%s, %s)\n", ptr->account_name, ptr->balance);
        ptr = ptr->next;
    }

    //printf(" ]\n");
}

void add_user_to_list(user_list * users, char * account_names, void * balances) {
    //printf("adding user function\n");
    user_node *user = (user_node *) malloc(sizeof(user_node));
    user_node *current;
    
    user-> account_name = account_names; 
    user->balance = balances;
    user->next = NULL;
    current = users->head;

    if(users->size == 0){
        users->head = user;
    } else {
        while(current->next != NULL) {
            current = current->next;
        }
        current->next = user; 
    }

    //free(user); 
    
    users->size += 1; 
}


//might need to return user node
void* search_for_user(user_list * users, char * account_names) {
    if(users == NULL) {
        //printf("empty list\n");
        return NULL; 
    }
    
    //printf("account_names in search user is %s\n", account_names); 
    user_node * temp = users->head;
    //printf("the user in temp in search is %s\n", temp->account_name); 

    while(temp != NULL) {
       // printf("account name in list: %s\n", temp->account_name);
    
        if(strcmp (account_names, temp->account_name) == 0) {
            //printf("provided account name: %s\n", account_names);
            //printf("found a match with account name: %s and balance: %p\n", temp->account_name, temp->balance);
            return temp->balance; 
        }
        temp = temp->next; 
    }

    return NULL;
} 

bool is_empty(user_list * users) {

    if(users->size == 0) {
        return true; 
    } else {
        return false;
    }
} 

void modify_amount(user_list * users, char * account_names, void * amount) {
    
    //printf("YAY! we made to modify amount!\n");
    
    if(users == NULL) {
        return NULL;
    }
    
    user_node * temp = users->head; 

    while(temp != NULL) {
        if(strcmp(account_names, temp->account_name) == 0){
            //printf("Made it into strcmp, about to set the balance to the new amount\n");
            //Might need to free old balance memory?
            temp->balance = amount;
        }

        temp = temp->next; 
    }

    return; 
}
