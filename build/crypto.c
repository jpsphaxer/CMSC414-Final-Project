#include "bank.h"
#include "net.h"
#include "linkedlist.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <openssl/ssl.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <stdlib.h>

int encrypt(unsigned char * message, unsigned char * key, unsigned char * iv, unsigned char * cipher) {
    int ciphertext_len;
    int len;
    EVP_CIPHER_CTX *ctx;

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        //fprintf(stderr, "Could not create cipher context\n");
        exit(255);
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        //fprintf(stderr, "Error initializing encryption operation\n");
        exit(255);
    }
    
    if (1 != EVP_EncryptUpdate(ctx, cipher, &len, message, strlen((const char *) message)+1)) {
        //fprintf(stderr, "Error while encrypting\n");
        exit(255);
    }
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, cipher + len, &len)) {
        //fprintf(stderr, "Error while finalizing\n");
        exit(255);
    }

    //printf("the len in encrypt is %d\n", len); 
        ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(unsigned char * message, unsigned char * key, unsigned char * iv, unsigned char * cipher, int cipher_len){

    int len;
    int plaintext_len; 

    EVP_CIPHER_CTX *ctx;
   // printf("creat cypher context\n");
    if(!(ctx = EVP_CIPHER_CTX_new())){
        exit(255);
    }
   // printf("bofer decyrpt init\n");
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)){
        exit(255);
    }
    //printf("before decrypt update\n");
    if( 1 != EVP_DecryptUpdate(ctx, message, &len, cipher, cipher_len )){
        exit(255);
    }
    plaintext_len = len; 
    //printf("Before decrypt final\n");
    if( 1 != EVP_DecryptFinal_ex(ctx, message + len, &len)){
        //printf("YUP failed in decrypt final\n");
        exit(255);
    }

    //message[plaintext_len] = '\0';

    plaintext_len += len; 

    EVP_CIPHER_CTX_free(ctx);
    //printf("DONE drypting\n");

    return plaintext_len; 

}