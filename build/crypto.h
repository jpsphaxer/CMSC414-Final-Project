#include "bank.h"
#include "net.h"
#include "linkedlist.h"

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/rand.h>
#include <openssl/evp.h>


#ifndef __CRYPTO_H__
#define __CRYPTO_H__

int encrypt(unsigned char * message, unsigned char * key, unsigned char * iv, unsigned char * cipher); 
int decrypt(unsigned char * message, unsigned char * key, unsigned char * iv, unsigned char * cipher, int cipher_len);

#endif
