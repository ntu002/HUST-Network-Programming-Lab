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
#define MAX_CLIENTS 5
#define MAX_LOGIN_ATTEMPTS 3
#define ACCOUNT_FILE "account.txt"

typedef struct {
    char user_id[50];
    char password[50];
    int status;
} Account;

void handle_client(int client_socket) {
    char user_id[MAX_BUFF_SIZE];
    char password[MAX_BUFF_SIZE];
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
    close(client_socket);
}

int authenticate_user(char *user_id, char *password, int *status) {
    FILE *account_file = fopen(ACCOUNT_FILE, "r");
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


