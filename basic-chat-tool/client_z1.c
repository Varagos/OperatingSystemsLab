/*
 * socket-client.c
 * Simple TCP/IP communication using sockets
 *
 * Markos Girgis <mg.markosgirgis@gmail.com>
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>


int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Plese enter 2 arguments, hostname and port\n");
        return -1;
    }
    int client_s, ret_val;
    uint16_t port;
    fd_set rd_fds;
    struct timeval tv;
    if ((port = atoi(argv[2])) == 0) {
            printf("Please enter a valid port number\n");
            return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    //ensure network byte
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
    char msg[1024] = "Hello, im client!";
    char buf[1024];
    memset(msg, 0, sizeof(msg));
    memset(buf, 0, sizeof(buf));
    char end_msg[4] = "bye";
    while (strcmp(buf, "bye")) {
        FD_ZERO(&rd_fds);
        FD_SET(STDIN_FILENO, &rd_fds);
        FD_SET(client_s, &rd_fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        ret_val = select(client_s + 1, &rd_fds, NULL, NULL, &tv);
        if (ret_val == -1)
            perror("Select()");
        else if (ret_val) {
            if(FD_ISSET(STDIN_FILENO, &rd_fds) != 0 && FD_ISSET(client_s, &rd_fds) != 0) {
                //corner case
                memset(msg, 0, strlen(msg));
                if (read(STDIN_FILENO, msg, sizeof(msg)) < 0) {
                    perror("stdin read()");
                    exit(EXIT_FAILURE);
                }
                send(client_s, msg, strlen(msg), 0);
                memset(buf, 0, strlen(buf));
                recv(client_s, buf, 1024, 0);
                printf("-%s\n", buf);
            } else if(FD_ISSET(STDIN_FILENO, &rd_fds) != 0) {
                memset(msg, 0, strlen(msg));
                if (read(STDIN_FILENO, msg, sizeof(msg)) < 0) {
                    perror("stdin read()");
                    exit(EXIT_FAILURE);
                }
                send(client_s, msg, strlen(msg), 0);
            } else if (FD_ISSET(client_s, &rd_fds) != 0) {
                memset(buf, 0, strlen(buf));
                recv(client_s, buf, 1024, 0);
                printf("-%s\n", buf);
            }
        }
    }
    close(client_s);
    return 0;



}
