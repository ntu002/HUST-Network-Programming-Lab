#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAX_BUFF_SIZE 255

int login(char *username, char *password);
int changePassword(char *new_password);
int changeClientFlag(char *username, char *password, char *buffer, int client_flag);
int getInfo(char *buffer, int *server_flag);
void decodePassword(char *buffer);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <IP address> <Port number>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    int client_socket;
    char buffer[MAX_BUFF_SIZE];
    struct sockaddr_in server_address;

    int client_flag = 0;
    int server_flag;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: ");
        return 0;
    }

    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_ip);
    server_address.sin_port = htons(port);

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Connect error: ");
        return 1;
    }

    char username[MAX_BUFF_SIZE];
    char password[MAX_BUFF_SIZE];
    char new_password[MAX_BUFF_SIZE];
    int n;

    while (1)
    {
        if (client_flag == 1 && changePassword(new_password) == -1)
        {
            return 0;
        }

        if (client_flag == 0 && login(username, password) == -1)
        {
            return 0;
        }

        changeClientFlag(username, (client_flag == 1) ? new_password : password, buffer, client_flag);

        n = send(client_socket, buffer, sizeof(buffer), 0);
        if (n < 0)
        {
            perror("Error: ");
            return 0;
        }

        n = recv(client_socket, buffer, sizeof(buffer), 0);
        if (n < 0)
        {
            perror("Error: ");
            return 0;
        }

        if (n == 0)
        {
            printf("Lost connection from server.\n");
            return 0;
        }

        buffer[n] = '\0';

        getInfo(buffer, &server_flag);

        if (server_flag == 0) // login successfully
        {
            client_flag = 1;
        }
        else if (server_flag == 2) // change password
        {
            decodePassword(buffer);
        }
        else if (server_flag == 4) // signed out
        {
            client_flag = 0;
        }

        printf("\nReply from server: %s\n\n", buffer);
    }
    close(client_socket);
    return 0;
}


int login(char *username, char *password)
{
    printf("Enter username: ");
    scanf("%s", username);
    if (strlen(username) == 0)
    {
        return -1;
    }

    printf("Enter password: ");
    scanf("%s", password);
    if (strlen(password) == 0)
    {
        return -1;
    }
    return 0;
}

int changePassword(char *new_password)
{
    printf("Enter new password (or 'bye' to sign out): ");
    scanf("%s", new_password);
    if (strlen(new_password) == 0)
    {
        return -1;
    }
    return 0;
}

int changeClientFlag(char *username, char *password, char *buffer, int client_flag)
{
    snprintf(buffer, MAX_BUFF_SIZE, "'%s %s' $%d", username, password, client_flag);
    // printf("%s\n", buff);
    return 0;
}

int getInfo(char *buffer, int *server_flag)
{
    char ack[MAX_BUFF_SIZE];
    int items_matched = sscanf(buffer, "'%[^']' $%d", ack, server_flag);

    if (items_matched != 2)
    {
        return -1;
    }
    strcpy(buffer, ack);
    return 0;
}

void decodePassword(char *buffer)
{
    int length = strlen(buffer);
    for (int i = length; i >= 0; i--)
    {
        buffer[i + 1] = buffer[i];
    }
    buffer[0] = '\n';
    for (int i = 0; i < length; i++)
    {
        if (buffer[i] == ' ')
        {
            buffer[i] = '\n';
        }
    }
}

