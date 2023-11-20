#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 5
#define MAX_LOGIN_ATTEMPTS 3
#define ACCOUNT_FILE "account.txt"

typedef struct {
    char user_id[50];
    char password[50];
    int status;
} Account;

void handle_client(int client_socket);
int authenticate_user(char *user_id, char *password, int *status);
void update_account_status(char *user_id, int new_status);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_socket, client_socket, port;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[1]);

    // Initialize server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Error listening");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);

    while (1) {
        // Accept a connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        // Fork a child process to handle the client
        if (fork() == 0) {
            // Close the original socket in the child process
            close(server_socket);

            // Handle the client
            handle_client(client_socket);

            // Close the client socket in the child process
            close(client_socket);

            // Terminate the child process
            exit(EXIT_SUCCESS);
        } else {
            // Close the client socket in the parent process
            close(client_socket);
        }
    }

    // Close the server socket
    close(server_socket);

    return 0;
}

void handle_client(int client_socket) {
    char user_id[50];
    char password[50];
    int login_attempts = 0;

    while (1) {
        // Receive user_id and password from the client
        recv(client_socket, user_id, sizeof(user_id), 0);
        recv(client_socket, password, sizeof(password), 0);

        int status;
        int auth_result = authenticate_user(user_id, password, &status);

        // Send authentication result to the client
        send(client_socket, &auth_result, sizeof(auth_result), 0);

        if (auth_result == 1) {
            printf("User %s authenticated successfully.\n", user_id);
            break;
        } else {
            login_attempts++;
            printf("Authentication failed for user %s (Attempt %d).\n", user_id, login_attempts);

            if (login_attempts >= MAX_LOGIN_ATTEMPTS) {
                printf("Too many login attempts. Locking account for user %s.\n", user_id);
                update_account_status(user_id, 0);  // Lock the account
                break;
            }
        }
    }

    // Perform other actions with the authenticated client as needed
    // ...

    // Close the connection with the client
    close(client_socket);
}

int authenticate_user(char *user_id, char *password, int *status) {
    FILE *account_file = fopen("account.txt", "r");
    if (!account_file) {
        perror("Error opening account file");
        exit(EXIT_FAILURE);
    }

    Account account;

    while (fscanf(account_file, "%s %s %d", account.user_id, account.password, &account.status) == 3) {
        if (strcmp(account.user_id, user_id) == 0 && strcmp(account.password, password) == 0) {
            *status = account.status;
            fclose(account_file);
            return 1; // Authentication successful
        }
    }

    fclose(account_file);
    return 0; // Authentication failed
}

void update_account_status(char *user_id, int new_status) {
    FILE *account_file = fopen(ACCOUNT_FILE, "r");
    if (!account_file) {
        perror("Error opening account file");
        exit(EXIT_FAILURE);
    }

    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) {
        perror("Error opening temporary file");
        fclose(account_file);
        exit(EXIT_FAILURE);
    }

    Account account;

    while (fscanf(account_file, "%s %s %d", account.user_id, account.password, &account.status) == 3) {
        if (strcmp(account.user_id, user_id) == 0) {
            fprintf(temp_file, "%s %s %d\n", account.user_id, account.password, new_status);
        } else {
            fprintf(temp_file, "%s %s %d\n", account.user_id, account.password, account.status);
        }
    }

    fclose(account_file);
    fclose(temp_file);

    // Replace the original account file with the temporary file
    if (rename("temp.txt", ACCOUNT_FILE) == -1) {
        perror("Error renaming file");
        exit(EXIT_FAILURE);
    }
}
