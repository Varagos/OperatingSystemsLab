###################################################
#
# Makefile
# Simple example for cryptodev-linux
#
# Markos Girgis <mg.markosgirgis@gmail.com>
# Stefanos Gerangelos <sgerag@cslab.ece.ntua.gr>
# Vangelis Koukis <vkoukis@cslab.ece.ntua.gr>
#
###################################################

CC = gcc

CRYPTODEVDIR=$(HOME)/cryptodev/cryptodev-linux-1.9

CFLAGS = -Wall -I$(CRYPTODEVDIR)
CFLAGS += -g
# CFLAGS += -O2 -fomit-frame-pointer -finline-functions

LIBS = 

BINS = crypto-test server-enc client-enc

all: $(BINS)

crypto-test: crypto-test.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

server-enc: server-enc.o crypto-func.o
	$(CC) -o server-enc server-enc.o crypto-func.o

server-enc.o: server-enc.c bscenc.h
	$(CC) -c server-enc.c

crypto-func.o: crypto-func.c bscenc.h
	$(CC) -c crypto-func.c


client-enc: client-enc.c bscenc.h
	$(CC) -o client-enc client-enc.c crypto-func.c
clean:
	rm -f *.o *~ $(BINS)
