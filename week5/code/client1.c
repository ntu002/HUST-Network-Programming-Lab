#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

const int NEW_PASSWORD_LENGTH = 120;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s IPAddress PortNumber\n", argv[0]);
        return 1;
    }

    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        perror("Error in socket creation");
        exit(1);
    }

    // Initialize server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    while (1) {
        char account[NEW_PASSWORD_LENGTH];
        char password[NEW_PASSWORD_LENGTH];
        char new_password[NEW_PASSWORD_LENGTH];

        printf("Enter account: ");
        scanf("%s", account);
        printf("Enter password: ");
        scanf("%s", password);

        sendto(client_socket, password, strlen(password), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

        char response[NEW_PASSWORD_LENGTH];
        int n = recvfrom(client_socket, response, NEW_PASSWORD_LENGTH, 0, NULL, NULL);
        response[n] = '\0';

        if (strcmp(response, "OK") == 0) {
            printf("Login successful.\n");
            while (1) {
                printf("Enter new password (or 'bye' to sign out): ");
                scanf("%s", new_password);
                sendto(client_socket, new_password, strlen(new_password), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

                if (strcmp(new_password, "bye") == 0) {
                    printf("Signed out.\n");
                    break;
                }
            }
            break;
        } else if (strcmp(response, "not OK") == 0) {
            printf("Login failed. Please try again.\n");
        } else {
            printf("Account blocked.\n");
            break;
        }
    }

    close(client_socket);
    return 0;
}
