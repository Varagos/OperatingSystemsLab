#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netinet/in.h> 
#include <unistd.h>
#include "bscenc.h"
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{


    unsigned char iv[BLOCK_SIZE] = {0x2f,0x93,0x10,0xbf,0xa5,0x8d,0xf2,0x22,0x74,0xba,0xc7,0xb4,0x63,0x23,0x7,0xd3};
    unsigned char key[KEY_SIZE] = {0x81,0xce,0x96,0x3d,0x42,0xd5,0x4d,0x2d,0xa7,0x7d,0xfe,0x9d,0x5d,0x92,0x71,0x32} ;


    if (argc != 2) {
        printf("Please give 1 argument, Port");
        exit(EXIT_FAILURE);
    }
    uint16_t port;
    int server_s, com_socket, fd, ret_val;
    struct sockaddr_in addr;
    fd_set rd_fds;
    struct timeval tv;

    if ((port=atoi(argv[1])) == 0) {
        printf("Please enter a valid port number\n");
        exit(EXIT_FAILURE);
    }

    if ((server_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_s, (struct sockaddr *)&addr,sizeof(addr)) < 0) {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_s, 1) < 0) {
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }
    int addr_len = sizeof(addr);
    printf("Server waiting on port %hu...\n", port);
    if ((com_socket = accept(server_s, (struct sockaddr *)&addr, (socklen_t*) &addr_len)) < 0 ) {
        perror("accept() failed");
        exit(EXIT_FAILURE);
    }
    printf("Server has accepted client\n");
    char msg[DATA_SIZE] = {0};
    unsigned char buf[DATA_SIZE] = {0};
    memset(msg, 0, sizeof(msg));
    memset(buf, 0, sizeof(buf));

    printf("Accepting messages up to %d bytes long :=)\n", DATA_SIZE);
    ssize_t received;
    unsigned char *encrypted_msg, *decrypted_msg;


    fd = open("/dev/crypto", O_RDWR);                                            
    if (fd < 0) {                                                               
        perror("open dev/crypto");                                              
    }   

    while(1) {
        FD_ZERO(&rd_fds);                                                       
        FD_SET(STDIN_FILENO, &rd_fds);                                          
        FD_SET(com_socket, &rd_fds);                                            
        tv.tv_sec = 0;                                                          
        tv.tv_usec = 0;                                                         
        ret_val = select(com_socket + 1, &rd_fds, NULL, NULL, &tv);             
        if (ret_val == -1)                                                      
            perror("select()");                                                 
        else if (ret_val) {                                                     
            if (FD_ISSET(STDIN_FILENO, &rd_fds) != 0)  {                  
                memset(msg, 0, strlen(msg));
                read(STDIN_FILENO, msg, sizeof(msg));                           
                encrypted_msg = test_encrypt(fd, key, iv, msg);
                if(!encrypted_msg) {
                    printf("Encryption failed\n");
                    exit(EXIT_FAILURE);
                }
                send(com_socket, encrypted_msg, DATA_SIZE, 0);
                free(encrypted_msg);
            } else if (FD_ISSET(com_socket, &rd_fds) != 0) {                    
                memset(buf, 0, strlen(buf));
                received = recv(com_socket, buf, DATA_SIZE,0);
                if (received == 0) {
                    close(com_socket);
                    close(server_s);
                    if (close(fd)) {
                        perror("closing:\"/dev/crypto/\"");
                        return 1;
                    }
                    break;
                }
                decrypted_msg = test_decrypt(fd, key, iv, buf);
                if(!decrypted_msg) {
                    printf("Decryption failed\n");
                    exit(EXIT_FAILURE);
                }
                printf("-%s\n", decrypted_msg);
                free(decrypted_msg);
            } else {                                                            
                printf("Unexpected result\n");                                  
                exit(EXIT_FAILURE);                                             
            }                                                                   
        }  
        /*
        printf("Waiting for response...\n");

        printf("Your reply: ");
        if (fgets(msg, sizeof(msg), stdin) == NULL) {
            perror("fgets");
        }
        msg[strcspn(msg,"\n")] = 0;
        */
    }
    return 0;
}
