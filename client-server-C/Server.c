#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "erproc.h"

#define MAX_CLIENTS 5

volatile int active_clients = 0;
pthread_mutex_t client_count_mutex = PTHREAD_MUTEX_INITIALIZER;
int server;

void* handle_client(void *arg) {
    int fd = *(int *)arg;
    ssize_t nread;
    char buf[256];
    
    pthread_mutex_lock(&client_count_mutex);
    active_clients++;
    pthread_mutex_unlock(&client_count_mutex);

    while (1) {
        nread = read(fd, buf, sizeof(buf));
        if (nread == -1) {
            perror("Read Failed");
            break;
        }

        if (nread == 0) {
            printf("Client disconnected\n");
            break;
        }

        write(STDOUT_FILENO, buf, nread);
        write(fd, buf, nread);
    }

    close(fd);

    pthread_mutex_lock(&client_count_mutex);
    active_clients--;
    int clients = active_clients;
    pthread_mutex_unlock(&client_count_mutex);

    if (clients == 0) {
        printf("No more clients connected. Closing server.\n");
        close(server);
        exit(EXIT_SUCCESS);
    }

    free(arg);
    pthread_exit(NULL);
}

int main() {
    server = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port = htons(34543);
    Bind(server, (struct sockaddr *)&adr, sizeof(adr));
    Listen(server, 5);
    socklen_t adrlen = sizeof(adr);
    pthread_t thread_id;

    printf("Server listening on port %d...\n", ntohs(adr.sin_port));

    while (1) {
        int* fd = (int*)malloc(sizeof(int));
        *fd = Accept(server, (struct sockaddr *)&adr, &adrlen);
        if (active_clients >= MAX_CLIENTS) {
            perror("Maximum Clients");
            continue;
        }
        if (*fd == -1) {
            perror("Accept Failed");
            continue;
        }

        printf("New client connected: %s:%d\n", inet_ntoa(adr.sin_addr), ntohs(adr.sin_port));

        if (pthread_create(&thread_id, NULL, handle_client, (void *)fd) != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }

        pthread_detach(thread_id);
    }

    close(server);
    return 0;
}
