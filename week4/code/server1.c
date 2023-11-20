#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_ATTEMPTS 3
#define PASSWORD "hust123"
#define NEW_PASSWORD_LENGTH 16

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s PortNumber\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        perror("Error in socket creation");
        exit(1);
    }

    // Initialize server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // Bind the server socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in binding");
        exit(1);
    }

    char password[NEW_PASSWORD_LENGTH];
    int login_attempts = 0;

    while (1) {
        char buffer[NEW_PASSWORD_LENGTH];
        int n = recvfrom(server_socket, buffer, NEW_PASSWORD_LENGTH, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        buffer[n] = '\0';

        if (strcmp(buffer, "bye") == 0) {
            printf("Client signed out.\n");
            break;
        }

        if (login_attempts >= MAX_ATTEMPTS) {
            sendto(server_socket, "block account", strlen("block account"), 0, (struct sockaddr*)&client_addr, client_addr_len);
            printf("Account blocked.\n");
        } else if (strcmp(buffer, PASSWORD) == 0) {
            sendto(server_socket, "OK", strlen("OK"), 0, (struct sockaddr*)&client_addr, client_addr_len);
            printf("Login successful.\n");
            login_attempts = 0;
        } else {
            sendto(server_socket, "not OK", strlen("not OK"), 0, (struct sockaddr*)&client_addr, client_addr_len);
            printf("Login failed.\n");
            login_attempts++;
        }
    }

    close(server_socket);
    return 0;
}
