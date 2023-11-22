#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE], server_reply[BUFFER_SIZE];

    if (argc < 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
    }

    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed");
        return 1;
    }

    // Keep communicating with server
    while (1) {
        printf("Enter message: ");
        scanf("%s", message);

        // Send some data
        if (send(sock, message, strlen(message), 0) < 0) {
            puts("Send failed");
            return 1;
        }

        // Receive a reply from the server
        if (recv(sock, server_reply, BUFFER_SIZE, 0) < 0) {
            puts("recv failed");
            break;
        }

        puts("Server reply:");

        puts(server_reply);

    }
    close(sock);
    return 0;
    
}