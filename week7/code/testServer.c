#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

void *handle_client(void *socket_desc);

int main(int argc, char *argv[]) {
    int socket_desc, client_sock, c, *new_sock;
    struct sockaddr_in server, client;

    if (argc < 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        perror("Could not create socket");
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));

    // Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind failed");
        return 1;
    }

    // Listen
    listen(socket_desc, 3);

    // Accept and incoming connection
    printf("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))) {
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if (pthread_create(&sniffer_thread, NULL, handle_client, (void*) new_sock) < 0) {
            perror("could not create thread");
            return 1;
        }
    }

    if (client_sock < 0) {
        perror("accept failed");
        return 1;
    }

    return 0;
}

void *handle_client(void *socket_desc) {
    // Handle connection here
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[BUFFER_SIZE];

    while ((read_size = recv(sock, client_message, BUFFER_SIZE, 0)) > 0) {
        // Process client message
        write(sock, client_message, strlen(client_message));
    }

    if (read_size == 0) {
        puts("Client disconnected");
    } else if (read_size == -1) {
        perror("recv failed");
    }

    free(socket_desc);
    return 0;
}
