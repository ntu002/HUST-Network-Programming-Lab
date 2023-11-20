#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

#define MAX_BUFF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket\n");
        exit(EXIT_FAILURE);
    }

    // Initialize server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server\n");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    char user_id[MAX_BUFF_SIZE];
    char password[MAX_BUFF_SIZE];
    int result;

    // Get user_id and password from the user
    printf("Enter username: ");
    scanf("%s", user_id);
    printf("Enter password: ");
    scanf("%s", password);

    // Send user_id and password to the server
    send(client_socket, user_id, sizeof(user_id), 0);
    send(client_socket, password, sizeof(password), 0);

    // Receive authentication result from the server
    recv(client_socket, &result, sizeof(result), 0);

    if (result == 1) {
        printf("[+]Authentication successful.\n");
    } else {
        printf("[+]Authentication failed.\n");
    }

    // Close the connection with the server
    close(client_socket);

    return 0;
}
