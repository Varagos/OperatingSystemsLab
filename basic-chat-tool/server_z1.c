#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netinet/in.h> 
#include <unistd.h>
#include <sys/select.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Please give 1 argument, Port");
        exit(EXIT_FAILURE);
    }
    uint16_t port;
    int server_s, com_socket, ret_val;
    struct sockaddr_in addr;
    fd_set rd_fds, wr_fds;
    struct timeval tv;
    char buffer[512];
    /*
    int retval;
    tv.tv_sec = 5;
    tv.tv_usec = 5;
    retval = select(1, &rd_fds, NULL, NULL, &tv);

    if (retval == -1)
        perror("select()");
    else if (retval) {
        if (FD_ISSET(STDIN_FILENO, &rd_fds) != 0)  {
            printf("Data is available now.\n");
            read(STDIN_FILENO, buffer, sizeof(buffer));
            printf("%s\n", buffer);
        } else {
            printf("Unexpected result\n");
        }
    }
    else
        printf("No data within five seconds\n");
    exit(EXIT_SUCCESS);
} 
*/


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
    char msg[1024] = {0};
    char buf[1024] = {0};
    memset(msg, 0, sizeof(msg));
    memset(buf, 0, sizeof(buf));
    printf("Accepting messages up to 1024 bytes long :=)\n");
    ssize_t received;
    while(1) {
        FD_ZERO(&rd_fds);
        FD_ZERO(&wr_fds);
        FD_SET(STDIN_FILENO, &rd_fds);
        FD_SET(com_socket, &rd_fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        ret_val = select(com_socket + 1, &rd_fds, NULL, NULL, &tv);
        if (ret_val == -1)
            perror("select()");
        else if (ret_val) {
            if (FD_ISSET(STDIN_FILENO, &rd_fds) != 0 && FD_ISSET(com_socket, &rd_fds) != 0) {
                //corner case
                memset(buf, 0, strlen(buf));
                received = recv(com_socket, buf, 1024,0);
                if (received == 0) {
                    close(com_socket);
                    close(server_s);
                    break;
                }
                printf("-%s\n", buf);
                memset(msg, 0, strlen(msg));
                read(STDIN_FILENO, msg, sizeof(msg));
                send(com_socket, msg, strlen(msg), 0);
            }else if (FD_ISSET(STDIN_FILENO, &rd_fds) != 0)  {
                memset(msg, 0, strlen(msg));
                read(STDIN_FILENO, msg, sizeof(msg));
                send(com_socket, msg, strlen(msg), 0);
            } else if (FD_ISSET(com_socket, &rd_fds) != 0) {
                memset(buf, 0, strlen(buf));
                received = recv(com_socket, buf, 1024,0);
                if (received == 0) {
                    close(com_socket);
                    close(server_s);
                    break;
                }
                printf("-%s\n", buf);
            } else {
                printf("Unexpected result\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    return 0;
}

