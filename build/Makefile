CC=gcc
CFLAGS=-g -fno-stack-protector -Wall -Wextra 
LDFLAGS=-lcrypto -lssl


all: atm bank 

atm: atm-main.c atm.c linkedlist.c parser.c regexcheck.c crypto.c net.c
	${CC} ${CFLAGS} atm.c atm-main.c linkedlist.c parser.c regexcheck.c crypto.c net.c -o atm ${LDFLAGS}

bank: bank-main.c bank.c linkedlist.c parser.c regexcheck.c crypto.c net.c
	${CC} ${CFLAGS} bank.c bank-main.c linkedlist.c parser.c regexcheck.c crypto.c net.c -o bank ${LDFLAGS}

clean:
	rm -f atm bank linkedlist parser regexcheck crypto *.o *.auth *.card
