#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "erproc.h"

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(34543);
    Inet_pton(AF_INET, "127.0.0.1", &adr.sin_addr);
    Connect(fd, (struct sockaddr*)&adr, sizeof adr);
    char buf[256];
    ssize_t nread;
    char inp[100];
    while(1){
        memset(inp, 0, strlen(inp));
        printf("Client: ");
        fgets(inp, 90, stdin);
        if(strcmp(inp, "quit") == 10) break;
        write(fd, inp, strlen(inp));
        nread = read(fd, buf, 256);
        if (nread == -1) {
            perror("Read Failed");
            exit(EXIT_FAILURE);
        }
        if (nread == 0) {
            printf("EOF occured");
        }
        write(STDOUT_FILENO, "Server: ", 8);
        write(STDOUT_FILENO, buf, nread);
    }
    close(fd);
    return 0; 
}