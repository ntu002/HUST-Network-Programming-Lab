#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define BUFF_SIZE 1024
#define CONNECTION_LIMIT 10

#include "linked_list.h"

LinkedList *userList;
const char *filename = "nguoidung.txt";
char usernameSignIn[MAX_USERNAME_LENGTH];
char last_username_signed_in[MAX_USERNAME_LENGTH];
char username[MAX_USERNAME_LENGTH];
char password[MAX_PASSWORD_LENGTH];
int client_flag;
int server_flag;
int sign_in_error_flag = 0;
int server_socket, rcvBytes, sendBytes;
char buff[BUFF_SIZE + 1];

int handle_client(int socket_desc);
int getInfo(char *buffer, char *username, char *password, int *client_flag)
{
    char response[BUFF_SIZE];
    int items_matched = sscanf(buffer, "'%[^']' $%d", response, client_flag);

    if (items_matched != 2)
    {
        return -1;
    }
    // printf("%i", items_matched);
    sscanf(response, "%s %s", username, password);
    return 0;
}

int assignFlag(char *buff, int *server_flag)
{
    char response[BUFF_SIZE];
    snprintf(response, BUFF_SIZE, "'%s' $%d", buff, *server_flag);
    strcpy(buff, response);
    return 0;
}

void passwordSeparate(char *newpass)
{
    int length = strlen(newpass);
    char word_string[MAX_PASSWORD_LENGTH] = "";
    char number_list[MAX_PASSWORD_LENGTH] = "";
    int word_count = 0;
    int number_count = 0;

    for (int i = 0; i < length; i++)
    {
        if (newpass[i] >= 'A' && newpass[i] <= 'Z')
        {
            word_string[word_count] = newpass[i];
            word_count++;
        }
        else if (newpass[i] >= 'a' && newpass[i] <= 'z')
        {
            word_string[word_count] = newpass[i];
            word_count++;
        }
        else if (newpass[i] >= '0' && newpass[i] <= '9')
        {
            number_list[number_count] = newpass[i];
            number_count++;
        }
    }
    strcpy(newpass, strcat(strcat(word_string, " "), number_list));
    return;
}

bool passwordFormatCheck(char *password)
{
    int length = strlen(password);
    for (int i = 0; i < length; i++)
    {
        if (!((password[i] >= 'A' && password[i] <= 'Z') ||
              (password[i] >= 'a' && password[i] <= 'z') ||
              (password[i] >= '0' && password[i] <= '9')))
        {
            return false;
        }
    }
    return true;
}

void login(LinkedList *userList, const char *filename,
           char *username, char *password, char *response,
           char *last_username_signed_in, int *sign_in_error_flag, int *server_flag)
{
    int max_attempt = 3;
    *server_flag = 1;
    Account *user = findAccount(userList, username);
    if (user == NULL)
    {
        strcpy(response, "Account does not exist!");
        return;
    }
    if (user->status == 0)
    {
        strcpy(response, "Account is blocked!");
        return;
    }
    if (strcmp(user->password, password) == 0)
    {
        if (user->status == 1)
        {
            strcpy(response, "OK");
            *server_flag = 0;
            return;
        }
        else if (user->status == 0)
        {
            strcpy(response, "Account is blocked!");
            return;
        }
        else
        {
            strcpy(response, "Account is not ready!");
            return;
        }
    }
    else
    {
        *sign_in_error_flag += 1;
        sprintf(response, "Wrong password, %d/%i tries attempted!", *sign_in_error_flag, max_attempt);
        if (strcmp(last_username_signed_in, username) == 0)
        {
            if (*sign_in_error_flag >= max_attempt)
            {
                strcpy(response, "Account is blocked!");
                user->status = 0;
                writeAccountsToFile(userList, filename);
            }
        }
        else
        {
            strcpy(last_username_signed_in, username);
            *sign_in_error_flag = 1;
        }
        return;
    }
    return;
}

void changePassword(LinkedList *userList, const char *filename,
                    char *username, char *newpass,
                    char *response, int *server_flag)
{
    *server_flag = 3;

    if (strcmp(newpass, "bye") == 0)
    {
        strcpy(response, "Goodbye ");
        strcat(response, username);
        *server_flag = 4;
        return;
    }

    Account *account = findAccount(userList, username);
    if (account == NULL)
    {
        strcpy(response, "Account does not exist");
        return;
    }

    if (!passwordFormatCheck(newpass))
    {
        strcpy(response, "Error: Wrong password format - letters and numbers only!");
        return;
    }
    else if (strcmp(account->password, newpass) == 0)
    {
        strcpy(response, "Error: New password must be different from current password!");
        return;
    }
    else
    {
        *server_flag = 2;
        strcpy(account->password, newpass);
        writeAccountsToFile(userList, filename);
        passwordSeparate(newpass);
        strcpy(response, newpass);
        return;
    }
    return;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <PortNumber>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);
    userList = createLinkedList();

    int new_socket;
    int client_socket[CONNECTION_LIMIT], max_clients = CONNECTION_LIMIT;
    int max_sd, activity, valread, sd;
    struct sockaddr_in server_address, client_address;
    fd_set readfds;

    for (int i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    // create a master socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: ");
        return 0;
    }

    // Set master socket to allow multiple connections
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        return 0;
    }

    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)))
    {
        perror("Error: ");
        return 0;
    }

    if (listen(server_socket, CONNECTION_LIMIT) == -1)
    {
        perror("Listen");
        return 1;
    }

    readAccountsFromFile(userList, filename);

    socklen_t addrlen;
    printf("Server started successfully!\n");

    while (1)
    {
        // Clear the socket set
        FD_ZERO(&readfds);

        // Add master socket to set
        FD_SET(server_socket, &readfds);
        max_sd = server_socket;

        // Add child socket to set
        for (int i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];
            if (sd > 0)
            {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd)
            {
                max_sd = sd;
            }
        }

        // Wait for activity on one of the sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0))
        {
            perror("select error");
            continue;
        }

        if (FD_ISSET(server_socket, &readfds))
        {
            if ((new_socket = accept(server_socket, (struct sockaddr *)&client_address, &addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // Add new socket to array of sockets
            for (int i = 0; i < max_clients; i++)
            {
                if (client_socket[i] == 0)
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
            // handle_client(new_socket);
        }

        // Else check for IO operations on other sockets
        for (int i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds))
            {
                // handle_client(sd);
                int rcvBytes;
                rcvBytes = recv(server_socket, buff, sizeof(buff), 0);
                printf("%s\n", buff);

                if (rcvBytes < 0)
                {
                    // Somebody disconnected, get details and print
                    getpeername(sd, (struct sockaddr *)&client_address, &addrlen);
                    printf("Host disconnected, ip %s, port %d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
                    // Close socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket[i] = 0;
                }
                else
                {
                    handle_client(sd);
                }
                // Check if closing, and read incoming message
            }
        }
    }
    freeLinkedList(userList);
    close(server_socket);
    return 0;
}

int handle_client(int socket_desc)
{
    int client_sock = socket_desc;

    // int rcvBytes;
    // rcvBytes = recv(client_sock, buff, sizeof(buff), 0);
    // printf("%s\n", buff);
    if (rcvBytes < 0)
    {
        perror("Error: ");
        return 0;
    }
    if (rcvBytes == 0)
    {
        printf("Lost connection from client.\n");
        return 0;
    }
    buff[rcvBytes] = '\0';

    getInfo(buff, username, password, &client_flag);
    printf("Client input: %s - %s\n", username, password);

    if (client_flag == 0)
    {
        login(userList, filename, username, password, buff, last_username_signed_in, &sign_in_error_flag, &server_flag);
    }
    else
    {
        changePassword(userList, filename, username, password, buff, &server_flag);
    }

    assignFlag(buff, &server_flag);

    sendBytes = send(client_sock, buff, sizeof(buff), 0);
    if (sendBytes < 0)
    {
        perror("Error: ");
        return 0;
    }
    writeAccountsToFile(userList, filename);

    return 0;
}
