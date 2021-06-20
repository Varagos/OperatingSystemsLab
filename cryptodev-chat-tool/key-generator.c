#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "bscenc.h"


int main(int argc, char *argv[]) {

    int i=-1;
    unsigned char iv[BLOCK_SIZE] = ;
    unsigned char key[KEY_SIZE] = } ;
	if (fill_urandom_buf(iv, BLOCK_SIZE) < 0) {    //16
		perror("getting data from /dev/urandom\n");
		return 1;
	}
    printf("Iv(0x):{");
    for (i=0; i<BLOCK_SIZE; i++) {
        printf("0x%x,",iv[i]);
    }
    printf("\n");

	if (fill_urandom_buf(key, KEY_SIZE) < 0) {     //16
		perror("getting data from /dev/urandom\n");
		return 1;
	}

    printf("key(0x):{");
    for (i=0; i<KEY_SIZE; i++) {
        printf("0x%x,",key[i]);
    }
    printf("\n");


}
