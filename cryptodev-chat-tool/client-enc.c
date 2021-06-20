#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "bscenc.h"
#include <fcntl.h>
#include <sys/stat.h>


int main(int argc, char *argv[]) {

    int i=-1, fd, client_s, ret_val;
    unsigned char iv[BLOCK_SIZE] = {0x2f,0x93,0x10,0xbf,0xa5,0x8d,0xf2,0x22,0x74,0xba,0xc7,0xb4,0x63,0x23,0x7,0xd3};
    unsigned char key[KEY_SIZE] = {0x81,0xce,0x96,0x3d,0x42,0xd5,0x4d,0x2d,0xa7,0x7d,0xfe,0x9d,0x5d,0x92,0x71,0x32} ;
    

    if (argc != 3) {
        printf("Plese enter 2 arguments, hostname and port\n");
        return -1;
    }
    uint16_t port;
    fd_set rd_fds;
    struct timeval tv; 
    if ((port = atoi(argv[2])) == 0) {
            printf("Please enter a valid port number\n");
            return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    //ensure network byte-(big endian form)
    addr.sin_port = htons(port);
    if ((client_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() failed");
        return -1;
    }
    if (inet_pton(AF_INET, argv[1], &addr.sin_addr.s_addr) <= 0) {
        printf("Oops inet_pton failed\n");
        return -1;
    }

    if (connect(client_s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect() failed");
        return -1;
    }

    char msg[DATA_SIZE] = "Hello, im client!";
    unsigned char buf[DATA_SIZE];
    memset(msg, 0, sizeof(msg));
    memset(buf, 0, sizeof(buf));

    unsigned char end_msg[4] = "hhi";
    unsigned char *encrypted_msg, *decrypted_msg;
    fd = open("/dev/crypto", O_RDWR);
    if (fd < 0) {
        perror("open dev/crypto");
    }

    while (memcmp(end_msg, "bye",sizeof(end_msg))) {
        FD_ZERO(&rd_fds);
        FD_SET(STDIN_FILENO, &rd_fds);
        FD_SET(client_s, &rd_fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        ret_val = select(client_s + 1, &rd_fds, NULL, NULL, &tv); 
        memset(buf, 0, strlen(buf));
        memset(msg, 0, strlen(msg));




        if (ret_val == -1)
            perror("Select()");
        else if (ret_val) {
                if (FD_ISSET(STDIN_FILENO, &rd_fds) != 0) {
                memset(msg, 0, strlen(msg));
                if (read(STDIN_FILENO, msg, sizeof(msg)) < 0) {
                    perror("stdin read()");
                    exit(EXIT_FAILURE);
                }
                encrypted_msg = test_encrypt(fd, key, iv, msg);
                if(!encrypted_msg) {
                    printf("Encryption failed\n");
                    exit(EXIT_FAILURE);
                }
                send(client_s, encrypted_msg, DATA_SIZE, 0);
                free(encrypted_msg);
            } else if (FD_ISSET(client_s, &rd_fds) != 0) {
                recv(client_s, buf, DATA_SIZE, 0);
                decrypted_msg = test_decrypt(fd, key, iv, buf);
                if(!decrypted_msg) {
                    printf("Decryption failed\n");
                    exit(EXIT_FAILURE);
                }
                printf("-%s\n", decrypted_msg);
                memcpy(end_msg, decrypted_msg, sizeof(end_msg));
                free(decrypted_msg);            //free after compare
            }
      }
	}
    close(client_s);
    return 0;



}

